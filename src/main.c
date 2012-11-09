
#include "netx_io_areas.h"

#include "main.h"
#include "rdy_run.h"
#include "netx_test.h"
#include "systime.h"
#include "uprintf.h"

int burst_access(unsigned long *pulStartaddress, unsigned long *pulEndAddress, unsigned long ulSeed, unsigned long ulInc);

/*-----------------------------------*/

/* test byte access */
static TEST_RESULT_T ram_test_08bit(unsigned char *pucStart, unsigned char *pucEnd, unsigned long ulSeed, unsigned long ulIncVal)
{
	TEST_RESULT_T tResult;
	volatile unsigned char *pucCnt;
	unsigned long ulCntVal;
	unsigned char ucReadBack;


	/* fill ram */
	ulCntVal = ulSeed;
	pucCnt = pucStart;
	while( pucCnt<pucEnd )
	{
		*pucCnt = (unsigned char)(ulCntVal&0x000000ffU);
		++pucCnt;
		ulCntVal += ulIncVal;
	};
	rdy_run_setLEDs(RDYRUN_GREEN);

	/* read back and compare */
	tResult = TEST_RESULT_OK;

	ulCntVal = ulSeed;
	pucCnt = pucStart;
	while( pucCnt<pucEnd )
	{
		ucReadBack = *pucCnt;
		if( ucReadBack!=(unsigned char)ulCntVal )
		{
			uprintf("! 8 bit access at address 0x%08x failed (offset 0x%08x)\n", (unsigned long)pucCnt, (unsigned long)(pucCnt-pucStart));
			uprintf("! wrote value:     0x%02x\n", ulCntVal&0x000000ff);
			uprintf("! read back value: 0x%02x\n", ucReadBack);
			tResult = TEST_RESULT_ERROR;
			break;
		}
		++pucCnt;
		ulCntVal += ulIncVal;
	};
	rdy_run_setLEDs(RDYRUN_OFF);

	return tResult;
}


/* test word access */
static TEST_RESULT_T ram_test_16bit(unsigned short *pusStart, unsigned short *pusEnd, unsigned long ulSeed, unsigned long ulIncVal)
{
	TEST_RESULT_T tResult;
	volatile unsigned short *pusCnt;
	unsigned long ulCntVal;
	unsigned short usReadBack;


	/* fill ram */
	ulCntVal = ulSeed;
	pusCnt = pusStart;
	while( pusCnt<pusEnd )
	{
		*pusCnt = (unsigned short)(ulCntVal&0x0000ffffU);
		++pusCnt;
		ulCntVal += ulIncVal;
	};
	rdy_run_setLEDs(RDYRUN_GREEN);

	/* read back and compare */
	tResult = TEST_RESULT_OK;

	ulCntVal = ulSeed;
	pusCnt = pusStart;
	while( pusCnt<pusEnd )
	{
		usReadBack = *pusCnt;
		if( usReadBack!=(unsigned short)ulCntVal )
		{
			uprintf("! 16 bit access at address 0x%08x failed (offset 0x%08x)\n", (unsigned long)pusCnt, (unsigned long)(pusCnt-pusStart));
			uprintf("! wrote value:     0x%04x\n", ulCntVal&0x0000ffff);
			uprintf("! read back value: 0x%04x\n", usReadBack);
			tResult = TEST_RESULT_ERROR;
			break;
		}
		++pusCnt;
		ulCntVal += ulIncVal;
	};
	rdy_run_setLEDs(RDYRUN_OFF);

	return tResult;
}


/* Test DWORD access. */
static TEST_RESULT_T ram_test_32bit(unsigned long *pulStart, unsigned long *pulEnd, unsigned long ulSeed, unsigned long ulIncVal)
{
	TEST_RESULT_T tResult;
	volatile unsigned long *pulCnt;
	unsigned long ulCntVal;
	unsigned long ulReadBack;


	/* fill ram */
	ulCntVal = ulSeed;
	pulCnt = pulStart;
	while( pulCnt<pulEnd )
	{
		*(pulCnt++) = ulCntVal;
		ulCntVal += ulIncVal;
	};
	rdy_run_setLEDs(RDYRUN_GREEN);

	/* read back and compare */
	tResult = TEST_RESULT_OK;

	ulCntVal = ulSeed;
	pulCnt = pulStart;
	while( pulCnt<pulEnd )
	{
		ulReadBack = *pulCnt;
		if( ulReadBack!=ulCntVal )
		{
			uprintf("! 32 bit access at address 0x%08x failed (offset 0x%08x)\n", (unsigned long)pulCnt, (unsigned long)(pulCnt-pulStart));
			uprintf("! wrote value:     0x%08x\n", ulCntVal);
			uprintf("! read back value: 0x%08x\n", ulReadBack);
			tResult = TEST_RESULT_ERROR;
			break;
		}
		++pulCnt;
		ulCntVal += ulIncVal;
	};
	rdy_run_setLEDs(RDYRUN_OFF);

	return tResult;
}

/*-----------------------------------*/

typedef union
{
	unsigned char *puc;
	unsigned short *pus;
	unsigned long *pul;
	unsigned long ul;
} ADR_T;

