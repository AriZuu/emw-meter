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
#include <eshell.h>

#include "wwd_wifi.h"

#include "potato-bus.h"
#include "potato-json.h"
#include "emw-meter.h"

int16_t outsideStats[MAX_STATS];
int16_t insideStats[MAX_STATS];
int16_t powerStats[MAX_STATS];

extern wiced_mac_t   myMac;

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

static PbClient client;

/*
 * Configure mqtt client.
 */
static int mqtt(EshContext* ctx)
{
  char* server = eshNamedArg(ctx, "server", true);

  eshCheckNamedArgsUsed(ctx);
  eshCheckArgsUsed(ctx);
  if (eshArgError(ctx) != EshOK)
    return -1;

  uosConfigSet("mqtt.server", server);
  return 0;
}

const EshCommand mqttCommand = {
  .flags = 0,
  .name = "mqtt",
  .help = "--server servername configure mqtt client",
  .handler = mqtt
}; 

static JsonContext ctx;

static float findValue(char* msg, const char* location, const char* sensor)
{
  JsonNode* node;

  node = jsonParse(&ctx, msg);
  if (node == NULL)
    return MISSING_VALUE;

  node = jsonFind(node, "locations");
  if (node == NULL)
    return MISSING_VALUE;

  node = jsonFind(node, location);
  if (node == NULL)
    return MISSING_VALUE;

  node = jsonFind(node, sensor);
  if (node == NULL)
    return MISSING_VALUE;

  if (!jsonIsArray(node))
    return MISSING_VALUE;

  float last = MISSING_VALUE;

  while ((node = jsonNext(node)) != NULL) {

    if (!jsonIsNumber(node))
      return MISSING_VALUE;

    last = jsonReadDouble(node);
  }

  return last;
}

static void potatoTask(void* arg)
{
  char jsonBuf[100];
  int timeoutCount;

  while (true) {

    sprintf(jsonBuf, "EMW%02x%02x%02x%02x%02x%02x", myMac.octet[0],
                myMac.octet[1], myMac.octet[2], myMac.octet[3],
                myMac.octet[4], myMac.octet[5]);

    PbConnect cd = {};
    cd.clientId = jsonBuf;
    cd.keepAlive= 60;
    const char* server = uosConfigGet("mqtt.server");
  
    if (server == NULL) {

      posTaskSleep(MS(10000));
      continue;
    }

    if (pbConnectURL(&client, server, &cd) < 0) {

      printf("potato: connect failed.\n");
      posTaskSleep(MS(10000));
      continue;
    }

    PbPublish pub = {};
  
/*
 * Subscribe topics we are interested in.
 */
    PbSubscribe sub = {};
    sub.topic = "ts/davis";
    pbSubscribe(&client, &sub);

    sub.topic = "ts/emeter";
    pbSubscribe(&client, &sub);

    sub.topic = "forecast/fmi";
    pbSubscribe(&client, &sub);

    int type;
 
/*
 * Poll mqtt events.
 */ 
    timeoutCount = 0;
    while((type = pbEvent(&client))) {

      if (type == PB_TIMEOUT) {
/*
 * When receive times out, either send out room temperature
 * that has been read or at least keepalive message.
 */
        if (!IS_MISSING(insideTemperature) && timeoutCount >= 5) {

          timeoutCount = 0;
          snprintf(jsonBuf, sizeof(jsonBuf),
                   "{\"locations\":{\"inside\":{\"livingRoom\":{\"temperature\":%5.1lf}}}}", insideTemperature);
          pub.message = (uint8_t*)jsonBuf;
          pub.len = strlen(jsonBuf);
          pub.topic = "sensor/emw-meter";
          if (pbPublish(&client, &pub) < 0)
            break;
        }
        else
          if (pbPing(&client) < 0)
            break;

        ++timeoutCount;
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
        pub.message[pub.len] = '\0';

        if (!strcmp(pub.topic, "ts/emeter")) {

          potatoLock();
          power = findValue((char*)pub.message, "emeter", "power");
          memmove(powerStats, powerStats + 1, (MAX_STATS - 1) * sizeof(powerStats[0]));
          powerStats[MAX_STATS - 1] = power;
          potatoUnlock();
        }

        if (!strcmp(pub.topic, "forecast/fmi")) {

          JsonNode* root = jsonParse(&ctx, (char*)pub.message);

          JsonNode* sym = jsonFind(root, "weatherSymbol3");

          potatoLock();
          if (!jsonIsNumber(sym)) {
 
            weatherSymbol3 = 0;
            weatherSymbol  = 0;
          }
          else {

            weatherSymbol3 = jsonReadInteger(sym);
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

          potatoUnlock();
        }

        if (!strcmp(pub.topic, "ts/davis")) {

          potatoLock();
          outsideTemperature = findValue((char*)pub.message, "outside", "temperature");
          memmove(outsideStats, outsideStats + 1, (MAX_STATS - 1) * sizeof(outsideStats[0]));
          outsideStats[MAX_STATS - 1] = round(outsideTemperature * 10);

          if (!IS_MISSING(insideTemperature)) {

            memmove(insideStats, insideStats + 1, (MAX_STATS - 1) * sizeof(insideStats[0]));
            insideStats[MAX_STATS - 1] = round(insideTemperature * 10);
          }

          potatoUnlock();
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
