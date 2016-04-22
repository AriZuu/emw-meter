/*
 * Copyright (c) 2015-2016, Ari Suutari <ari@stonepile.fi>.
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

#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

#include "network/wwd_network_constants.h"

#define SYS_LIGHTWEIGHT_PROT 1

#define MEM_LIBC_MALLOC                (1)

/*
 * Use 2-byte padding word in ethernet header, but make it
 * overlap Wiced lower layer headers (38 bytes). This gives
 * a 36-byte link encapsulation header size, which aligns
 * perfectly on 32-bit boundary (stm32 dma requires 32-bit aligment). 
 */

#define ETH_PAD_SIZE                    2
#define PBUF_LINK_ENCAPSULATION_HLEN    (WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX-ETH_PAD_SIZE)

/*
 * Ensure that buffers are big enough to hold largest packet that comes
 * from wifi chip.
 */
#define PBUF_POOL_BUFSIZE              (WICED_LINK_MTU)

#define LWIP_NETIF_TX_SINGLE_PBUF      (1)

#define TCPIP_THREAD_PRIO               7
#define TCPIP_THREAD_STACKSIZE          3000
#define LWIP_IPV6                       1
#define LWIP_RAW                        1 // for ping
#define LWIP_SO_RCVTIMEO                1
#define LWIP_NETIF_API                  1

#define IP_REASSEMBLY                   0
#define ARP_QUEUEING                    1
#define LWIP_DNS			1

#define LWIP_DEBUG                      1
#define MEM_ALIGNMENT                   4
#define LWIP_STATS	                    1
#define LWIP_STATS_DISPLAY	            0

#define LWIP_DHCP                       1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define ETHARP_SUPPORT_STATIC_ENTRIES 1

#define PBUF_POOL_SIZE                  16
#define MEMP_NUM_PBUF                   16 
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                5
#define MEMP_NUM_TCP_PCB_LISTEN         2
#define TCP_SND_QUEUELEN                6
#define MEMP_NUM_TCP_SEG                16

int sys_random(void);
#define LWIP_RAND() sys_random()
#define LWIP_COMPAT_SOCKETS		0

#define WDCFG_FIRMWARE "43362A2.bin"
#define WDCFG_FIRMWARE_PATH "/firmware", "/flash"

//#define TIMERS_DEBUG LWIP_DBG_ON
//#define TCPIP_DEBUG      LWIP_DBG_ON
//#define NETIF_DEBUG      LWIP_DBG_ON
//#define SOCKETS_DEBUG    LWIP_DBG_OFF
//#define IP_DEBUG         LWIP_DBG_ON
//#define ICMP_DEBUG       LWIP_DBG_ON
//#define DHCP_DEBUG LWIP_DBG_ON
//#define ETHARP_DEBUG LWIP_DBG_ON
//#define LWIP_DBG_TYPES_ON 0xff

#endif