static TEST_RESULT_T test_ram(RAMTEST_PARAMETER_T *ptParameter, unsigned long ulSeed, unsigned long ulIncVal)
{
	TEST_RESULT_T tResult;
	unsigned long ulCases;
	ADR_T uCnt;
	ADR_T uEnd;
	int iResult;


	ulCases = ptParameter->ulCases;
	tResult = TEST_RESULT_OK;

	uCnt.ul = ptParameter->ulStart;
	uEnd.ul = ptParameter->ulStart + ptParameter->ulSize;

	/* test 8 bit access */
	if( tResult==TEST_RESULT_OK && (ulCases&RAMTESTCASE_08BIT)!=0 )
	{
		uprintf(". Testing 8 bit access...\n");
		tResult = ram_test_08bit(uCnt.puc, uEnd.puc, ulSeed, ulIncVal);
		if( tResult==TEST_RESULT_OK )
		{
			uprintf(". 8 bit access OK.\n");
		}
		else
		{
			uprintf("! 8 bit access failed.\n");
		}
	}

	/* test 16 bit access */
	if( tResult==TEST_RESULT_OK && (ulCases&RAMTESTCASE_16BIT)!=0 )
	{
		uprintf(". Testing 16 bit access...\n");
		tResult = ram_test_16bit(uCnt.pus, uEnd.pus, ulSeed, ulIncVal);
		if( tResult==TEST_RESULT_OK )
		{
			uprintf(". 16 bit access OK.\n");
		}
		else
		{
			uprintf("! 16 bit access failed.\n");
		}
	}

	/* test 32 bit access */
	if( tResult==TEST_RESULT_OK && (ulCases&RAMTESTCASE_32BIT)!=0 )
	{
		uprintf(". Testing 32 bit access...\n");
		tResult = ram_test_32bit(uCnt.pul, uEnd.pul, ulSeed, ulIncVal);
		if( tResult==TEST_RESULT_OK )
		{
			uprintf(". 32 bit access OK.\n");
		}
		else
		{
			uprintf("! 32 bit access failed.\n");
		}
	}

	/* test 32 bit burst access */
	if( tResult==TEST_RESULT_OK && (ulCases&RAMTESTCASE_BURST)!=0 )
	{
		uprintf(". Testing 32 bit burst access...\n");
		rdy_run_setLEDs(RDYRUN_GREEN);
		iResult = burst_access(uCnt.pul, uEnd.pul, ulSeed, ulIncVal);
		rdy_run_setLEDs(RDYRUN_OFF);
		if( iResult==0 )
		{
			uprintf(". 32 bit burst access OK.\n");
		}
		else
		{
			uprintf("! 32 bit burst access failed.\n");
			tResult = TEST_RESULT_ERROR;
		}
	}

	return tResult;
}

/*-----------------------------------*/

typedef struct
{
	unsigned long uiSeed;
	unsigned long uiIncVal;
} testPair_t;

static const testPair_t testPairs[4] =
{
	{   5, 113 },
	{  71,  43 },
	{ 191,   7 },
	{  53,  97 }
};

TEST_RESULT_T test(TEST_PARAMETER_T *ptTestParam)
{
	TEST_RESULT_T tTestResult;
//	unsigned long ulSdramReady;
	unsigned int uiTestCnt;
	RAMTEST_PARAMETER_T *ptTestParams;
	unsigned long ulCases;
	unsigned long ulLoopMax;
	unsigned long ulLoopCnt;


	systime_init();

	ptTestParams = (RAMTEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Say "hi". */
	uprintf("\f. *** Ram test start ***\n");
	uprintf(". Ram start address:      0x%08x\n", ptTestParams->ulStart);
	uprintf(". Ram size in bytes:      0x%08x\n", ptTestParams->ulSize);
	uprintf(". Test cases:\n");
	ulCases = ptTestParams->ulCases;
	if( ulCases==0 )
	{
		uprintf("    none\n");
	}
	if( (ulCases&RAMTESTCASE_08BIT)!=0 )
	{
		uprintf("     8 bit\n");
	}
	if( (ulCases&RAMTESTCASE_16BIT)!=0 )
	{
		uprintf("    16 bit\n");
	}
	if( (ulCases&RAMTESTCASE_32BIT)!=0 )
	{
		uprintf("    32 bit\n");
	}
	if( (ulCases&RAMTESTCASE_BURST)!=0 )
	{
		uprintf("    Burst\n");
	}
	ulLoopMax = ptTestParams->ulLoops;
	if( ulLoopMax==0 )
	{
		uprintf(". Loops:                  endless\n");
	}
	else
	{
		uprintf(". Loops:                  0x%08x\n", ulLoopMax);
	}
	uprintf("\n\n");

	/* Run test cases. */
	ulLoopCnt = 0;
	do
	{
		++ulLoopCnt;

		uprintf("*******************\n");
		uprintf("*                 *\n");
		uprintf("*  Loop %08d  *\n", ulLoopCnt);
		uprintf("*                 *\n");
		uprintf("*******************\n");

		for(uiTestCnt=0; uiTestCnt<(sizeof(testPairs)/sizeof(testPair_t)); ++uiTestCnt )
		{
			uprintf(". Start test case %d of %d\n", uiTestCnt+1, sizeof(testPairs)/sizeof(testPair_t));
			tTestResult = test_ram(ptTestParams, testPairs[uiTestCnt].uiSeed, testPairs[uiTestCnt].uiIncVal);
			if( tTestResult!=TEST_RESULT_OK )
			{
				uprintf("! Test case %d failed.\n", uiTestCnt+1);
				rdy_run_setLEDs(RDYRUN_YELLOW);
				break;
			}
			uprintf(". Test case %d OK.\n", uiTestCnt+1);
		}

		if( tTestResult==TEST_RESULT_OK )
		{
			/* Test loop OK. */
			uprintf("* OK *\n");
		}

		if( ulLoopMax!=0 && ulLoopCnt>=ulLoopMax )
		{
			break;
		}
	} while(tTestResult==TEST_RESULT_OK);

	return tTestResult;
}

/*-----------------------------------*/

