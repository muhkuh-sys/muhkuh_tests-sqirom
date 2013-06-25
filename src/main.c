
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


	systime_init();

	ptTestParams = (SQITEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Say "hi". */
	uprintf("\f. *** SQI test start ***\n");

	tTestResult = boot_sqi_xip(ptTestParams->ulOffset, ptTestParams->ulSize, ptTestParams->pucBuffer);

	return tTestResult;
}

/*-----------------------------------*/

