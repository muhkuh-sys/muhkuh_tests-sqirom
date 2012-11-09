/***************************************************************************
 *   Copyright (C) 2005 - 2011 by Hilscher GmbH                            *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include "options.h"
#include "system.h"


#if ASIC_TYP==10
#       define USB_VENDOR_ID  0x1939U
#       define USB_DEVICE_ID  0x000cU
#       define USB_RELEASE_ID 0x0001U
#       define USB_DEVICE_STR {  6, 'n', 'e', 't', 'X', '1', '0',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }
#       define USB_SERIAL_STR {  1, '1',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }
#elif ASIC_TYP==56
#       define USB_VENDOR_ID  0x1939U
#       define USB_DEVICE_ID  0x0018U
#       define USB_RELEASE_ID 0x0001U
#       define USB_DEVICE_STR {  6, 'n', 'e', 't', 'X', '5', '6',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }
#       define USB_SERIAL_STR {  1, '1',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }
#endif


const ROMLOADER_OPTIONS_T t_default_options =
{
	.at_uart =
	{
		{
			/* NOTE: the RX and TX MMIO pins are set in the
			 * function "options_set_default".
			 */
			.uc_rx_mmio = 0xffU,
			.uc_tx_mmio = 0xffU,
			.uc_rts_mmio = 0xffU,
			.uc_cts_mmio = 0xffU,
			.uc_mode = UART_MODE_BYTESIZE_8BIT,
/* Lower the UART baud rate on all FPGA boards. They are too slow for 115200bps. */
#if ASIC_ENV==ASIC_ENV_FPGA_ETH || ASIC_ENV==ASIC_ENV_FPGA_USB
			.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_9600),
#else
			.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200),
