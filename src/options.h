/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include "boot_sqi_xip.h"

typedef struct STRUCT_DEBUG_CONFIGURATION
{
	unsigned long ul_arp;
	unsigned long ul_boot_common;
	unsigned long ul_boot_drv_eth;
	unsigned long ul_boot_drv_spi;
	unsigned long ul_boot_drv_sqi;
	unsigned long ul_boot_parflash;
	unsigned long ul_boot_spi_mem;
	unsigned long ul_boot_sqi_xip;
	unsigned long ul_boot_tftp;
	unsigned long ul_bootblock_oldstyle;
	unsigned long ul_buckets;
	unsigned long ul_dhcp;
	unsigned long ul_dns;
	unsigned long ul_eth;
	unsigned long ul_icmp;
	unsigned long ul_ipv4;
	unsigned long ul_main_common;
	unsigned long ul_memory;
	unsigned long ul_options;
	unsigned long ul_tftp;
	unsigned long ul_udp;
} DEBUG_CONFIGURATION_T;


typedef struct STRUCT_ROMLOADER_OPTIONS
{
	/* SQI options */
	SQI_CONFIGURATION_T t_sqi_options;

#if CFG_DEBUGMSG==1
	/* TODO: Always enable this block once the debug messages are
	 * included in the normal build.
	 */
	/* debug config */
	DEBUG_CONFIGURATION_T t_debug_settings;
#endif
} ROMLOADER_OPTIONS_T;


extern const ROMLOADER_OPTIONS_T g_t_romloader_options;


#endif  /* __OPTIONS_H__ */

