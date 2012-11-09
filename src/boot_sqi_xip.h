/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include <stddef.h>

#include "boot_common.h"
#include "boot_spi.h"


#ifndef __SQI_H__
#define __SQI_H__


typedef enum ENUM_QSI_CMD
{
	QSI_MODE		= 0x00,		/* switch to mode (0=1bit, 1=2bit, 2=4bit) */
	QSI_ADR			= 0x08,		/* send address bytes specified in bits 0-2 starting msb */
	QSI_CMP			= 0x10,		/* compare the bytes in the buffer */
	QSI_CMP_FATAL		= 0x18,		/* compare the bytes in the buffer, fail if not equal */
	QSI_MASK		= 0x20,		/* mask the bytes in the buffer */

	QSI_SEND_NNN		= 0x80,		/* exchange bytes */
	QSI_SEND_SNN		= 0x88,		/* chip select, exchange bytes */
	QSI_SEND_SDN		= 0x90,		/* chip select, exchange bytes, chip deselect */
	QSI_SEND_SDD		= 0x98,		/* chip select, exchange bytes, chip deselect, 1 dummy transfer */
	QSI_RECEIVE_NNN		= 0xa0,		/* exchange bytes */
	QSI_RECEIVE_SNN		= 0xa8,		/* chip select, exchange bytes */
	QSI_RECEIVE_SDN		= 0xb0,		/* chip select, exchange bytes, chip deselect */
	QSI_RECEIVE_SDD		= 0xb8,		/* chip select, exchange bytes, chip deselect, 1 dummy transfer */
	QSI_DUMMY_NNN		= 0xc0,		/* transfer n dummy bytes of 0x00 */
	QSI_DUMMY_SNN		= 0xc8,		/* transfer n dummy bytes of 0x00 */
	QSI_DUMMY_SDN		= 0xd0,		/* transfer n dummy bytes of 0x00 */
	QSI_DUMMY_SDD		= 0xd8,		/* transfer n dummy bytes of 0x00 */

	QSI_SKIP_EQ		= 0xe0,		/* skip n bytes if last cmp was equal */
	QSI_SKIP_NE		= 0xf0		/* skip n bytes if last cmp was not equal */
} QSI_CMD_T;


typedef struct STRUCT_SQI_CONFIGURATION
{
	BOOT_SPI_CONFIGURATION_T tSpiCfg;
	unsigned long ulSqiRomCfg;
/*	unsigned char ucIdleCfg; */
	unsigned char ucSeqId_Length;
	unsigned char tSeqId[1+48];
	unsigned char tSeqRead[1+16];
	unsigned char tSeqDeactivate[1+16];
} SQI_CONFIGURATION_T;



typedef struct STRUCT_QSI_CFG
{
	SPI_CFG_T tCfg;			/* the unit configuration */
	unsigned long ulSqiRomCfg;	/* the xip configuration */
	unsigned long ulAddress;	/* the current address */
	size_t sizBufferPos;		/* the write position of the receive buffer */
	union
	{
		unsigned char auc[8];
		unsigned short aus[8/sizeof(unsigned short)];
		unsigned long aul[8/sizeof(unsigned long)];
	} uBuffer;			/* the receive buffer in different flavours */
} QSI_CFG_T;


BOOTING_T boot_sqi_xip(void);


#endif	/* __SQI_H__ */

