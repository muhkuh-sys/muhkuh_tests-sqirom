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

#include "boot_drv_eth/boot_drv_eth.h"
#include "boot_common.h"
#include "boot_spi_mem.h"
#include "boot_spi_mmc.h"
#include "console_io.h"
#include "boot_sqi_xip.h"
#include "memory.h"
#include "uart.h"
#include "usb.h"


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


#define NUMBER_OF_MMIOS ARRAYSIZE(((NX56_MMIO_CTRL_AREA_T*)NULL)->aulMmio_cfg)

typedef enum ENUM_CHIP_SUBTYP
{
	CHIP_SUBTYP_NETX50              = 0,
	CHIP_SUBTYP_NETX51              = 1,
	CHIP_SUBTYP_NETX52              = 2
} CHIP_SUBTYP_T;


typedef struct STRUCT_SYSTEM_CONFIG
{
	unsigned long  ul_io_config;
	unsigned long  ul_io_config2;
	unsigned long  ul_clock_enable;
	CHIP_SUBTYP_T  tChipSubTyp;
	unsigned char auc_mmio_sel[NUMBER_OF_MMIOS];
	unsigned char auc_mmio_inv[NUMBER_OF_MMIOS];
} SYSTEM_CONFIG_T;


typedef struct STRUCT_ROMLOADER_OPTIONS
{
	/* console device settings */
	UART_CONFIGURATION_T at_uart[2];
	USB_CONFIGURATION_T t_usb;

	/* media settings */
	BOOT_SPI_MEM_CONFIGURATION_T atSpiMemCfg[6];
	BOOT_SPI_MMC_CONFIGURATION_T atSpiMmcCfg[6];


	/* target settings */
	SRAM_CONFIGURATION_T atSRam[MEMORY_UNIT_Max][4];
	SRAM_GENERAL_CONFIGURATION_T tSRamGeneral[MEMORY_UNIT_Max];
	SDRAM_CONFIGURATION_T t_sdram[MEMORY_UNIT_Max];


	/* boot settings */
	BOOTDEV_CONFIGURATION_T t_bootdev_options;

	/* console options */
	CONSOLE_IO_OPTIONS_T t_console_options;

	/* SQI options */
	SQI_CONFIGURATION_T t_sqi_options;

	/* DPM options */
	HIF_CONFIGURATION_T t_hif_options;

	/* ethernet settings */
	ETHERNET_CONFIGURATION_T t_ethernet;

#if CFG_DEBUGMSG==1
	/* TODO: Always enable this block once the debug messages are
	 * included in the normal build.
	 */
	/* debug config */
	DEBUG_CONFIGURATION_T t_debug_settings;
#endif

	/* system config */
	SYSTEM_CONFIG_T t_system_config;
} ROMLOADER_OPTIONS_T;


extern ROMLOADER_OPTIONS_T g_t_romloader_options;


#endif	/* __OPTIONS_H__ */

