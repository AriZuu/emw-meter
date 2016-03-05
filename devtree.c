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
#include <picoos-u-spiffs.h>
#include <stdbool.h>
#include <inttypes.h>

#include "devtree.h"


/*
 * Configuration settings for SPI1 bus.
 */
const BusConf spi1BusConf = {
  .base = {
    .init    = spiInit,
    .cs      = spiCs,
    .control = spiControl,
    .xchg    = spiXchg,
  },
  .spi = SPI1
};

/*
 * Configuration settings for SPI4 bus.
 */
const BusConf spi4BusConf = {
  .base = {
    .init    = spiInit,
    .cs      = spiCs,
    .control = spiControl,
    .xchg    = spiXchg,
  },
  .spi = SPI4
};

/*
 * SPI devices: oled display.
 */
const UosSpiDevConf oledConf = {

  .cs = {

    .gpioPort = GPIOA,
    .gpioPin  = GPIO_Pin_5
  }
};

/*
 * SPI devices: flash chip.
 */
const UosFlashConf flashConf = {

  .base = {
    .cs = {

      .gpioPort = GPIOA,
      .gpioPin  = GPIO_Pin_15
    }
  },
  .spiflash = {
// MX25L1606E 
    .cf = {
      .sz                   = 1024 * 1024 * 2,
      .page_sz              = 256,
      .addr_sz              = 3,
      .addr_dummy_sz        = 0,
      .addr_endian          = SPIFLASH_ENDIANNESS_BIG,
      .sr_write_ms          = 10,
      .page_program_ms      = 3,
      .block_erase_4_ms     = 50,
      .block_erase_8_ms     = 0,
      .block_erase_16_ms    = 0,
      .block_erase_32_ms    = 0,
      .block_erase_64_ms    = 500,
      .chip_erase_ms        = 20000
    },
    .cmds = SPIFLASH_CMD_TBL_STANDARD
  }
};

/*
 * Device "handles".
 */
UosSpiBus   spi1Bus;
UosSpiBus   spi4Bus;
UosFlashDev flashDev;
UosSpiDev oledDev;

void devTreeInit()
{
// Initialize SPI buses.
  uosSpiInit(&spi1Bus, &spi1BusConf.base);
  uosSpiInit(&spi4Bus, &spi4BusConf.base);

// oled display
  uosSpiDevInit(&oledDev, &oledConf, &spi4Bus);

// Add flash chip
  uosFlashInit(&flashDev, &flashConf, &spi1Bus);
}

void fsInit(void)
{
  uint32_t jedec;
  SPIFLASH_read_jedec_id(&flashDev.spif, &jedec);
  printf("spiflash ID 0x%" PRIx32 "\n", jedec);

  spiffs_config cfg;

  cfg.phys_size        = 2 * 1024 * 1024;
  cfg.phys_addr        = 0;
  cfg.phys_erase_block = 65536;
  cfg.log_block_size   = 65536;
  cfg.log_page_size    = 256;

  uosMountSpiffs("/flash", &flashDev, &cfg);
}


