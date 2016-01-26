/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include <string.h>

#include "boot_sqi_xip.h"

#include "boot_drv_sqi.h"
#include "netx_io_areas.h"
#include "options.h"
#include "uprintf.h"


#if CFG_DEBUGMSG==1
	#define DEBUGZONE(n)  (g_t_romloader_options.t_debug_settings.ul_boot_sqi_xip&(0x00000001<<(n)))

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

	#define DEBUGMSG(cond,...) ((void)((cond)?(uprintf(__VA_ARGS__)),1:0))
#else
	#define DEBUGMSG(cond,...) ((void)0)
#endif


static void qsi_seq_reset_input_buffer(QSI_CFG_T *ptQsiCfg)
{
	DEBUGMSG(ZONE_VERBOSE, ". reset the input buffer.\n");

	ptQsiCfg->sizBufferPos = 0;
	memset(ptQsiCfg->uBuffer.auc, 0, sizeof(ptQsiCfg->uBuffer.auc));
}


static int qsi_seq_compare(QSI_CFG_T *ptQsiCfg, size_t sizSeq, const unsigned char *pucSeq)
{
	const unsigned char *pucEnd;
	const unsigned char *pucBuf;
	int iResult;


	/* expect success */
	iResult = 0;

	pucEnd = pucSeq + sizSeq;
	pucBuf = ptQsiCfg->uBuffer.auc;

#if CFG_DEBUGMSG==1
	DEBUGMSG(ZONE_VERBOSE, ". compare buffer with sequence.\n");
	DEBUGMSG(ZONE_VERBOSE, ". buffer:\n");
	if( ZONE_VERBOSE )
	{
		hexdump(pucBuf, sizSeq);
	}
	DEBUGMSG(ZONE_VERBOSE, ". Compare sequence:\n");
	if( ZONE_VERBOSE )
	{
		hexdump(pucSeq, sizSeq);
	}
#endif

	while(pucSeq<pucEnd)
	{
		iResult =  *(pucSeq++) - *(pucBuf++);
		if( iResult!=0 )
		{
			break;
		}
	}

	if( iResult==0 )
	{
		DEBUGMSG(ZONE_VERBOSE, ". Ok!\n");
	}
	else
	{
		DEBUGMSG(ZONE_ERROR, "! Buffer and sequence differ!\n");
	}

	return iResult;
}


static void qsi_seq_mask(QSI_CFG_T *ptQsiCfg, size_t sizSeq, const unsigned char *pucSeq)
{
	const unsigned char *pucEnd;
	unsigned char *pucBuf;


	pucEnd = pucSeq + sizSeq;
	pucBuf = ptQsiCfg->uBuffer.auc;

#if CFG_DEBUGMSG==1
	DEBUGMSG(ZONE_VERBOSE, ". Mask the buffer.\n");
	DEBUGMSG(ZONE_VERBOSE, ". Buffer: \n");
	if( ZONE_VERBOSE )
	{
		hexdump(pucBuf, sizSeq);
	}
	DEBUGMSG(ZONE_VERBOSE, ". Mask sequence:\n");
	if( ZONE_VERBOSE )
	{
		hexdump(pucSeq, sizSeq);
	}
#endif

	while( pucSeq<pucEnd )
	{
		*(pucBuf++) &= *(pucSeq++);
	}

#if CFG_DEBUGMSG==1
	DEBUGMSG(ZONE_VERBOSE, ". Masked Buffer: \n");
	if( ZONE_VERBOSE )
	{
		hexdump(ptQsiCfg->uBuffer.auc, sizSeq);
	}
#endif
}


