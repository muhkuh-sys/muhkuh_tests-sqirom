/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#ifndef __BOOT_SPI_H__
#define __BOOT_SPI_H__

#include <stddef.h>

#include "netx_io_areas.h"


#define MSK_SQI_CFG_IDLE_IO1_OE		0x01U
#define SRT_SQI_CFG_IDLE_IO1_OE		0
#define MSK_SQI_CFG_IDLE_IO1_OUT	0x02U
#define SRT_SQI_CFG_IDLE_IO1_OUT	1
#define MSK_SQI_CFG_IDLE_IO2_OE		0x04U
#define SRT_SQI_CFG_IDLE_IO2_OE		2
#define MSK_SQI_CFG_IDLE_IO2_OUT	0x08U
#define SRT_SQI_CFG_IDLE_IO2_OUT	3
#define MSK_SQI_CFG_IDLE_IO3_OE		0x10U
#define SRT_SQI_CFG_IDLE_IO3_OE		4
#define MSK_SQI_CFG_IDLE_IO3_OUT	0x20U
#define SRT_SQI_CFG_IDLE_IO3_OUT	5


typedef enum ENUM_SPI_MODE
{
	SPI_MODE0 = 0,
	SPI_MODE1 = 1,
	SPI_MODE2 = 2,
	SPI_MODE3 = 3
} SPI_MODE_T;


typedef struct STRUCT_BOOT_SPI_CONFIGURATION
{
	unsigned long ulInitialSpeedKhz;
	unsigned char ucIdleCfg;
	unsigned char ucMode;
	unsigned char aucMmio[4];
} BOOT_SPI_CONFIGURATION_T;


/* predef for the functions */
struct STRUCT_SPI_CFG;

typedef int (*PFN_SPI_INIT)(struct STRUCT_SPI_CFG *psCfg, const BOOT_SPI_CONFIGURATION_T *ptSpiCfg, unsigned int uiChipSelect);
typedef int (*PFN_SPI_SLAVE_SELECT_T)(const struct STRUCT_SPI_CFG *psCfg, int fIsSelected);
typedef void (*PFN_SEND_IDLE_T)(const struct STRUCT_SPI_CFG *psCfg, unsigned int uiIdleChars);
typedef unsigned long (*PFN_READ_SIMPLE_CHKSUM_T)(const struct STRUCT_SPI_CFG *psCfg, unsigned long *pulDst, size_t sizDword);
typedef int (*PFN_SET_NEW_SPEED_T)(unsigned long ulDeviceSpecificSpeed);
typedef unsigned char (*PFN_EXCHANGE_BYTE_T)(const struct STRUCT_SPI_CFG *ptCfg, unsigned char uiByte);
typedef unsigned long (*PFN_GET_DEVICE_SPEED_REPRESENTATION_T)(unsigned int uiSpeed);
typedef void (*PFN_DEACTIVATE_T)(const struct STRUCT_SPI_CFG *psCfg);


typedef struct STRUCT_SPI_CFG
{
	unsigned long ulSpeed;		/* device speed in kHz */
	unsigned int uiIdleCfg;		/* the idle configuration */
	SPI_MODE_T tMode;		/* bus mode */
	unsigned int uiChipSelect;	/* chip select */

	PFN_SPI_SLAVE_SELECT_T pfnSelect;
	PFN_SEND_IDLE_T pfnSendIdle;
	PFN_READ_SIMPLE_CHKSUM_T pfnReadSimpleChecksum;
	PFN_SET_NEW_SPEED_T pfnSetNewSpeed;
	PFN_EXCHANGE_BYTE_T pfnExchangeByte;
	PFN_GET_DEVICE_SPEED_REPRESENTATION_T pfnGetDeviceSpeedRepresentation;
	PFN_DEACTIVATE_T pfnDeactivate;

	unsigned char ucIdleChar;	/* the idle character */
	unsigned long ulTrcBase;	/* the base bits of the transfer control register */
	unsigned char aucMmio[4];	/* mmio pins */
} SPI_CFG_T;


void boot_spi_activate_mmio(const SPI_CFG_T *ptCfg, const MMIO_CFG_T *ptMmioValues);
void boot_spi_deactivate_mmio(const SPI_CFG_T *ptCfg, const MMIO_CFG_T *ptMmioValues);

#endif  /* __BOOT_SPI_H__ */

