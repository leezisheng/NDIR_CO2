#include "crc16.h"
#include "uart.h"


void CRC_Config(void)
{
	/* Enable CRC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

void CRC_calc(uint16_t value)
{
	uint32_t CRCValue = 0;
	CRCValue = CRC_CalcCRC(value);
	printf("\r\n32-bit CRC Ð£ÑéÂëÎª:0X%X\r\n", CRCValue);
	
}