static void qsi_receive(QSI_CFG_T *ptQsiCfg, unsigned int uiParameter)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;
	unsigned char ucByte;
	size_t sizBufferPos;
	unsigned long ulData;
	unsigned int uiByteCnt;


	/* set mode to "receive" */
	ulValue  = ptQsiCfg->tCfg.ulTrcBase;
	ulValue |= 1 << HOSTSRT(sqi_tcr_duplex);
	/* set the transfer size */
	ulValue |= uiParameter << HOSTSRT(sqi_tcr_transfer_size);
	/* start the transfer */
	ulValue |= HOSTMSK(sqi_tcr_start_transfer);
	ptSqiArea->ulSqi_tcr = ulValue;

	/* get buffer position */
	sizBufferPos = ptQsiCfg->sizBufferPos;

	/* check the mode */
	if( (ptQsiCfg->tCfg.ulTrcBase&HOSTMSK(sqi_tcr_mode))==0 )
	{
		/* mode 0 : the fifo size is 8 bit */
		do
		{
			/* wait for one byte in the fifo */
			do
			{
				ulValue  = ptSqiArea->ulSqi_sr;
				ulValue &= HOSTMSK(sqi_sr_rx_fifo_not_empty);
			} while( ulValue==0 );
			/* grab byte */
			ucByte = (unsigned char)(ptSqiArea->ulSqi_dr);
			ptQsiCfg->uBuffer.auc[sizBufferPos] = ucByte;
			DEBUGMSG(ZONE_VERBOSE, ". rec $%02x\n", ucByte);
			/* move and wrap buffer pointer */
			++sizBufferPos;
			sizBufferPos &= 0x0f;
		} while( (uiParameter--)>0 );
	}
	else
	{
		/* DSI/QSI mode : the fifo size is 32 bit */
		++uiParameter;
		do
		{
			/* wait for one byte in the fifo */
			do
			{
				ulValue  = ptSqiArea->ulSqi_sr;
				ulValue &= HOSTMSK(sqi_sr_rx_fifo_not_empty);
			} while( ulValue==0 );

			/* get the dword */
			ulData = ptSqiArea->ulSqi_dr;
			DEBUGMSG(ZONE_VERBOSE, "received dw: $%08x\n", ulData);

			/* process the dword */
			uiByteCnt = 4;
			do
			{
				/* grab byte */
				ucByte = (unsigned char)ulData;
				ptQsiCfg->uBuffer.auc[sizBufferPos] = ucByte;
				DEBUGMSG(ZONE_VERBOSE, ". rec $%02x\n", ucByte);
				/* move to next byte */
				ulData >>= 8;
				--uiByteCnt;
				/* move and wrap buffer pointer */
				++sizBufferPos;
				sizBufferPos &= 0x0f;
				/* one byte processed */
				--uiParameter;
			} while( uiParameter!=0 && uiByteCnt!=0 );
		} while( uiParameter!=0 );
	}

	/* update buffer position */
	ptQsiCfg->sizBufferPos = sizBufferPos;

	/* wait until the transfer is done */
	do
	{
		ulValue  = ptSqiArea->ulSqi_sr;
		ulValue &= HOSTMSK(sqi_sr_busy);
	} while( ulValue!=0 );
}


