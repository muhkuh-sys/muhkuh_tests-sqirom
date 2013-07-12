
#include <string.h>

#include "netx_io_areas.h"

#include "boot_sqi_xip.h"
#include "main.h"
#include "rdy_run.h"
#include "netx_test.h"
#include "systime.h"
#include "uart.h"
#include "uprintf.h"

#include "serial_vectors.h"

/*-------------------------------------------------------------------------*/

#if ASIC_TYP==10
/* NXHX10-ETM */
static const UART_CONFIGURATION_T tUartCfg =
{
        .uc_rx_mmio = 20U,
        .uc_tx_mmio = 21U,
        .uc_rts_mmio = 0xffU,
        .uc_cts_mmio = 0xffU,
        .us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#elif ASIC_TYP==56
/* NXHX51-ETM */
static const UART_CONFIGURATION_T tUartCfg =
{
        .uc_rx_mmio = 34U,
        .uc_tx_mmio = 35U,
        .uc_rts_mmio = 0xffU,
        .uc_cts_mmio = 0xffU,
        .us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#elif ASIC_TYP==50
/* NXHX50-ETM */
static const UART_CONFIGURATION_T tUartCfg =
{
        .uc_rx_mmio = 34U,
        .uc_tx_mmio = 35U,
        .uc_rts_mmio = 0xffU,
        .uc_cts_mmio = 0xffU,
        .us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#elif ASIC_TYP==100 || ASIC_TYP==500
static const UART_CONFIGURATION_T tUartCfg =
{
        .us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#endif


#define IO_UART_UNIT 0

static unsigned char io_uart_get(void)
{
	return (unsigned char)uart_get(IO_UART_UNIT);
}


static void io_uart_put(unsigned int uiChar)
{
	uart_put(IO_UART_UNIT, (unsigned char)uiChar);
}


static unsigned int io_uart_peek(void)
{
	return uart_peek(IO_UART_UNIT);
}


static void io_uart_flush(void)
{
	uart_flush(IO_UART_UNIT);
}


static const SERIAL_COMM_UI_FN_T tSerialVectors_Uart =
{
	.fn =
	{
		.fnGet = io_uart_get,
		.fnPut = io_uart_put,
		.fnPeek = io_uart_peek,
		.fnFlush = io_uart_flush
	}
};


SERIAL_COMM_UI_FN_T tSerialVectors;


/*-----------------------------------*/

void test_main(void) __attribute__((noreturn));
void test_main(void)
{
	TEST_RESULT_T tTestResult;
	QSI_CFG_T tQsiCfg;
	/* the load address of an SQI XIP image must be at the start of SQI ROM right after the boot block */
	const unsigned char * const pucSqiXipAddress = (const unsigned char * const)(HOSTADDR(sqirom));


	systime_init();
	uart_init(IO_UART_UNIT, &tUartCfg);

	/* Set the serial vectors. */
	memcpy(&tSerialVectors, &tSerialVectors_Uart, sizeof(SERIAL_COMM_UI_FN_T));

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Say "hi". */
	uprintf("\f. *** SQI test start ***\n");

	tTestResult = boot_sqi_xip(&tQsiCfg);
	if( tTestResult!=TEST_RESULT_OK )
	{
		uprintf("ERROR: Failed to activate SQI ROM!\n");
		rdy_run_setLEDs(RDYRUN_YELLOW);
		while(1) {};
	}
	else
	{
		while(1)
		{
			uprintf("\fSQI ROM:\n");
			hexdump(pucSqiXipAddress, 512);
		}
	}
}

/*-----------------------------------*/

