/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/


#include <string.h>

#include "boot_drv_sqi.h"

#include "console_io.h"
#include "netx_io_areas.h"
#include "options.h"
#include "tools.h"


#if CFG_DEBUGMSG==1
	#define DEBUGZONE(n)  (g_t_romloader_options.t_debug_settings.ul_boot_drv_sqi&(0x00000001<<(n)))

	/*
	 * These defines must match the ZONE_* defines
	 */
	#define DBG_ZONE_ERROR		0
	#define DBG_ZONE_WARNING	1
	#define DBG_ZONE_FUNCTION	2
	#define DBG_ZONE_INIT		3
	#define DBG_ZONE_VERBOSE	7

	#define ZONE_ERROR		DEBUGZONE(DBG_ZONE_ERROR)
	#define ZONE_WARNING		DEBUGZONE(DBG_ZONE_WARNING)
	#define ZONE_FUNCTION		DEBUGZONE(DBG_ZONE_FUNCTION)
	#define ZONE_INIT		DEBUGZONE(DBG_ZONE_INIT)
	#define ZONE_VERBOSE		DEBUGZONE(DBG_ZONE_VERBOSE)

	#define DEBUGMSG(cond,...) ((void)((cond)?(uprintf(IDH_DEBUG,__VA_ARGS__)),1:0))
#else
	#define DEBUGMSG(cond,...) ((void)0)
#endif


static const MMIO_CFG_T aatMmioValues[3][4] =
{
#if ASIC_TYP==10
	/* Chip select 0 */
	{
		0xffU,                  /* chip select */
		0xffU,                  /* clock */
		0xffU,                  /* MISO */
		0xffU                   /* MOSI */
	},

	/* Chip select 1 */
	{
		MMIO_CFG_spi0_cs1n,     /* chip select */
		0xffU,                  /* clock */
		0xffU,                  /* MISO */
		0xffU                   /* MOSI */
	},

	/* Chip select 2 */
	{
		MMIO_CFG_spi0_cs2n,     /* chip select */
		0xffU,                  /* clock */
		0xffU,			/* MISO */
		0xffU                   /* MOSI */
	}
#elif ASIC_TYP==56
	/* Chip select 0 */
	{
		0xffU,                  /* chip select */
		0xffU,                  /* clock */
		MMIO_CFG_spi0_sio2_mmio,/* SIO2 */
		MMIO_CFG_spi0_sio3_mmio /* SIO3 */
	},

	/* Chip select 1 */
	{
		0xffU,                  /* chip select */
		0xffU,                  /* clock */
		MMIO_CFG_spi0_sio2_mmio,/* SIO2 */
		MMIO_CFG_spi0_sio3_mmio /* SIO3 */
	},

	/* Chip select 2 */
	{
		MMIO_CFG_spi0_cs2n,     /* chip select */
		0xffU,                  /* clock */
		MMIO_CFG_spi0_sio2_mmio,/* SIO2 */
		MMIO_CFG_spi0_sio3_mmio /* SIO3 */
	}
#endif
};


//-------------------------------------

/* BootSPI_ExchangeByte
 *
 * Description:
 *   Send and receive one byte on the spi bus.
 *
 * Parameters:
 *
 *   unsigned char out_byte = byte to send
 *
 * Return:
 *   unsigned char          = received byte
 *
 * Notes:
 *   This function has no timeout. If the transfer hangs for some reason, it will never return.
 */
static unsigned char qsi_spi_exchange_byte(const SPI_CFG_T *ptCfg, unsigned char uiByte)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;
	unsigned char ucByte;


	/* Set mode to "full duplex". */
	ulValue  = ptCfg->ulTrcBase;
	ulValue |= 3 << HOSTSRT(sqi_tcr_duplex);
	/* Start the transfer. */
	ulValue |= HOSTMSK(sqi_tcr_start_transfer);
	ptSqiArea->ulSqi_tcr = ulValue;

	/* Send byte. */
	ptSqiArea->ulSqi_dr = uiByte;

	/* Wait for one byte in the FIFO. */
	do
	{
		ulValue  = ptSqiArea->ulSqi_sr;
		ulValue &= HOSTMSK(sqi_sr_busy);
	} while( ulValue!=0 );

	/* Grab byte. */
	ucByte = (unsigned char)(ptSqiArea->ulSqi_dr);
	return ucByte;
}

//---------------------------------------------------------------------------


