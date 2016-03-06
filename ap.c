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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/dns.h"

#include "lwip/stats.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include <lwip/dhcp.h>
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "apps/dhcps/dhcps.h"

#include "wwd_management.h"
#include "wwd_wifi.h"
#include "wwd_network.h"

#include "eshell.h"
#include "eshell-commands.h"
#include "emw-meter.h"

static bool apActive = false;
static bool apIfAdded = false;

struct netif apIf;

int apUp()
{
  wiced_ssid_t ssid;

  strcpy((char*)ssid.value, "EMW3165");
  ssid.length = strlen((char*)ssid.value);

  printf("Starting Access Point.\n");

  if (!apIfAdded) {

    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&gw, 192,168,0,1);
    IP4_ADDR(&ipaddr, 192,168,0,1);
    IP4_ADDR(&netmask, 255,255,255,0);

    netifapi_netif_add(&apIf,
                       &ipaddr,
                       &netmask,
                       &gw,
                       (void*)WWD_AP_INTERFACE,
                       ethernetif_init,
                       tcpip_input);
    apIfAdded = true;
  }

#define AP_CHANNEL 1

  wwd_wifi_start_ap( &ssid, WICED_SECURITY_WPA2_AES_PSK, (uint8_t*) "xxx123!!!", 9, AP_CHANNEL );
  netifapi_netif_set_up(&apIf);

  dhcpServerStart(&apIf);

  apActive = true;
  return 0;
}

static void apDown()
{
  dhcpServerStop(&apIf);
  netifapi_netif_set_down(&apIf);
  wwd_wifi_leave(WWD_AP_INTERFACE);
  apActive = false;
}

/*
 * Activate access point in EMW3165, handy for initial configuration.
 */
static int ap(EshContext* ctx)
{
  char* stop = eshNamedArg(ctx, "stop", false);

  eshCheckNamedArgsUsed(ctx);
  eshCheckArgsUsed(ctx);
  if (eshArgError(ctx) != EshOK)
    return -1;

  if (stop) {

    if (apActive) {

      apDown();
      eshPrintf(ctx, "AP stopped.\n");
      apActive = false;
    }

    return 0;
  }

  if (apActive) {

    eshPrintf(ctx, "AP already running.\n");
    return -1;
  }

  if (apUp() == -1)
    eshPrintf(ctx, "Cannot start AP.\n");

  return 0;
}


const EshCommand apCommand = {
  .flags = 0,
  .name = "ap",
  .help = "--stop | start stop access point",
  .handler = ap
}; 

