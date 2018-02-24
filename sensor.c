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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "devtree.h"

#include <picoos-ow.h>
#include <temp10.h>

static void sensorAddressStr(char* buf, uint8_t* serialNum)
{
  int i;

  *buf = '\0';
  for (i = 7; i >= 0; i--) {

    sprintf(buf + strlen(buf), "%02X", (int)serialNum[i]);
    if (i > 0)
      strcat(buf, "-");
   }
}

void init1Wire()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOB, GPIO_Pin_10);
  owInit();

  if (!owAcquire(0, NULL)) {

    printf("OneWire: owAcquire failed\n");
    return;
  }

  owRelease(0);
  printf("OneWire OK.\n");
}

float read1Wire()
{
  int	  result;
  uint8_t serialNum[8];
  char buf[40];
  float value = -273;

  if (!owAcquire(0, NULL)) {

    printf("OneWire: owAcquire failed\n");
    return value;
  }

  result = owFirst(0, TRUE, FALSE);

  if (result) {

    owSerialNum(0, serialNum, TRUE);
    sensorAddressStr(buf, serialNum);
    if (!ReadTemperature(0, serialNum, &value) || value >= 85.0)
      value = MISSING_VALUE;
  }

  owRelease(0);
  return value;
}
