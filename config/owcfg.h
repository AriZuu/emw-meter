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

/*
 * Onewire gpio settings.
 */

#define OW_PIN 10
#define OW_BIT (1 << OW_PIN)
#define OW_PORT GPIOB

#define _OW_SHIFT(v) (v << (OW_PIN * 2))
#define _OW_MODE(m) OW_PORT->MODER = ((OW_PORT->MODER & ~_OW_SHIFT(GPIO_MODER_MODER0)) \
                                   | _OW_SHIFT(m))

/*
 * Read GPIO input.
 */
#define OWCFG_READ_IN()  ((OW_PORT->IDR & OW_BIT) ? 1 : 0)

/*
 * GPIO to output 1.
 */
#define OWCFG_OUT_LOW()  do { \
                              OW_PORT->BSRRH = OW_BIT; \
                              _OW_MODE(GPIO_Mode_OUT); \
                         } while (0)

/*
 * GPIO to output 0.
 */
#define OWCFG_OUT_HIGH() do { \
                              OW_PORT->BSRRL = OW_BIT; \
                              _OW_MODE(GPIO_Mode_OUT); \
                         } while (0)
/*
 * GPIO to input.
 */
#define OWCFG_DIR_IN()   do {  \
                              _OW_MODE(GPIO_Mode_IN); \
                         } while(0)

