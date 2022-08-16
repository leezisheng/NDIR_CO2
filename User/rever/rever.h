#ifndef __REVER_H__
#define __REVER_H__


/*
	本文件只用于生成反向(频率2Hz，占空比98%),
*/

#include <stm32f10x.h>

#define rever_tim								TIM17
#define rever_APBxClock_CMD				RCC_APB2PeriphClockCmd
#define rever_CLK								RCC_APB2Periph_TIM17

#define rever_PERTOD							(10000-1)
#define rever_PSC								(400-1)
#define rever_CCR1						 	9780

#define rever_CH_GPIO_CLK				RCC_APB2Periph_GPIOB
#define rever_CH_PORT						GPIOB

#define rever_CH1_PIN						GPIO_Pin_9


void rever_init(void);

#endif