static unsigned long qsi_get_device_speed_representation(unsigned int uiSpeed)
{
	unsigned long ulDevSpeed;
	unsigned long ulInputFilter;


	/* ulSpeed is in kHz. */

	/* Limit speed to upper border. */
	if( uiSpeed>50000 )
	{
		uiSpeed = 50000;
	}

	/* Convert speed to "multiply add value". */
	ulDevSpeed  = uiSpeed * 4096;

	/* NOTE: do not round up here. */
	ulDevSpeed /= 100000;

	/* Use input filtering? */
	ulInputFilter = 0;
	if( ulDevSpeed<=0x0200 )
	{
		ulInputFilter = HOSTMSK(sqi_cr0_filter_in);
	}

	/* Shift to register position. */
	ulDevSpeed <<= HOSTSRT(sqi_cr0_sck_muladd);

	/* Add filter bit. */
	ulDevSpeed |= ulInputFilter;

	return ulDevSpeed;
}


static int qsi_slave_select(const SPI_CFG_T *ptCfg, int fIsSelected)
{
	HOSTDEF(ptSqiArea)
	int iResult;
	unsigned long uiChipSelect;
	unsigned long ulValue;


	iResult = 0;

	/* Get the chip select value. */
	uiChipSelect  = 0;
	if( fIsSelected!=0 )
	{
		uiChipSelect  = ptCfg->uiChipSelect << HOSTSRT(sqi_cr1_fss);
		uiChipSelect &= HOSTMSK(sqi_cr1_fss);
	}

	/* Get control register contents. */
	ulValue  = ptSqiArea->aulSqi_cr[1];

	/* Compare the active selection with the requested one. */
	if( (ulValue&HOSTMSK(sqi_cr1_fss))==uiChipSelect )
	{
		/* The slave is already selected. */
		/* NOTE: of course it does not hurt to select it again, but
		 * the chip select is also used to reset the buffer counter.
		 */
		iResult = -1;
	}
	else
	{
		/* Mask out the slave select bits. */
		ulValue &= ~HOSTMSK(sqi_cr1_fss);

		/* Mask in the new slave ID. */
		ulValue |= uiChipSelect;

		/* Write back new value. */
		ptSqiArea->aulSqi_cr[1] = ulValue;
	}

	return iResult;
}


static void qsi_send_idle(const SPI_CFG_T *ptCfg, unsigned int uiIdleChars)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;
	unsigned int uiShift;


	/* 0 means 1 cycle. */
	++uiIdleChars;

	/* Get the current mode. */
	ulValue   = ptCfg->ulTrcBase;
	ulValue  &= HOSTMSK(sqi_tcr_mode);
	ulValue >>= HOSTSRT(sqi_tcr_mode);
	uiShift = 3U - ulValue;

	/* Convert the number of idle bytes to cycles. */
	uiIdleChars <<= uiShift;
	--uiIdleChars;

	/* Set mode to "send dummy". */
	ulValue  = ptCfg->ulTrcBase;
	ulValue |= 0 << HOSTSRT(sqi_tcr_duplex);
	/* Set the transfer size. */
	ulValue |= uiIdleChars << HOSTSRT(sqi_tcr_transfer_size);
	/* Start the transfer. */
	ulValue |= HOSTMSK(sqi_tcr_start_transfer);
	ptSqiArea->ulSqi_tcr = ulValue;

	/* Wait until the transfer is done. */
	do
	{
		ulValue  = ptSqiArea->ulSqi_sr;
		ulValue &= HOSTMSK(sqi_sr_busy);
	} while( ulValue!=0 );
}


static unsigned long qsi_read_simple_chksum(const SPI_CFG_T *ptCfg, unsigned long *pulDst, size_t sizDword)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;
	int iCnt;
	unsigned long ulData;
	volatile unsigned long *pulCnt;
	volatile unsigned long *pulEnd;
	unsigned long ulChecksum;


	ulChecksum = 0;

	/* Set mode to "receive". */
	ulValue  = ptCfg->ulTrcBase;
	ulValue |= 1 << HOSTSRT(sqi_tcr_duplex);
	/* Set the transfer size. */
	ulValue |= (sizDword*sizeof(unsigned long) - 1) << HOSTSRT(sqi_tcr_transfer_size);
	/* Start the transfer. */
	ulValue |= HOSTMSK(sqi_tcr_start_transfer);
	ptSqiArea->ulSqi_tcr = ulValue;

	/* Get start and end address. */
	pulCnt = pulDst;
	pulEnd = pulDst + sizDword;

	/* Check the mode. */
	if( (ptCfg->ulTrcBase&HOSTMSK(sqi_tcr_mode))==0 )
	{
		ulData = 0;

		/* Mode 0 : the FIFO size is 8 bit. */
		while( pulCnt<pulEnd )
		{
			iCnt = 3;
			do
			{
				/* Wait for one byte in the FIFO. */
				do
				{
					ulValue  = ptSqiArea->ulSqi_sr;
					ulValue &= HOSTMSK(sqi_sr_rx_fifo_not_empty);
				} while( ulValue==0 );
				/* Grab a byte. */
				ulData >>= 8U;
				ulData  |= ptSqiArea->ulSqi_dr << 24U;
			} while( --iCnt>=0 );
			*pulCnt = ulData;
			ulChecksum += *(pulCnt++);
		}
	}
	else
	{
		/* DSI/QSI mode : the FIFO size is 32 bit. */
		while( pulCnt<pulEnd )
		{
			/* Wait for one DWORD in the FIFO. */
			do
			{
				ulValue  = ptSqiArea->ulSqi_sr;
				ulValue &= HOSTMSK(sqi_sr_rx_fifo_not_empty);
			} while( ulValue==0 );

			/* Get the DWORD. */
			*pulCnt = ptSqiArea->ulSqi_dr;
			ulChecksum += *(pulCnt++);
		}
	}

	return ulChecksum;
}


