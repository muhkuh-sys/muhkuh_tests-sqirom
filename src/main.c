
#include <string.h>

#include "netx_io_areas.h"

#include "boot_sqi_xip.h"
#include "main.h"
#include "rdy_run.h"
#include "netx_test.h"
#include "systime.h"
#include "uprintf.h"

#include "serial_vectors.h"

/*-----------------------------------*/


TEST_RESULT_T test(TEST_PARAMETER_T *ptTestParam)
{
	TEST_RESULT_T tTestResult;
	SQITEST_PARAMETER_T *ptTestParams;
	QSI_CFG_T tQsiCfg;
	/* the load address of an SQI XIP image must be at the start of SQI ROM right after the boot block */
	const unsigned char * const pucSqiXipAddress = (const unsigned char * const)(HOSTADDR(sqirom));


	systime_init();

	ptTestParams = (SQITEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Say "hi". */
	uprintf("\f. *** SQI test start ***\n");

	tTestResult = boot_sqi_xip(&tQsiCfg);
	if( tTestResult==TEST_RESULT_OK )
	{
		memcpy(ptTestParams->pucBuffer, pucSqiXipAddress+ptTestParams->ulOffset, ptTestParams->ulSize);
	}

	return tTestResult;
}


/*-----------------------------------*/

