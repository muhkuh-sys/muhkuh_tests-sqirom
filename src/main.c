
#include <string.h>

#include "netx_io_areas.h"

#include "boot_sqi_xip.h"
#include "main.h"
#include "rdy_run.h"
#include "netx_test.h"
#include "systime.h"
#include "uprintf.h"


/*-----------------------------------*/


TEST_RESULT_T test(TEST_PARAMETER_T *ptTestParam)
{
	TEST_RESULT_T tTestResult;
	SQITEST_PARAMETER_T *ptTestParams;
	QSI_CFG_T tQsiCfg;
	const unsigned char * const pucSqiRomAddress = (const unsigned char * const)(HOSTADDR(sqirom));


	systime_init();

	ptTestParams = (SQITEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Say "hi". */
	uprintf("\f. *** SQI test start ***\n");
	uprintf(". offset: 0x%08x\n", ptTestParams->ulOffset);
	uprintf(". size:   0x%08x\n", ptTestParams->ulSize);
	uprintf(". buffer: 0x%08x\n", ptTestParams->pucBuffer);

	tTestResult = boot_sqi_xip(&tQsiCfg);
	if( tTestResult==TEST_RESULT_OK )
	{
		memcpy(ptTestParams->pucBuffer, pucSqiRomAddress+ptTestParams->ulOffset, ptTestParams->ulSize);
	}

	return tTestResult;
}


/*-----------------------------------*/

