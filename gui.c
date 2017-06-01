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
#include <picoos-u.h>
#include <stdint.h>
#include <stdbool.h>
#include "ugui.h"
#include "devtree.h"
#include "emw-meter.h"
#include "BebasNeue_17X34.h"
#include "FMI_weather_34X33.h"

#define VALUE_POS 0, 0
#define FORECAST_POS 90, 0
#define LABEL_POS 0, 37
#define MAX_POS   0, 46
#define MIN_POS   0, 55

static void guiTask(void* arg)
{
  int meas = -1;
  float t;
  char buf[20];
  int16_t* stats = NULL;

  while (true) {

    ++meas;
    if (meas > 2)
      meas = 0;

    UG_FillScreen(C_BLACK);

    UG_SetBackcolor(C_BLACK);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_6X8);

    switch (meas)
    {
    case 0:
      t = read1Wire();
      potatoLock();
      insideTemperature = t;
      stats = insideStats;
      potatoUnlock();
      if (IS_MISSING(t))
        strcpy(buf, "--.-");
      else
        sprintf(buf, "%1.1f", t);

      UG_PutString(LABEL_POS, "IN      C");
      break;

    case 1:
      potatoLock();
      t = outsideTemperature;
      stats = outsideStats;
      potatoUnlock();
      if (IS_MISSING(t))
        strcpy(buf, "--.-");
      else
        sprintf(buf, "%1.1f", t);
      
      UG_PutString(LABEL_POS, "OUT     C");
      break;

    case 2:
      potatoLock();
      t = power;
      stats = powerStats;
      potatoUnlock();
      if (IS_MISSING(t))
        strcpy(buf, "----");
      else
        sprintf(buf, "%1.0f", t);

      UG_PutString(LABEL_POS, "PWR      W");
      break;
    }

    UG_FontSelect(&font_BebasNeue_17X34); //&FONT_22X36);
    UG_PutString(VALUE_POS, buf);

    if (weatherSymbol && (meas == 0 || meas == 1)) {

      UG_FontSelect(&font_FMI_weather_34X33);
      UG_PutChar(weatherSymbol, FORECAST_POS, C_WHITE, C_BLACK);
    }

    if (stats != NULL) {

      int x;
      int v;
      int min = 32767;
      int max = -32768;
      int i;
      float scale;
      int sum = 0;
      int cnt = 0;
      float avg;

      for (i = 0; i < MAX_STATS; i++) {

        if (IS_MISSING(stats[i]))
          continue;

        sum += stats[i];
        cnt++;
        
        if (stats[i] > max)
          max = stats[i];

        if (stats[i] < min)
          min = stats[i];
      }

      UG_FontSelect(&FONT_6X8);


      if (cnt) {

        avg = ((float)sum) / cnt;
        switch (meas) {
        case 0:
        case 1:
          sprintf(buf, "MAX %5.1f", max * 0.1);
          UG_PutString(MAX_POS, buf);

          sprintf(buf, "MIN %5.1f", min * 0.1);
          UG_PutString(MIN_POS, buf);

          if (max - min < 50) {

            max = avg + 25;
            min = avg - 25;
          }

          break;

        case 2:
          sprintf(buf, "MAX %5d", max);
          UG_PutString(MAX_POS, buf);

          sprintf(buf, "MIN %5d", min);
          UG_PutString(MIN_POS, buf);

          if (max < 2000)
            max = 2000;

          break;
        }

        scale = 25.0 / (float) (max - min);
        int prevX;
        int prevV = MISSING_VALUE;
        for (x = 68, i = 0; i < MAX_STATS; i++, x++) {

          v = stats[i];
          if (IS_MISSING(v))
            continue;

          v = (v - min) * scale;

          if (meas == 2) {

            UG_DrawLine(x, 63, x, 63 - v, C_WHITE);
          }
          else {

             if (!IS_MISSING(prevV)) {

               UG_DrawLine(prevX, 63 - prevV, x, 63 - v, C_WHITE);
             }

             prevX = x;
             prevV = v;
          }
        }
      }
    }

    guiUpdateScreen();
    posTaskSleep(MS(5000));
  }
}

void guiStart()
{
  nosTaskCreate(guiTask, NULL, 2, 3000, "GUI");
}

