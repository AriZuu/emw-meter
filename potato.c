/*
 * Copyright (c) 2016, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <picoos.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "mjson.h"

#include "potato-bus.h"
#include "emw-meter.h"

int16_t outsideStats[MAX_STATS];
int16_t insideStats[MAX_STATS];
int16_t powerStats[MAX_STATS];

typedef struct {

  char ch;
  int8_t weatherSymbol3;
} SymbolFontMap;

/*
 * Map ascii codes in weather symbol font into
 * actual weather symbol codes.
 */
const SymbolFontMap fontMap[] = {

  { 'A', 1 },
  { 'B', 2 },
  { 'C', 3 },
  { 'D', 21 },
  { 'E', 22 },
  { 'F', 23 },
  { 'G', 31 },
  { 'H', 32 },
  { 'I', 33 },
  { 'J', 41 },
  { 'K', 42 },
  { 'L', 43 },
  { 'M', 51 },
  { 'N', 52 },
  { 'O', 53 },
  { 'P', 61 },
  { 'Q', 62 },
  { 'R', 63 },
  { 'S', 64 },
  { 'T', 71 },
  { 'U', 72 },
  { 'V', 73 },
  { 'W', 81 },
  { 'X', 82 },
  { 'Y', 83 },
  { 'Z', 91 },
  { 'a', 92 },
};

double outsideTemperature = MISSING_VALUE;
double insideTemperature = MISSING_VALUE;
int power = MISSING_VALUE;
int weatherSymbol3 = 0;
char weatherSymbol = 0;

/*
 * Parse temperature, electrical power and weather forecast symbol from
 * json data that is received from MQTT broker.
 */
static const struct json_attr_t jsonAttrs[] = {

  { "outsideTemperature", t_real, .addr.real = &outsideTemperature, .nodefault = true },
  { "power", t_integer, .addr.integer = &power, .nodefault = true },
  { "weatherSymbol3", t_integer, .addr.integer = &weatherSymbol3, .nodefault = true },
  { NULL, t_ignore, .addr.integer = NULL }, // Ignore attributes that are not known
  { NULL }
};
  
static PbClient client;

static void potatoTask(void* arg)
{
  char jsonBuf[80];

  while (true) {

    PbConnect cd = {};
    cd.clientId = "test";
    cd.keepAlive= 60;
  
    if (pbConnect(&client, "mqtt.stonepile.fi", "1883", &cd) < 0) {

      printf("potato: connect failed.\n");
      posTaskSleep(MS(10000));
      continue;
    }

    PbPublish pub = {};
  
/*
 * Subscribe topics we are interested in.
 */
    PbSubscribe sub = {};
    sub.topic = "sensors/davis";
    pbSubscribe(&client, &sub);

    sub.topic = "sensors/power-meter";
    pbSubscribe(&client, &sub);

    sub.topic = "forecast/fmi";
    pbSubscribe(&client, &sub);

    int type;
 
/*
 * Poll mqtt events.
 */ 
    while((type = pbEvent(&client))) {

      if (type == PB_TIMEOUT) {
/*
 * When receive times out, either send out room temperature
 * that has been read or at least keepalive message.
 */
        if (!IS_MISSING(insideTemperature)) {

          sprintf(jsonBuf, "{\"temperature\":%5.1lf}", insideTemperature);
          pub.message = (uint8_t*)jsonBuf;
          pub.len = strlen(jsonBuf);
          pub.topic = "sensors/emw-meter";
          if (pbPublish(&client, &pub) < 0)
            break;
        }
        else
          if (pbPing(&client) < 0)
            break;

        continue;
      }

      if (type == PB_TOOBIG) {
   
        printf ("potato: too big packet\n");
        break;
      }
  
      if (type < 0)
        break;
 /*
  * Handle incoming data.
  */ 
      if (type == PB_MQ_PUBLISH) {
  
        pbReadPublish(&client.packet, &pub);
  
        int st;

        pub.message[pub.len] = '\0';
        potatoLock();
        st = json_read_object((char*)pub.message, jsonAttrs, NULL);
        potatoUnlock();
        if (st != 0) {

           printf ("json err %s\n", json_error_string(st));
           continue;
        }

        if (!strcmp(pub.topic, "sensors/power-meter")) {

          memmove(powerStats, powerStats + 1, (MAX_STATS - 1) * sizeof(powerStats[0]));
          powerStats[MAX_STATS - 1] = power;
        }

        if (!strcmp(pub.topic, "forecast/fmi")) {

          unsigned int i;
          bool found = false;

          for (i = 0; i < sizeof(fontMap) / sizeof(SymbolFontMap); i++) {

            if (fontMap[i].weatherSymbol3 == weatherSymbol3) {

              weatherSymbol = fontMap[i].ch;
              found = true;
              break;
            }
          }

          if (!found)
            weatherSymbol = 0;
        }

        if (!strcmp(pub.topic, "sensors/davis")) {

          memmove(outsideStats, outsideStats + 1, (MAX_STATS - 1) * sizeof(outsideStats[0]));
          outsideStats[MAX_STATS - 1] = round(outsideTemperature * 10);

          if (!IS_MISSING(insideTemperature)) {

            memmove(insideStats, insideStats + 1, (MAX_STATS - 1) * sizeof(insideStats[0]));
            insideStats[MAX_STATS - 1] = round(insideTemperature * 10);
          }
        }
      }
    }
  
    pbDisconnect(&client);
    printf("potato: disconnected.\n");
    posTaskSleep(MS(10000));
  }
}

static bool started = false;
static POSMUTEX_t potatoMutex;
void potatoInit()
{
  potatoMutex = posMutexCreate();
  int i;
  int16_t* sp;

  for (sp = outsideStats, i = 0; i < MAX_STATS; i++, sp++)
    *sp = MISSING_VALUE;

  for (sp = insideStats, i = 0; i < MAX_STATS; i++, sp++)
    *sp = MISSING_VALUE;

  for (sp = powerStats, i = 0; i < MAX_STATS; i++, sp++)
    *sp = MISSING_VALUE;
}

void potatoStart()
{
  if (started)
    return;

  started = true;

  nosTaskCreate(potatoTask, NULL, 2, 4000, "PotatoBus");
}

void potatoLock()
{
  posMutexLock(potatoMutex);
}

void potatoUnlock()
{
  posMutexUnlock(potatoMutex);
}
