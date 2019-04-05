#
# Copyright (c) 2019, Ari Suutari <ari@stonepile.fi>.
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

cmake_minimum_required(VERSION 3.10)

set(PORT cortex-m)
set(CPU stm32)
set(BUNDLE_FIRMWARE 1)

set(WICED_PLATFORM EMW3165)
set(WICED_CHIP 43362)
set(WICED_CHIP_REVISION A2)
set(WICED_MCU STM32F4xx)
set(WICED_BUS SDIO)
set(CMSIS_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../cmsis-ports/stm32f4xx)
set(STM32_CHIP STM32F411xE)
set(CORTEX m4)
set(LD_SCRIPTS emw3165.ld)
set(NANO 1)
set(STM32_DEFINES HSE_VALUE=26000000)

set(ROMFILES romfiles.c)

include(../picoos/cmake/ToolchainInit.cmake)

project(emw-meter)

set(DIR_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/config)
set(SRC  main.c
         startup.c
         ${ROMFILES}
         sta.c
         ap.c
         led.c
         devtree.c
         spibus.c
         sensor.c
         ssd1306.c
         gui.c
         ugui.c
         potato.c
         fonts/BebasNeue_17X34.c
         fonts/FMI_weather_34X33.c)

add_peer_directory(${PICOOS_DIR})
add_peer_directory(../picoos-lwip)
add_peer_directory(../wiced-driver)
add_peer_directory(../eshell)
add_peer_directory(../picoos-ow)
add_peer_directory(../potato-bus)
add_peer_directory(../picoos-micro)
add_peer_directory(../picoos-micro-spiffs)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} wiced-driver picoos-lwip eshell picoos-ow potato-bus picoos-micro-spiffs picoos-micro picoos m)
target_compile_definitions(${PROJECT_NAME} PRIVATE BUNDLE_FIRMWARE=${BUNDLE_FIRMWARE})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND  arm-none-eabi-size ${PROJECT_NAME}.elf)
target_include_directories(${PROJECT_NAME}
  PRIVATE . fonts)


set(FIRMWARE ${WICED_SDK}/resources/firmware/${WICED_CHIP}/${WICED_CHIP}${WICED_CHIP_REVISION}.bin)

add_custom_command(
  OUTPUT ${ROMFILES}
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/gen_romfs.sh ${BUNDLE_FIRMWARE} ${FIRMWARE} ${CMAKE_CURRENT_SOURCE_DIR} > ${ROMFILES}
  COMMENT "Generating RomFS.")

