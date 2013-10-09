/***************************************************************************
 *   Copyright (C) 2005 - 2011 by Hilscher GmbH                            *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include "options.h"


const ROMLOADER_OPTIONS_T g_t_romloader_options =
{
	.t_sqi_options =
	{
		.tSpiCfg =
		{
			.ulInitialSpeedKhz = 50000,
			.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT|MSK_SQI_CFG_IDLE_IO2_OE|MSK_SQI_CFG_IDLE_IO2_OUT|MSK_SQI_CFG_IDLE_IO3_OE|MSK_SQI_CFG_IDLE_IO3_OUT,
			.ucMode = SPI_MODE3,
			.aucMmio =
			{
				0xffU,          /* chip select */
				0xffU,          /* clock */
				0xffU,          /* sio2 */
				0xffU           /* sio3 */
			}

		},
		.ulSqiRomCfg = 0x02 << SRT_NX56_sqi_sqirom_cfg_clk_div_val
		             |    0 << SRT_NX56_sqi_sqirom_cfg_t_csh
		             |    4 << SRT_NX56_sqi_sqirom_cfg_dummy_cycles
		             | 0xa5 << SRT_NX56_sqi_sqirom_cfg_cmd_byte
		             |    4 << SRT_NX56_sqi_sqirom_cfg_addr_bits
		             |    1 << SRT_NX56_sqi_sqirom_cfg_addr_nibbles
		             |    1 << SRT_NX56_sqi_sqirom_cfg_addr_before_cmd
		             |    1 << SRT_NX56_sqi_sqirom_cfg_enable,
		.tSeqId =
		{
			/* Length is 23 bytes. */
			23,
			/* Set netX to mode 0 (1 bit interface). */
			QSI_MODE|0x0,
			/* Send the reset command. This switches a device in 4 bit mode back to 1 bit. */
			QSI_SEND_SDD|0x1, 0xff, 0xff,
			/* Read the ID and compare it with the Macronix magic. */
			QSI_SEND_SNN|0x0, 0x9f, QSI_RECEIVE_SDD|0x1,
			QSI_CMP_FATAL|0x1, 0xc2, 0x20,
			/* Check if the quad enable bit is set.*/
			QSI_SEND_SNN|0x0, 0x05, QSI_RECEIVE_SDD|0x0,
			QSI_MASK|0x0, 0x40,
			QSI_CMP|0x0, 0x40,
			QSI_SKIP_EQ|0x4,
			/* Write enable. */
			QSI_SEND_SDD|0x0, 0x06,
			/* Write status register 2 */
			QSI_SEND_SDD|0x1, 0x01, 0x40
		},
		.tSeqRead =
		{
			/* length is 8 */
			8,
			QSI_MODE|0x0,                   /* send the command always in SPI mode */
			QSI_SEND_SNN|0x0, 0xeb,         /* send the command byte 0xeb */
			QSI_MODE|0x2,                   /* switch to QSI mode */
			QSI_ADR|0x7,                    /* send adr2,adr1,adr0 */
			QSI_SEND_SNN|0x0, 0xa5,         /* send modifier 0xa5 */
			QSI_DUMMY_NNN|0x1               /* send 2 dummy bytes */
		},
		.tSeqDeactivate =
		{
			/* length is 3 */
			3,
			QSI_SEND_SDD|0x1, 0xff, 0xff
		}
	},

#if CFG_DEBUGMSG==1
	/* TODO: Always enable this block once the debug messages are
	 * included in the normal build.
	 */
	.t_debug_settings =
	{
		.ul_boot_drv_sqi = 0xffffffffU,
		.ul_boot_sqi_xip = 0xffffffffU
	}
#endif
};