#endif

			.uc_connect1 = '*',
			.uc_connect2 = '#'
		},
		{
			.uc_rx_mmio = 0xffU,
			.uc_tx_mmio = 0xffU,
			.uc_rts_mmio = 0xffU,
			.uc_cts_mmio = 0xffU,
			.uc_mode = 0U,
			.us_baud_div = 0U,
			.uc_connect1 = 0U,
			.uc_connect2 = 0U
		}
	},

	.t_usb =
	{
		.us_vendor_id = USB_VENDOR_ID,
		.us_device_id = USB_DEVICE_ID,
		.us_release_id = USB_RELEASE_ID,
		.uc_config_characteristics = 0xc0,
		.uc_maximum_power_consumption = 0x00,
		.t_vendor_string = {  8, 'H', 'i', 'l', 's', 'c', 'h', 'e', 'r',   0,   0,   0,   0,   0,   0,   0,   0 },
		.t_device_string = USB_DEVICE_STR,
		.t_serial_string = USB_SERIAL_STR
	},


	.atSpiMemCfg =
	{
		/* BOOTDEV_SQI_MEM_CS0 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 10000,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE,
				.ucMode = 0,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.ucMaxMagicSeek = 16
		},
		/* BOOTDEV_SQI_MEM_CS1 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 10000,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE,
				.ucMode = 0,
				.aucMmio =
				{
					0xffU,		/* Chip select. */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.ucMaxMagicSeek = 16
		},
		/* BOOTDEV_SQI_MEM_CS2 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 10000,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE,
				.ucMode = 0,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.ucMaxMagicSeek = 16
		},
		/* BOOTDEV_SPI_MEM_CS0 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 10000,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE,
				.ucMode = 0,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.ucMaxMagicSeek = 16
		},
		/* BOOTDEV_SPI_MEM_CS1 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 10000,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE,
				.ucMode = 0,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.ucMaxMagicSeek = 16
		},
		/* BOOTDEV_SPI_MEM_CS2 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 10000,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE,
				.ucMode = 0,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.ucMaxMagicSeek = 16
		}
	},
	.atSpiMmcCfg =
	{
		/* BOOTDEV_SQI_MMC_CS0 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 400,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT,
				.ucMode = 3,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.atFileName = {  8, 'N', 'E', 'T', 'X', '.', 'R', 'O', 'M',   0,   0,   0,   0 }
		},
		/* BOOTDEV_SQI_MMC_CS1 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 400,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT,
				.ucMode = 3,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.atFileName = {  8, 'N', 'E', 'T', 'X', '.', 'R', 'O', 'M',   0,   0,   0,   0 }
		},
		/* BOOTDEV_SQI_MMC_CS2 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 400,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT,
				.ucMode = 3,
				.aucMmio =
				{
					0xffU,		/* Chip select. NOTE: this is set to 9 for netX52. */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.atFileName = {  8, 'N', 'E', 'T', 'X', '.', 'R', 'O', 'M',   0,   0,   0,   0 }
		},
		/* BOOTDEV_SPI_MMC_CS0 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 400,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT,
				.ucMode = 3,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.atFileName = {  8, 'N', 'E', 'T', 'X', '.', 'R', 'O', 'M',   0,   0,   0,   0 }
		},
		/* BOOTDEV_SPI_MMC_CS1 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 400,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT,
				.ucMode = 3,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.atFileName = {  8, 'N', 'E', 'T', 'X', '.', 'R', 'O', 'M',   0,   0,   0,   0 }
		},
		/* BOOTDEV_SPI_MMC_CS2 */
		{
			.tSpiCfg =
			{
				.ulInitialSpeedKhz = 400,
				.ucIdleCfg = MSK_SQI_CFG_IDLE_IO1_OE|MSK_SQI_CFG_IDLE_IO1_OUT,
				.ucMode = 3,
				.aucMmio =
				{
					0xffU,		/* chip select */
					0xffU,		/* clock */
					0xffU,		/* MISO */
					0xffU		/* MOSI */
				}
			},
			.atFileName = {  8, 'N', 'E', 'T', 'X', '.', 'R', 'O', 'M',   0,   0,   0,   0 }
		}
	},


	.atSRam =
	{
		{
			{
				.ulCtrl = 0x0303033fU
			},
			{
				.ulCtrl = 0x0303033fU
			},
			{
				.ulCtrl = 0x0303033fU
			},
			{
				.ulCtrl = 0x0303033fU
			}
		},
		{
			{
				.ulCtrl = 0x0303033fU
			},
			{
				.ulCtrl = 0x0303033fU
			},
			{
				.ulCtrl = 0x0303033fU
			},
			{
				.ulCtrl = 0x0303033fU
			}
		}
	},


	.tSRamGeneral =
	{
		{
			.ulCs0_apm_ctrl = 0x000000ffU,
			.ulRdy_cfg = 0x00000001U
		},
		{
			.ulCs0_apm_ctrl = 0x000000ffU,
			.ulRdy_cfg = 0x00000001U
		}
	},

	.t_sdram =
	{
		{
			.ul_general_ctrl = 0,
			.ul_timing_ctrl = 0,
			.ul_mr = 0x00000033U,
			.us_sdram_setup_timeout_ms = 100
		},
		{
			.ul_general_ctrl = 0,
			.ul_timing_ctrl = 0,
			.ul_mr = 0x00000033U,
			.us_sdram_setup_timeout_ms = 100
		}
	},


	.t_bootdev_options =
	{
		.auc_boot_devices =
		{
			BOOTDEV_NONE,
			BOOTDEV_NONE,
			BOOTDEV_NONE,
			BOOTDEV_NONE,
			BOOTDEV_NONE,
			BOOTDEV_NONE,
			BOOTDEV_NONE,
			BOOTDEV_NONE
		}
	},

	.t_console_options =
	{
		/* general settings */
		.uc_console_line_size = 80U,
		.uc_console_linefeed_mode = CONSOLE_LINEFEED_CRLF,

		/* console devices */
		.auc_console_devices =
		{
			CONSOLE_DEVICE_UART0,
#if ASIC_ENV==ASIC_ENV_FPGA_ETH
			/* The ethernet FPGA crashes when USB is used. */
			CONSOLE_DEVICE_NONE,
#else
			CONSOLE_DEVICE_USB,
#endif
			CONSOLE_DEVICE_NONE,
			CONSOLE_DEVICE_NONE
		},

		/* UART diagnostic settings */
		.uc_uart_diagnostics_device = CONSOLE_DEVICE_NONE
	},


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
		.ulSqiRomCfg = 0x0234a547U,
		.tSeqId =
		{
			/* Length is 27 bytes. */
			27,
			/* Set netX to mode 0 (1 bit interface). */
			QSI_MODE|0x0,
			/* Send the reset command. This switches a device in 4 bit mode back to 1 bit. */
			QSI_SEND_SDD|0x1, 0xff, 0xff,
			/* Read the ID and compare it with the Winbond magic. */
			QSI_SEND_SNN|0x0, 0x9f, QSI_RECEIVE_SDD|0x1,
			QSI_MASK|0x1, 0xff, 0xef,
			QSI_CMP_FATAL|0x1, 0xef, 0x40,
			/* Check if the quad enable bit is set.*/
			QSI_SEND_SNN|0x0, 0x35, QSI_RECEIVE_SDD|0x0,
			QSI_MASK|0x0, 0x02,
			QSI_CMP|0x0, 0x02,
			QSI_SKIP_EQ|0x5,
			/* Write enable. */
			QSI_SEND_SDD|0x0, 0x06,
			/* Write status register 2 */
			QSI_SEND_SDD|0x2, 0x01, 0x02, 0x02
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
			/* length is 2 */
			2,
			QSI_SEND_SDD|0x0, 0xab
		}
	},


	.t_hif_options =
	{
/* FIXME: imlement this for the other asics */
#if ASIC_TYP==10
		.ulHifIoCfg = DFLT_VAL_NX10_hif_io_cfg,
		.ulMemCfg = 0,
		.ucDpmCfg0x0	= DFLT_VAL_NX10_dpm_cfg0x0,
		.ucDpmAddrCfg	= DFLT_VAL_NX10_dpm_addr_cfg,
		.ucDpmTimingCfg	= DFLT_VAL_NX10_dpm_timing_cfg,
		.ucDpmRdyCfg	= DFLT_VAL_NX10_dpm_rdy_cfg,
		.ucDpmMiscCfg	= DFLT_VAL_NX10_dpm_misc_cfg,
		.ucDpmIoCfgMisc	= DFLT_VAL_NX10_dpm_io_cfg_misc,
		.ucDpmIrqFiqCfg	= 0
#elif ASIC_TYP==56
		/* NOTE: Do not set a default value here. It is set by the strapping options from atPresetMemoryCfg. */
		.ulHifIoCfg     = DFLT_VAL_NX56_hif_io_cfg,
		.ulMemCfg       = 0,
		.ucDpmCfg0x0    = DFLT_VAL_NX56_dpm_cfg0x0,
		.ucDpmAddrCfg   = DFLT_VAL_NX56_dpm_addr_cfg,
		.ucDpmTimingCfg = DFLT_VAL_NX56_dpm_timing_cfg,
		.ucDpmRdyCfg    = DFLT_VAL_NX56_dpm_rdy_cfg,
		.ucDpmMiscCfg   = DFLT_VAL_NX56_dpm_misc_cfg,
		.ucDpmIoCfgMisc = DFLT_VAL_NX56_dpm_io_cfg_misc,
		.ucDpmIrqFiqCfg = 0
#endif
	},

	.t_ethernet =
	{
		.auc_tftp_server_name = { 0U },
		.auc_tftp_bootfile_name = { 0U },
		.aucMac = { 0x00U, 0x02U, 0xa2U, 0x20U, 0x20U, 0x00U },
		.usTftpPort = 69U,
		.ulIp = 0U,
		.ulGatewayIp = 0U,
		.ulNetmask = 0U,
		.ulTftpIp = 0U,
		.ulDnsIp = 0U,
		.usLinkUpDelay = 1000,
		/* NOTE: Only allow 10M links for FPGA.
		 * The ethernet FPGA runs with a clock of 10MHz, but the PHY
		 * is still operating at normal speed of 25MHz. This means the
		 * PHY is way too fast for the FPGA. Data from a 100M link can
		 * not be sampled. Fortunately 10M links still work.
		 */
#if ASIC_ENV==ASIC_ENV_FPGA_ETH
		.usPhyControl = MSK_PHY_CTRL_FULL_DUPLEX | MSK_PHY_CTRL_AUTO_NEG_ENABLE,
		.usPhyAutonegAdvertisement = 0x01 | MSK_PHY_ANADV_10_BASE_T | MSK_PHY_ANADV_10_BASE_T_FDX,
#else
		.usPhyControl = DFLT_PHY_CTRL,
		.usPhyAutonegAdvertisement = DFLT_PHY_ANADV,
#endif
		.usArpTimeout = 1000,
		.usDhcpTimeout = 3000,
		.usDnsTimeout = 3000,
		.usTftpTimeout = 3000,
		.ucXcPort = 0U,
#if ASIC_ENV==ASIC_ENV_FPGA_ETH
		/* The FPGA uses an external phy. It has a fixed address which is 1. */
		.ucPhyAdr = 1U,
#else
		/* The internal phy has address 0. */
		.ucPhyAdr = 0U,
#endif
		.ucArpRetries = 10,
		.ucDhcpRetries = 5,
		.ucDnsRetries = 5,
		.ucTftpRetries = 5,
		.aucMmioCfg =
		{
			0xff,	/* eth link */
			0xff	/* eth act */
		},
#if ASIC_ENV==ASIC_ENV_INTRAM
		/* In INTRAM environment, an external PHY is connected to the
		 * pins. Set the XPIC to MAC mode.
		 */
		.ulIoConfig2 = 24U << HOSTSRT(io_config2_sel_eth_mii)
#else
		/* All other environments expect a plain PHY connection. */
		.ulIoConfig2 = 20U << HOSTSRT(io_config2_sel_eth_mii)
#endif
	},

#if CFG_DEBUGMSG==1
	/* TODO: Always enable this block once the debug messages are
	 * included in the normal build.
	 */
	//.t_debug_settings = { 0 },
#endif

	.t_system_config =
	{
		.ul_io_config = HOSTMSK(io_config_usb2jtag_en),
		.ul_io_config2 = 0,
		.ul_clock_enable = 0,
		.tChipSubTyp = CHIP_SUBTYP_NETX50,

		.auc_mmio_sel = { 0 },
		.auc_mmio_inv = { 0 }
	}
};

