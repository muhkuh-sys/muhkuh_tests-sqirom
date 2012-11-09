/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include "boot_spi.h"


void boot_spi_activate_mmio(const SPI_CFG_T *ptCfg, const MMIO_CFG_T *ptMmioValues)
{
	HOSTDEF(ptAsicCtrlArea)
	HOSTDEF(ptMmioCtrlArea)
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned int uiMmioPin;
	MMIO_CFG_T tMmioCfg;


	/* loop over all pins */
	pucCnt = ptCfg->aucMmio;
	pucEnd = pucCnt + sizeof(ptCfg->aucMmio);
	do
	{
		/* get the mmio config */
		tMmioCfg = *(ptMmioValues++);
		/* get the pin index */
		uiMmioPin = *(pucCnt++);
		/* is the index valid? */
		if( tMmioCfg!=0xffU && uiMmioPin!=0xffU )
		{
			/* activate the pin */
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptMmioCtrlArea->aulMmio_cfg[uiMmioPin] = tMmioCfg;
		}
	} while( pucCnt<pucEnd );
}


void boot_spi_deactivate_mmio(const SPI_CFG_T *ptCfg, const MMIO_CFG_T *ptMmioValues)
{
	HOSTDEF(ptAsicCtrlArea)
	HOSTDEF(ptMmioCtrlArea)
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned int uiMmioPin;
	MMIO_CFG_T tMmioCfg;


	/* loop over all pins */
	pucCnt = ptCfg->aucMmio;
	pucEnd = pucCnt + sizeof(ptCfg->aucMmio);
	do
	{
		/* get the mmio config */
		tMmioCfg = *(ptMmioValues++);
		/* get the pin index */
		uiMmioPin = *(pucCnt++);
		/* is the index valid? */
		if( tMmioCfg!=0xffU && uiMmioPin!=0xffU )
		{
			/* deactivate the pin */
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptMmioCtrlArea->aulMmio_cfg[uiMmioPin] = MMIO_CFG_DISABLE;
		}
	} while( pucCnt<pucEnd );
}

