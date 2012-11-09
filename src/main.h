
#ifndef __MAIN_H__
#define __MAIN_H__

typedef enum
{
	RAMTESTCASE_08BIT  = 0x00000001,
	RAMTESTCASE_16BIT  = 0x00000002,
	RAMTESTCASE_32BIT  = 0x00000004,
	RAMTESTCASE_BURST  = 0x00000008
} RAMTESTCASE_T;

typedef struct
{
	unsigned long ulStart;
	unsigned long ulSize;
	unsigned long ulCases;
	unsigned long ulLoops;
} RAMTEST_PARAMETER_T;


#endif  /* __MAIN_H__ */
