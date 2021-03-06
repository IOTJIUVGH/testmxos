/**
******************************************************************************
* @file    mxos_config.h
* @author  William Xu
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

/************************************************************************
 * Main application stack size. */
#define MXOS_DEFAULT_APPLICATION_STACK_SIZE         (0x1000)

#define MXOS_WLAN_CONNECTION_ENABLE     1
#define MXOS_SYSTEM_DISCOVERY_ENABLE    0

/************************************************************************
 * Enable IPv6 in TCPIP stack. */
#define MXOS_CONFIG_IPV6                1

#endif
