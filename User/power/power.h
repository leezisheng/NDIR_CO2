#ifndef __POWER_H__
#define __POWER_H__

/*
	本文件只用于生成供电2Hz(频率2Hz，占空比3%),
*/

#include <stm32f10x.h>

#define power_tim								TIM15
#define power_APBxClock_CMD				RCC_APB2PeriphClockCmd
#define power_CLK								RCC_APB2Periph_TIM15

#define power_PERTOD							(10000-1)
#define power_PSC								(400-1)
#define power_CCR1						 	9700
#define power_CCR2					     	9700

#define power_CH_GPIO_CLK					RCC_APB2Periph_GPIOA
#define power_CH_PORT						GPIOA

#define power_CH1_PIN						GPIO_Pin_2
#define power_CH2_PIN						GPIO_Pin_3

void power_init(void);

#endif