static void qsi_send(QSI_CFG_T *ptQsiCfg, unsigned int uiParameter, const unsigned char *pucData)
{
	HOSTDEF(ptSqiArea)
	unsigned long ulValue;
	unsigned char ucSend;
	unsigned int uiShiftCnt;
	unsigned long ulSend;


	DEBUGMSG(ZONE_VERBOSE, ". sending %d bytes\n", uiParameter+1);

	/* set mode to "send" */
	ulValue  = ptQsiCfg->tCfg.ulTrcBase;
	ulValue |= 2 << HOSTSRT(sqi_tcr_duplex);
	/* set the transfer size */
	ulValue |= uiParameter << HOSTSRT(sqi_tcr_transfer_size);
	/* start the transfer */
	ulValue |= HOSTMSK(sqi_tcr_start_transfer);
	ptSqiArea->ulSqi_tcr = ulValue;

	/* check the mode */
	if( (ptQsiCfg->tCfg.ulTrcBase&HOSTMSK(sqi_tcr_mode))==0 )
	{
		/* mode 0 : the fifo size is 8 bit */
		do
		{
			/* wait for space in the fifo */
			do
			{
				ulValue  = ptSqiArea->ulSqi_sr;
				ulValue &= HOSTMSK(sqi_sr_rx_fifo_full);
			} while( ulValue!=0 );

			/* send byte */
			ucSend = *(pucData++);
			ptSqiArea->ulSqi_dr = ucSend;
			DEBUGMSG(ZONE_VERBOSE, ". send $%02x\n", ucSend);
		} while( (uiParameter--)>0 );
	}
	else
	{
		/* DSI/QSI mode : the FIFO size is 32 bit */
		++uiParameter;
		do
		{
			/* collect a DWORD */
			ulSend = 0;
			uiShiftCnt = 0;
			do
			{
				ulSend |= ((unsigned long)(*(pucData++))) << (uiShiftCnt<<3U);
				++uiShiftCnt;
				--uiParameter;
			} while( uiParameter!=0 && uiShiftCnt<4 );

			/* wait for space in the FIFO */
			do
			{
				ulValue  = ptSqiArea->ulSqi_sr;
				ulValue &= HOSTMSK(sqi_sr_rx_fifo_full);
			} while( ulValue!=0 );
			/* send DWORD */
			ptSqiArea->ulSqi_dr = ulSend;
			DEBUGMSG(ZONE_VERBOSE, ". send $%08x\n", ulSend);
		} while( uiParameter!=0 );
	}

	/* wait until the transfer is done */
	do
	{
		ulValue  = ptSqiArea->ulSqi_sr;
		ulValue &= HOSTMSK(sqi_sr_busy);
	} while( ulValue!=0 );
}


static void qsi_seq_send_address(QSI_CFG_T *ptQsiCfg, unsigned int uiParameter)
{
	unsigned int uiByteCnt;
	unsigned long ulAddress;
	unsigned char aucAdr[3];



	uiByteCnt = 0;
	ulAddress = ptQsiCfg->ulAddress;
	while( uiParameter!=0 )
	{
		if( (uiParameter&0x04U)!=0 )
		{
			aucAdr[uiByteCnt] = (unsigned char)((ulAddress>>16U)&0xffU);
			++uiByteCnt;
		}
		uiParameter <<= 1U;
		ulAddress <<= 8U;
	}

	if( uiByteCnt>0 )
	{
		qsi_send(ptQsiCfg, uiByteCnt-1, aucAdr);
	}
}


static int qsi_seq_set_mode(QSI_CFG_T *ptQsiCfg, unsigned int uiMode)
{
	HOSTDEF(ptSqiArea)
	int iResult;
	unsigned long ulSioCfg;
	unsigned long ulValue;


	/* switch to mode (0=1bit, 1=2bit, 2=4bit) */
	DEBUGMSG(ZONE_VERBOSE, "Mode: %d\n", uiMode);

	/* expect success */
	iResult = 0;

	/* set mode */
	if( uiMode>2 )
	{
		iResult = -1;
	}
	else
	{
		/* get sio cfg */

		/* default is io0/1; sio, io2/3: pio */
		ulSioCfg = 0;
		/* check for QSI mode */
		if( uiMode==2 )
		{
			/* io0..3; sio */
			ulSioCfg = 1;
		}

		/* set the new sio cfg */
		ulValue  = ptSqiArea->aulSqi_cr[0];
		ulValue &= ~HOSTMSK(sqi_cr0_sio_cfg);
		ulValue |= ulSioCfg << HOSTSRT(sqi_cr0_sio_cfg);
		ptSqiArea->aulSqi_cr[0] = ulValue;

		/* change the mode */
		ulValue  = ptQsiCfg->tCfg.ulTrcBase;
		ulValue &= ~HOSTMSK(sqi_tcr_mode);
		ulValue |= uiMode<<HOSTSRT(sqi_tcr_mode);
		ptQsiCfg->tCfg.ulTrcBase = ulValue;
	}

	return iResult;
}


