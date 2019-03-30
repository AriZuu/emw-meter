#
# Copyright (c) 2012-2015, Ari Suutari <ari@stonepile.fi>.
# All rights reserved. 
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. The name of the author may not be used to endorse or promote
#     products derived from this software without specific prior written
#     permission. 
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

RELROOT = ../picoos/

PORT = cortex-m
CPU = stm32

BUILD ?= RELEASE

WICED_PLATFORM       = EMW3165
WICED_CHIP           = 43362
WICED_CHIP_REVISION  = A2
WICED_MCU            = STM32F4xx
WICED_BUS            = SDIO
CMSIS_MODULES	     = $(CURRENTDIR)/../cmsis-ports/stm32f4xx
STM32_CHIP	     = STM32F411xE
CORTEX               = m4
LD_SCRIPTS 	     = emw3165.ld

export CORTEX


include $(RELROOT)make/common.mak

NANO = 1
TARGET = emw-meter
SRC_TXT =	 main.c \
                 startup.c \
                 romfiles.c \
                 sta.c \
                 ap.c \
                 led.c \
                 devtree.c \
                 spibus.c \
                 sensor.c \
                 ssd1306.c \
                 gui.c \
                 ugui.c \
                 potato.c \
                 fonts/BebasNeue_17X34.c \
                 fonts/FMI_weather_34X33.c
SRC_HDR = 
SRC_OBJ =
SRC_LIB = -lm
DIR_USRINC = fonts
CDEFINES += BUNDLE_FIRMWARE=1

# CMSIS setup
STM32_DEFINES = HSE_VALUE=26000000

MODULES += ../picoos-lwip ../wiced-driver ../eshell ../picoos-ow ../potato-bus

DIR_CONFIG = $(CURRENTDIR)/config
DIR_OUTPUT = $(CURRENTDIR)/bin
MODULES +=  ../picoos-micro ../picoos-micro-spiffs

POSTLINK1 = arm-none-eabi-size $(TARGETOUT)

# ---------------------------------------------------------------------------
# BUILD THE EXECUTABLE
# ---------------------------------------------------------------------------

include $(MAKE_OUT)

romfiles.c:
	sh gen_romfs.sh > romfiles.c

FONT=BebasNeue

$(FONT)_17X34.c $(FONT)_17X34.h:	$(FONT).otf
	../ttf2ugui/ttf2ugui --minchar 45 --maxchar 57 --size 46 --font=$(FONT).otf --dump

FMI_weather_34X33.c FMI_weather_34X33.h: FMI_weather.ttf
	../ttf2ugui/ttf2ugui --minchar 65 --maxchar 97 --size 33 --font=FMI_weather.ttf --dump