static int qsi_set_new_speed(unsigned long ulDeviceSpecificSpeed)
{
	HOSTDEF(ptSqiArea)
	int iResult;
	unsigned long ulValue;


	/* Expect error. */
	iResult = 1;

	/* All irrelevant bits must be 0. */
	if( (ulDeviceSpecificSpeed&(~(HOSTMSK(sqi_cr0_sck_muladd)|HOSTMSK(sqi_cr0_filter_in))))!=0 )
	{
		DEBUGMSG(ZONE_VERBOSE, _U(". new device specific speed has bits set in should-be-zero fields.\n"));
	}
	else if( ulDeviceSpecificSpeed==0 )
	{
		DEBUGMSG(ZONE_VERBOSE, _U(". new device specific speed is 0. This is invalid for this unit!\n"));
	}
	else
	{
		ulValue  = ptSqiArea->aulSqi_cr[0];
		ulValue &= ~(HOSTMSK(sqi_cr0_sck_muladd)|HOSTMSK(sqi_cr0_filter_in));
		ulValue |= ulDeviceSpecificSpeed;
		ptSqiArea->aulSqi_cr[0] = ulValue;

		/* All OK! */
		iResult = 0;
	}

	return iResult;
}


static void qsi_deactivate(const SPI_CFG_T *ptCfg)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;


	/* Deactivate IRQs. */
	ptSqiArea->ulSqi_irq_mask = 0;
	/* Clear all pending IRQs. */
	ulValue  = HOSTMSK(sqi_irq_clear_RORIC);
	ulValue |= HOSTMSK(sqi_irq_clear_RTIC);
	ulValue |= HOSTMSK(sqi_irq_clear_RXIC);
	ulValue |= HOSTMSK(sqi_irq_clear_TXIC);
	ulValue |= HOSTMSK(sqi_irq_clear_rxneic);
	ulValue |= HOSTMSK(sqi_irq_clear_rxfic);
	ulValue |= HOSTMSK(sqi_irq_clear_txeic);
	ulValue |= HOSTMSK(sqi_irq_clear_trans_end);
	ptSqiArea->ulSqi_irq_clear = ulValue;
	/* Deactivate IRQ routing to the CPUs. */
	ptSqiArea->ulSqi_irq_cpu_sel = 0;

	/* Deactivate DMAs. */
	ptSqiArea->ulSqi_dmacr = 0;

	/* Deactivate XIP. */
	ptSqiArea->ulSqi_sqirom_cfg = 0;

	ptSqiArea->ulSqi_tcr = 0;
	ptSqiArea->ulSqi_pio_oe = 0;
	ptSqiArea->ulSqi_pio_out = 0;

	/* Deactivate the unit. */
	ptSqiArea->aulSqi_cr[0] = 0;
	ptSqiArea->aulSqi_cr[1] = 0;

	/* Deactivate the SPI pins. */
	boot_spi_deactivate_mmio(ptCfg, aatMmioValues[ptCfg->uiChipSelect]);
}