static int execute_sequence(QSI_CFG_T *ptCfg, const unsigned char *pucSeq)
{
	int iResult;
	size_t sizSeq;
	unsigned char ucCmd;
	QSI_CMD_T tQsiCmd;
	unsigned int uiParameter;
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	int iLastCompareResult;


	/* expect success */
	iResult = 0;

	/* no last compare result */
	iLastCompareResult = -1;

	qsi_seq_reset_input_buffer(ptCfg);

	/* get the sequence size */
	sizSeq = *(pucSeq++);

	pucCnt = pucSeq;
	pucEnd = pucCnt + sizSeq;
	while( iResult==0 && pucCnt<pucEnd )
	{
		ucCmd = *(pucCnt++);
		if( ucCmd<0xe0U )
		{
			tQsiCmd = ucCmd & 0xf8U;
			uiParameter = ucCmd & 0x07U;
		}
		else
		{
			tQsiCmd = ucCmd & 0xf0U;
			uiParameter = ucCmd & 0x0fU;
		}
		switch( tQsiCmd )
		{
		case QSI_MODE:
			qsi_seq_set_mode(ptCfg, uiParameter);
			break;


		case QSI_ADR:
			/* send address bytes specified in bits 0-3 starting msb */
			qsi_seq_send_address(ptCfg, uiParameter);
			break;


		case QSI_CMP:
		case QSI_CMP_FATAL:
			/* a value of 0 means 1 byte */
			++uiParameter;
			/* compare the bytes in the buffer */
			if( pucCnt+uiParameter>pucEnd )
			{
				/* the sequence does not fit into the remaining buffer */
				/* this is an error in the seqnence */
				DEBUGMSG(ZONE_ERROR, "! cmp argument too large!\n");
				iResult = -1;
			}
			else
			{
				/* compare the sequence */
				iLastCompareResult = qsi_seq_compare(ptCfg, uiParameter, pucCnt);
				if( tQsiCmd==QSI_CMP_FATAL && iLastCompareResult!=0 )
				{
					iResult = -1;
				}
				pucCnt += uiParameter;
			}
			break;


		case QSI_MASK:
			/* a value of 0 means 1 byte */
			++uiParameter;
			/* mask the bytes in the buffer */
			if( pucCnt+uiParameter>pucEnd )
			{
				/* the skip size does not fit into the remaining buffer */
				/* this is an error in the seqence */
				DEBUGMSG(ZONE_ERROR, "! mask argument too large!\n");
				iResult = -1;
			}
			else
			{
				qsi_seq_mask(ptCfg, uiParameter, pucCnt);
				pucCnt += uiParameter;
			}
			break;


		case QSI_SEND_NNN:
		case QSI_SEND_SNN:
		case QSI_SEND_SDN:
		case QSI_SEND_SDD:
			/* transfer n dummy bytes of 0x00 */
			if( tQsiCmd!=QSI_SEND_NNN )
			{
				if( ptCfg->tCfg.pfnSelect(&(ptCfg->tCfg), 1)==0 )
				{
					DEBUGMSG(ZONE_VERBOSE, ". select the slave.\n");
					qsi_seq_reset_input_buffer(ptCfg);
				}
			}
			qsi_send(ptCfg, uiParameter, pucCnt);
			pucCnt += uiParameter + 1;
			if( tQsiCmd>=QSI_SEND_SDN )
			{
				DEBUGMSG(ZONE_VERBOSE, ". deselect the slave\n");
				ptCfg->tCfg.pfnSelect(&(ptCfg->tCfg), 0);
				if( tQsiCmd>=QSI_SEND_SDD )
				{
					ptCfg->tCfg.pfnSendIdle(&(ptCfg->tCfg), 0);
				}
			}
			break;


		case QSI_RECEIVE_NNN:
		case QSI_RECEIVE_SNN:
		case QSI_RECEIVE_SDN:
		case QSI_RECEIVE_SDD:
			/* receive bytes */
			if( tQsiCmd!=QSI_RECEIVE_NNN )
			{
				if( ptCfg->tCfg.pfnSelect(&(ptCfg->tCfg), 1)==0 )
				{
					DEBUGMSG(ZONE_VERBOSE, ". select the slave.\n");
					qsi_seq_reset_input_buffer(ptCfg);
				}
			}
			qsi_receive(ptCfg, uiParameter);
			if( tQsiCmd>=QSI_RECEIVE_SDN )
			{
				DEBUGMSG(ZONE_VERBOSE, ". deselect the slave\n");
				ptCfg->tCfg.pfnSelect(&(ptCfg->tCfg), 0);
				if( tQsiCmd>=QSI_RECEIVE_SDD )
				{
					ptCfg->tCfg.pfnSendIdle(&(ptCfg->tCfg), 0);
				}
			}
			break;


		case QSI_DUMMY_NNN:
		case QSI_DUMMY_SNN:
		case QSI_DUMMY_SDN:
		case QSI_DUMMY_SDD:
			/* transfer n dummy bytes of 0x00 */
			if( tQsiCmd!=QSI_DUMMY_NNN )
			{
				if( ptCfg->tCfg.pfnSelect(&(ptCfg->tCfg), 1)==0 )
				{
					DEBUGMSG(ZONE_VERBOSE, ". select the slave.\n");
					qsi_seq_reset_input_buffer(ptCfg);
				}
			}
			ptCfg->tCfg.pfnSendIdle(&(ptCfg->tCfg), uiParameter);
			if( tQsiCmd>=QSI_DUMMY_SDN )
			{
				DEBUGMSG(ZONE_VERBOSE, ". deselect the slave\n");
				ptCfg->tCfg.pfnSelect(&(ptCfg->tCfg), 0);
				if( tQsiCmd>=QSI_DUMMY_SDD )
				{
					ptCfg->tCfg.pfnSendIdle(&(ptCfg->tCfg), 0);
				}
			}
			break;


		case QSI_SKIP_EQ:
		case QSI_SKIP_NE:
			/* a value of 0 means 1 byte */
			++uiParameter;
			/* skip n bytes if last cmp was equal */
			if( pucCnt+uiParameter>pucEnd )
			{
				/* the skip size does not fit into the remaining buffer */
				/* this is an error in the seqence */
				DEBUGMSG(ZONE_ERROR, "! skip argument too large!\n");
				iResult = -1;
			}
			else
			{
				if( (tQsiCmd==QSI_SKIP_EQ && iLastCompareResult==0) || (tQsiCmd==QSI_SKIP_NE && iLastCompareResult!=0) )
				{
					DEBUGMSG(ZONE_VERBOSE, ". Skipping %d bytes\n", uiParameter);
					pucCnt += uiParameter;
				}
				else
				{
					DEBUGMSG(ZONE_VERBOSE, ". Not skipping.\n");
				}
			}
			break;


		default:
			DEBUGMSG(ZONE_ERROR, "! unknown command: $%02x\n", tQsiCmd);
			iResult = -1;
			break;
		}
	}

	return iResult;
}


