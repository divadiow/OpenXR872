/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if PRJCONF_NET_EN

#include <string.h>
#include <stdlib.h>
#include "kernel/os/os.h"
#include "net/wlan/wlan.h"
#include "net/wlan/wlan_defs.h"
#include "lwip/tcpip.h"
#include "net/udhcp/usr_dhcpd.h"

#include "common/framework/sysinfo.h"
#include "common/board/board.h"
#include "net_ctrl.h"
#include "net_ctrl_debug.h"

#include "driver/chip/hal_prcm.h"
#include "pm/pm.h"

struct netif *g_wlan_netif = NULL;

#ifdef STA_SOFTAP_COEXIST
struct netif *g_wlan_ap_netif = NULL;
#endif


static void netif_tcpip_init_done(void *arg)
{
//	NET_DBG("%s()\n", __func__);
}

#ifdef STA_SOFTAP_COEXIST
int net_sys_init(void)
{
	tcpip_init(netif_tcpip_init_done, NULL); /* Init lwip module */
	struct sysinfo *sysinfo = sysinfo_get();
	if (sysinfo == NULL) {
		NET_ERR("failed to get sysinfo %p\n", sysinfo);
		return -1;
	}

	if (wlan_sys_init() != 0) {
		NET_ERR("net system start failed\n");
		return -1;
	}

#ifndef __CONFIG_ETF_CLI
	wlan_set_mac_addr(NULL, sysinfo->mac_addr, SYSINFO_MAC_ADDR_LEN);
	wlan_attach(net_ctrl_msg_send);
#endif /* __CONFIG_ETF_CLI */
	return 0;
}

void net_sys_deinit(void)
{
	wlan_detach();
#if LWIP_XR_DEINIT
	tcpip_deinit(); /* DeInit lwip module */
#endif
	while (HAL_PRCM_IsSys3Alive()) {
		OS_MSleep(10); /* wait net */
	}

	if (wlan_sys_deinit()) {
		NET_ERR("net system stop failed\n");
	}
}

int net_sys_start(enum wlan_mode mode)
{
#ifndef __CONFIG_ETF_CLI
	if (mode == WLAN_MODE_HOSTAP) {
		if (g_wlan_ap_netif) {
			NET_ERR("Soft AP is already start!\n");
			return 0;
		}
		g_wlan_ap_netif = net_open(mode);
	} else {
		if (g_wlan_netif) {
			NET_ERR("STA is already start!\n");
			return 0;
		}
		g_wlan_netif = net_open(mode);
	}
#endif /* __CONFIG_ETF_CLI */
	return 0;
}

int net_sys_stop(enum wlan_mode mode)
{
	if (mode == WLAN_MODE_HOSTAP) {
		if (g_wlan_ap_netif) {
			dhcp_server_stop();
			net_close(g_wlan_ap_netif);
			g_wlan_ap_netif = NULL;
		}
	} else {
		net_close(g_wlan_netif);
		g_wlan_netif = NULL;
	}

	return 0;
}

int net_sys_onoff(unsigned int enable)
{
	struct sysinfo *sysinfo = sysinfo_get();
	if (sysinfo == NULL) {
		NET_ERR("failed to get sysinfo %p\n", sysinfo);
		return -1;
	}

	printf("%s set net to power%s\n", __func__, enable?"on":"off");

	if (enable) {
		net_sys_start(sysinfo->wlan_mode);
#ifdef CONFIG_AUTO_RECONNECT_AP
		net_ctrl_connect_ap(NULL);
#endif
	} else {
#ifdef CONFIG_AUTO_RECONNECT_AP
		net_ctrl_disconnect_ap(NULL, 1);
#endif
		net_sys_stop(WLAN_MODE_HOSTAP);
		net_sys_stop(WLAN_MODE_STA);
	}

	return 0;
}
#else
void net_sys_init(void)
{
	tcpip_init(netif_tcpip_init_done, NULL); /* Init lwip module */
}

#if LWIP_XR_DEINIT
void net_sys_deinit(void)
{
	tcpip_deinit(); /* DeInit lwip module */
}
#endif

static int _net_sys_start(enum wlan_mode mode)
{
	struct sysinfo *sysinfo = sysinfo_get();
	if (sysinfo == NULL) {
		NET_ERR("failed to get sysinfo %p\n", sysinfo);
		return -1;
	}

	if (wlan_sys_init() != 0) {
		NET_ERR("net system start failed\n");
		return -1;
	}

#ifndef __CONFIG_ETF_CLI
	wlan_set_mac_addr(NULL, sysinfo->mac_addr, SYSINFO_MAC_ADDR_LEN);
	wlan_attach(net_ctrl_msg_send);
	g_wlan_netif = net_open(mode);
#endif /* __CONFIG_ETF_CLI */
	return 0;
}

static int _net_sys_stop(void)
{
	if (g_wlan_netif && wlan_if_get_mode(g_wlan_netif) == WLAN_MODE_HOSTAP) {
		dhcp_server_stop();
	}

	net_close(g_wlan_netif);
	g_wlan_netif = NULL;
	wlan_detach();

	while (HAL_PRCM_IsSys3Alive()) {
		OS_MSleep(10); /* wait net */
	}

	if (wlan_sys_deinit()) {
		NET_ERR("net system stop failed\n");
		return -1;
	}

	return 0;
}

int net_sys_start(enum wlan_mode mode)
{
	_net_sys_start(mode);

#if PRJCONF_NET_PM_EN
	pm_register_wlan_power_onoff(net_sys_onoff, PRJCONF_NET_PM_MODE);
#endif

	return 0;
}

int net_sys_stop(void)
{
#if PRJCONF_NET_PM_EN
	pm_unregister_wlan_power_onoff();
#endif

	_net_sys_stop();

	return 0;
}

int net_sys_onoff(unsigned int enable)
{
	struct sysinfo *sysinfo = sysinfo_get();
	if (sysinfo == NULL) {
		NET_ERR("failed to get sysinfo %p\n", sysinfo);
		return -1;
	}

	printf("%s set net to power%s\n", __func__, enable?"on":"off");

	if (enable) {
		_net_sys_start(sysinfo->wlan_mode);
#ifdef CONFIG_AUTO_RECONNECT_AP
		net_ctrl_connect_ap(NULL);
#endif
	} else {
#ifdef CONFIG_AUTO_RECONNECT_AP
		net_ctrl_disconnect_ap(NULL, 1);
#endif
		_net_sys_stop();
	}

	return 0;
}
#endif

#endif /* PRJCONF_NET_EN */
