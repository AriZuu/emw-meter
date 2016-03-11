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

static bool alreadyJoined = false;

void initConfig()
{
  uosConfigInit();
  uosConfigLoad("/flash/test.cfg");
}

/*
 * Write config entries to file in spiffs.
 */
static int wr(EshContext* ctx)
{

  eshCheckNamedArgsUsed(ctx);

  eshCheckArgsUsed(ctx);
  if (eshArgError(ctx) != EshOK)
    return -1;

  uosConfigSave("/flash/test.cfg");
  return 0;
}

/*
 * Clear all config entries by removing saved file.
 */
static int clear(EshContext* ctx)
{
  eshCheckNamedArgsUsed(ctx);
  eshCheckArgsUsed(ctx);
  if (eshArgError(ctx) != EshOK)
    return -1;

  uosFileUnlink("/flash/test.cfg");
  return 0;
}

void ifStatusCallback(struct netif *netif);

/*
 * This is called by lwip when interface status changes.
 */
void ifStatusCallback(struct netif *netif)
{
  if (netif_is_up(netif)) {

    potatoStart();
  }
}

struct netif defaultIf;

static int staUp()
{
  wiced_ssid_t ssid;
  /*
   * Get AP network name and password and attempt to join.
   */

  const char* ap = uosConfigGet("ap");
  const char* pass = uosConfigGet("pass");

  strcpy((char*)ssid.value, ap);
  ssid.length = strlen(ap);

  if (wwd_wifi_join(&ssid, WICED_SECURITY_WPA2_MIXED_PSK, (uint8_t*)pass, strlen(pass), NULL) != WWD_SUCCESS)
    return -1;

  printf("Join OK.\n");

  netifapi_netif_set_up(&defaultIf);
  netif_set_status_callback(&defaultIf, ifStatusCallback);
  netifapi_dhcp_start(&defaultIf);

#if LWIP_IPV6
  netif_create_ip6_linklocal_address(&defaultIf, 1);
  netif_ip6_addr_set_state(&defaultIf, 0, IP6_ADDR_TENTATIVE);
  defaultIf.ip6_autoconfig_enabled = 1;
#endif

  alreadyJoined = true;
  wifiLed(true);

  return 0;
}

static void staDown()
{
  wifiLed(false);

  netifapi_dhcp_stop(&defaultIf);
  netifapi_netif_set_down(&defaultIf);
  wwd_wifi_leave(WWD_STA_INTERFACE);
}

/*
 * Connect to existing access point.
 */
static int sta(EshContext* ctx)
{
  char* reset = eshNamedArg(ctx, "reset", false);
  char* ap;
  char* pass;

  eshCheckNamedArgsUsed(ctx);

  if (reset == NULL) {

    ap = eshNextArg(ctx, true);
    pass = eshNextArg(ctx, true);
  }

  eshCheckArgsUsed(ctx);
  if (eshArgError(ctx) != EshOK)
    return -1;

  if (reset) {

    if (alreadyJoined) {

      staDown();
      eshPrintf(ctx, "Left ap.\n");
      alreadyJoined = false;
    }

    uosConfigSet("ap", "");
    uosConfigSet("pass", "");
    return 0;
  }

  if (ap == NULL || pass == NULL) {

    eshPrintf(ctx, "Usage: sta --reset | ap pass\n");
    return -1;
  }

  if (alreadyJoined) {

    eshPrintf(ctx, "Already joined Wifi network.\n");
    return -1;
  }

  uosConfigSet("ap", ap);
  uosConfigSet("pass", pass);

  eshPrintf(ctx, "Joining %s with password %s\n", ap, pass);
  if (staUp() == -1)
    eshPrintf(ctx, "Join failed.\n");

  return 0;
}

static void joinThread(void* arg)
{
  const char* ap;
  const char* pass;

  do {
    
    ap = uosConfigGet("ap");
    pass = uosConfigGet("pass");

    posTaskSleep(MS(5000));

  } while (ap &&
           pass &&
           !alreadyJoined &&
           staUp() == -1);

  printf("Background join exiting.\n");
}

void checkAP()
{
  const char* ap = uosConfigGet("ap");
  const char* pass = uosConfigGet("pass");

  if (ap[0] != '\0' && pass[0] != '\0') {

    printf("Joining %s.\n", ap);
    if (staUp() == -1) {

       printf("Join failed, retrying join in background.\n");
       apUp();
       posTaskCreate(joinThread, NULL, 5, 1024);
    }
  }
  else
    apUp();
}

#if BUNDLE_FIRMWARE

/*
 * Copy Wifi firmware to spiffs. This allows compilation of
 * version without firmware blob in code flash, shrinking it's
 * size about 200 kb.
 */
static int copyfw(EshContext* ctx)
{
  eshCheckNamedArgsUsed(ctx);
  eshCheckArgsUsed(ctx);
  if (eshArgError(ctx) != EshOK)
    return -1;

  int from;
  int to;

  from = open("/firmware/" WDCFG_FIRMWARE, O_RDONLY);
  if (from == -1) {

    eshPrintf(ctx, "Cannot open /firmware/" WDCFG_FIRMWARE ".\n");
    return -1;
  }

  to = open("/flash/" WDCFG_FIRMWARE, O_WRONLY | O_CREAT | O_TRUNC);
  if (to == -1) {

    eshPrintf(ctx, "Cannot open /flash/" WDCFG_FIRMWARE ".\n");
    close(from);
    return -1;
  }

  int len;
  char buf[128];

  while ((len = read(from, buf, sizeof(buf))) > 0) {

    if (write(to, buf, len) != len) {

      eshPrintf(ctx, "Cannot write to /flash/" WDCFG_FIRMWARE ".\n");
      break;
    }
  }

  close(from);
  close(to);

  return 0;
}

const EshCommand copyfwCommand = {
  .flags = 0,
  .name = "copyfw",
  .help = "Copy wifi firmware from /firmware to /flash",
  .handler = copyfw
};

#endif

const EshCommand staCommand = {
  .flags = 0,
  .name = "sta",
  .help = "--reset | ap pass\ndisassociate/associate with given wifi access point.",
  .handler = sta
}; 

const EshCommand wrCommand = {
  .flags = 0,
  .name = "wr",
  .help = "save settings to flash",
  .handler = wr
}; 

const EshCommand clearCommand = {
  .flags = 0,
  .name = "clear",
  .help = "clear saved settings from flash",
  .handler = clear
}; 

extern const EshCommand mqttCommand;
extern const EshCommand apCommand;

const EshCommand *eshCommandList[] = {

#if BUNDLE_FIRMWARE
  &copyfwCommand,
#endif
  &mqttCommand,
  &staCommand,
  &apCommand,
  &wrCommand,
  &clearCommand,
  &eshTsCommand,
  &eshOnewireCommand,
  &eshPingCommand,
  &eshIfconfigCommand,
  &eshHelpCommand,
  &eshExitCommand,
  NULL
};