unsigned long aulBuffer[16];

TEST_RESULT_T boot_sqi_xip(QSI_CFG_T *ptQsiCfg)
{
	HOSTDEF(ptSqiArea);
	HOSTDEF(ptAsicCtrlArea);
	TEST_RESULT_T tResult;
	unsigned long ulValue;
	int iResult;
	/* the chip select is fixed to 0 */
	const unsigned int uiSqiXipChipselect = 0;


#if ASIC_TYP==56
	/* This is netX56 specific:
	 * The SQI ROM pins must me muxed out with io_config2.
	 * If this is masked out with io_config2_mask, SQI ROM is not possible.
	 */
	ulValue  = ptAsicCtrlArea->ulIo_config2_mask;
	ulValue &= HOSTMSK(io_config2_mask_sel_sqi);
	if( ulValue==0 )
	{
		DEBUGMSG(ZONE_VERBOSE, "The sqirom pins can not be muxed out. Booting disabled!\n");
		tResult = TEST_RESULT_ERROR;
	}
	else
#endif
	{
		/* Copy the XIP configuration. */
		ulValue = g_t_romloader_options.t_sqi_options.ulSqiRomCfg;
		ptQsiCfg->ulSqiRomCfg = ulValue;

		/* is the configuration valid? */
		ulValue &= HOSTMSK(sqi_sqirom_cfg_enable);
		if( ulValue==0 )
		{
			DEBUGMSG(ZONE_VERBOSE, "The sqirom enable bit is not set. Booting disabled!\n");
			tResult = TEST_RESULT_ERROR;
		}
		else
		{
			/* Init the config and the unit.*/
			boot_drv_sqi_init(&(ptQsiCfg->tCfg), &(g_t_romloader_options.t_sqi_options.tSpiCfg), uiSqiXipChipselect);

			DEBUGMSG(ZONE_VERBOSE, "*** sending init seq ***\n");

			/* init the address */
			ptQsiCfg->ulAddress = 0;

#if ASIC_TYP==56
			/* This is netX56 specific: Enable the SQI ROM pins. */
			ulValue  = ptAsicCtrlArea->ulIo_config2;
			ulValue |= HOSTMSK(io_config2_sel_sqi);
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptAsicCtrlArea->ulIo_config2 = ulValue;
#endif

			iResult = execute_sequence(ptQsiCfg, g_t_romloader_options.t_sqi_options.tSeqId);
			if( iResult!=0 )
			{
				DEBUGMSG(ZONE_ERROR, "error executing the id sequence: %d\n", iResult);
				tResult = TEST_RESULT_ERROR;
			}
			else
			{
				DEBUGMSG(ZONE_VERBOSE, "*** sending read seq ***\n");
				iResult = execute_sequence(ptQsiCfg, g_t_romloader_options.t_sqi_options.tSeqRead);
				if( iResult!=0 )
				{
					DEBUGMSG(ZONE_ERROR, "error executing the read sequence: %d\n", iResult);
					tResult = TEST_RESULT_ERROR;
				}
				else
				{
					/* receive the complete bootblock */
					ptQsiCfg->tCfg.pfnReadSimpleChecksum(&(ptQsiCfg->tCfg), aulBuffer, (sizeof(aulBuffer)/sizeof(aulBuffer[0])));

					/* deselect the chip and send idles */
					ptQsiCfg->tCfg.pfnSelect(&(ptQsiCfg->tCfg), 0U);
					ptQsiCfg->tCfg.pfnSendIdle(&(ptQsiCfg->tCfg), 0U);

					/* Activate the SQI XIP unit. */
					ptSqiArea->ulSqi_sqirom_cfg = ptQsiCfg->ulSqiRomCfg;

					tResult = TEST_RESULT_OK;
				}
			}
		}
	}

	return tResult;
}


