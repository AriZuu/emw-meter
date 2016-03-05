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

/*
 * SSD1306 commands from datasheet.
 */
#define CMD_CONTRAST_CONTROL         0x81
#define CMD_ENTIRE_DISPLAY_ON        0xa4
#define CMD_NORMAL_OR_INVERSE        0xa6
#define CMD_DISPLAY_ON_OFF           0xae
#define CMD_CLOCK_SETUP              0xd5
#define CMD_MULTIPLEX_RATIO          0xa8
#define CMD_DISPLAY_OFFSET           0xd3
#define CMD_DISPLAY_START_LINE       0x40
#define CMD_CHARGE_PUMP_SETTING      0x8d
#define CMD_MEM_ADDR_MODE            0x20
#define CMD_SEGMENT_REMAP            0xa0
#define CMD_OUTPUT_SCAN_DIR          0xc0
#define CMD_PIN_HW_CONF              0xda
#define CMD_PRECHARGE_PERIOD         0xd9
#define CMD_VCOMH_DESELECT_LVL       0xdb
#define CMD_LOW_COLUMN               0x00
#define CMD_HIGH_COLUMN              0x10

static UG_GUI gui;

// Initialization loosed based on instructions
// in UG-2832HSWEG04.pdf datasheet and example
// pset function by Achim Döbler (the uGUI author).

static const uint8_t init[] =
{

CMD_DISPLAY_ON_OFF | 0x0,          // display off
CMD_CLOCK_SETUP,                   // set display clock divider & osc freq
    0x80,                              //  100 frames / sec
    CMD_MULTIPLEX_RATIO,               // set multiplex ratio
    0x3f,                              //  1/64 duty
    CMD_DISPLAY_OFFSET,                // set display offset
    0x0,                               //  no offset
    CMD_DISPLAY_START_LINE | 0x0,      // set display start line
    CMD_CHARGE_PUMP_SETTING,           // charge pump
    0x14,                              //  turn on
    CMD_MEM_ADDR_MODE,                 // set memory addressing
    0x0,                               //  page addressing, 0x0=horiz
    CMD_SEGMENT_REMAP | 0x1,           // segment remapping
    CMD_OUTPUT_SCAN_DIR | 0x8,         // set scan direction
    CMD_PIN_HW_CONF,                   // com pins hw config
    0x12,                              //
    CMD_CONTRAST_CONTROL,              // set display contrast
    0xcf,                              //
    CMD_PRECHARGE_PERIOD,              // set pre charge period
    0xf1,                              //
    CMD_VCOMH_DESELECT_LVL,            // set Vcomh deselect leve
    0x30,                              //
    CMD_ENTIRE_DISPLAY_ON | 0x0,       // display on/off
    CMD_NORMAL_OR_INVERSE | 0x0,       // not inverse
    CMD_DISPLAY_ON_OFF | 0x1           // dispplay on
};

static void writeCmd(uint8_t cmd);
static void writeData(uint8_t cmd);

static unsigned char buffer[128 * 8]; // 128x64 1BPP OLED

static void drawPixel(UG_S16 x, UG_S16 y, UG_COLOR c)
{
  unsigned int p;

  if (x > 127 || y > 63)
    return;

  p = x + (y / 8) * 128;

  if (c)
    buffer[p] |= 1 << (y % 8);
  else
    buffer[p] &= ~(1 << (y % 8));
}

void guiUpdateScreen(void)
{
  UG_Update();

  // Start at first pixel.
  writeCmd(CMD_LOW_COLUMN | 0x0);
  writeCmd(CMD_HIGH_COLUMN | 0x0);
  writeCmd(CMD_DISPLAY_START_LINE | 0x0);

  int i;
  for (i = 0; i < 1024; i++)
    writeData(buffer[i]);
}

#define RST(x) GPIO_WriteBit(GPIOB, GPIO_Pin_1, x)
#define C_D(x) GPIO_WriteBit(GPIOA, GPIO_Pin_12, x)

static void writeCmd(uint8_t cmd)
{
  C_D(Bit_RESET);
  uosSpiBegin(&oledDev);
  uosSpiXchg(&oledDev, cmd);
  uosSpiEnd(&oledDev);
}

static void writeData(uint8_t cmd)
{
  C_D(Bit_SET);
  uosSpiBegin(&oledDev);
  uosSpiXchg(&oledDev, cmd);
  uosSpiEnd(&oledDev);
}

void guiInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  // C_D pin
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // RST pin
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  C_D(Bit_RESET);
  RST(Bit_SET);
  posTaskSleep(MS(100));
  RST(Bit_RESET);

  posTaskSleep(MS(100));
  RST(Bit_SET);
  posTaskSleep(MS(100));

  const uint8_t* cmd;
  unsigned int i;

  cmd = init;
  for (i = 0; i < sizeof(init); i++, cmd++)
    writeCmd(*cmd);

  UG_Init(&gui, drawPixel, 128, 64);

  UG_FillScreen(C_BLACK);

  UG_SetBackcolor(C_BLACK);
  UG_SetForecolor(C_WHITE);
  UG_FontSelect(&FONT_6X8);

  UG_PutString(0, 0, "Pico]OS");
  guiUpdateScreen();
}