int boot_drv_sqi_init(SPI_CFG_T *ptCfg, const BOOT_SPI_CONFIGURATION_T *ptSpiCfg, unsigned int uiChipSelect)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;
	int iResult;
	unsigned int uiIdleCfg;
	unsigned char ucIdleChar;


	iResult = 0;

	/* Initial device speed in kHz. */
	ptCfg->ulSpeed = ptSpiCfg->ulInitialSpeedKhz;
	/* The idle configuration. */
	ptCfg->uiIdleCfg = ptSpiCfg->ucIdleCfg;
	/* Bus mode. */
	ptCfg->tMode = ptSpiCfg->ucMode;
	/* Chip select. */
	ptCfg->uiChipSelect = 1U<<uiChipSelect;

	/* Set the function pointers. */
	ptCfg->pfnSelect = qsi_slave_select;
	ptCfg->pfnSendIdle = qsi_send_idle;
	ptCfg->pfnReadSimpleChecksum = qsi_read_simple_chksum;
	ptCfg->pfnSetNewSpeed = qsi_set_new_speed;
	ptCfg->pfnExchangeByte = qsi_spi_exchange_byte;
	ptCfg->pfnGetDeviceSpeedRepresentation = qsi_get_device_speed_representation;
	ptCfg->pfnDeactivate = qsi_deactivate;

	/* Copy the MMIO pins. */
	memcpy(ptCfg->aucMmio, ptSpiCfg->aucMmio, sizeof(ptSpiCfg->aucMmio));

	/* Do not use IRQs in bootloader. */
	ptSqiArea->ulSqi_irq_mask = 0;
	/* Clear all pending IRQs. */
	ulValue  = HOSTMSK(sqi_irq_clear_RORIC);
	ulValue |= HOSTMSK(sqi_irq_clear_RTIC);
	ulValue |= HOSTMSK(sqi_irq_clear_RXIC);
	ulValue |= HOSTMSK(sqi_irq_clear_TXIC);
	ulValue |= HOSTMSK(sqi_irq_clear_rxneic);
	ulValue |= HOSTMSK(sqi_irq_clear_rxfic);
	ulValue |= HOSTMSK(sqi_irq_clear_txeic);
	ulValue |= HOSTMSK(sqi_irq_clear_trans_end);
	ptSqiArea->ulSqi_irq_clear = ulValue;
	/* Do not route the IRQs to a CPU. */
	ptSqiArea->ulSqi_irq_cpu_sel = 0;

	/* Do not use DMA. */
	ptSqiArea->ulSqi_dmacr = 0;

	/* Do not use XIP. */
	ptSqiArea->ulSqi_sqirom_cfg = 0;

	/* Set 8 bits. */
	ulValue  = 7 << HOSTSRT(sqi_cr0_datasize);
	/* Set speed and filter. */
	ulValue |= qsi_get_device_speed_representation(ptCfg->ulSpeed);
	/* Start in SPI mode: use only IO0 and IO1 for transfer. */
	ulValue |= 0 << HOSTSRT(sqi_cr0_sio_cfg);
	/* Set the clock polarity.  */
	if( (ptCfg->tMode==SPI_MODE2) || (ptCfg->tMode==SPI_MODE3) )
	{
		ulValue |= HOSTMSK(sqi_cr0_sck_pol);
	}
	/* Set the clock phase. */
	if( (ptCfg->tMode==SPI_MODE1) || (ptCfg->tMode==SPI_MODE3) )
	{
		ulValue |= HOSTMSK(sqi_cr0_sck_phase);
	}
	ptSqiArea->aulSqi_cr[0] = ulValue;


	/* Manual chip select. */
	ulValue  = HOSTMSK(sqi_cr1_fss_static);
	/* Manual transfer start. */
	ulValue |= HOSTMSK(sqi_cr1_spi_trans_ctrl);
	/* Enable the interface. */
	ulValue |= HOSTMSK(sqi_cr1_sqi_en);
	/* Clear both FIFOs. */
	ulValue |= HOSTMSK(sqi_cr1_rx_fifo_clr)|HOSTMSK(sqi_cr1_tx_fifo_clr);
	ptSqiArea->aulSqi_cr[1] = ulValue;


	uiIdleCfg = ptCfg->uiIdleCfg;

	/* Set transfer control base. */
	ulValue  = HOSTMSK(sqi_tcr_ms_bit_first);
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO1_OE)!=0 )
	{
		ulValue |= HOSTMSK(sqi_tcr_tx_oe);
	}
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO1_OUT)!=0 )
	{
		ulValue |= HOSTMSK(sqi_tcr_tx_out);
	}
	ptCfg->ulTrcBase = ulValue;
	ptSqiArea->ulSqi_tcr = ulValue;

	ulValue = 0;
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO2_OUT)!=0 )
	{
		ulValue |= HOSTMSK(sqi_pio_out_sio2);
	}
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO3_OUT)!=0 )
	{
		ulValue |= HOSTMSK(sqi_pio_out_sio3);
	}
	ptSqiArea->ulSqi_pio_out = ulValue;

	ulValue = 0;
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO2_OE)!=0 )
	{
		ulValue |= HOSTMSK(sqi_pio_oe_sio2);
	}
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO3_OE)!=0 )
	{
		ulValue |= HOSTMSK(sqi_pio_oe_sio3);
	}
	ptSqiArea->ulSqi_pio_oe = ulValue;

	/* Set the idle char from the TX configuration. */
	if( (uiIdleCfg&MSK_SQI_CFG_IDLE_IO1_OUT)!=0 )
	{
		ucIdleChar = 0xffU;
	}
	else
	{
		ucIdleChar = 0x00U;
	}
	ptCfg->ucIdleChar = ucIdleChar;

	/* Activate the SPI pins. */
	boot_spi_activate_mmio(ptCfg, aatMmioValues[uiChipSelect]);

	return iResult;
}