void deactivate_sqi(QSI_CFG_T *ptQsiCfg)
{
	HOSTDEF(ptSqiArea);
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	int iResult;


	/* Deselect the chip and send idles. */
	ptQsiCfg->tCfg.pfnSelect(&(ptQsiCfg->tCfg), 0U);
	ptQsiCfg->tCfg.pfnSendIdle(&(ptQsiCfg->tCfg), 0U);

	/* Deactivate the SQI XIP unit. */
	ptSqiArea->ulSqi_sqirom_cfg = 0;

	DEBUGMSG(ZONE_VERBOSE, "*** sending deactivate sequence ***\n");
	iResult = execute_sequence(ptQsiCfg, g_t_romloader_options.t_sqi_options.tSeqDeactivate);
	if( iResult!=0 )
	{
		DEBUGMSG(ZONE_ERROR, "error executing the deactivate sequence: %d\n", iResult);
	}

	/* Deactivate the unit */
	ptQsiCfg->tCfg.pfnDeactivate(&(ptQsiCfg->tCfg));

#if ASIC_TYP==56
	/* This is netX56 specific: Disable the SQI ROM pins. */
	ulValue  = ptAsicCtrlArea->ulIo_config2;
	ulValue &= ~HOSTMSK(io_config2_sel_sqi);
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptAsicCtrlArea->ulIo_config2 = ulValue;
#endif
}

