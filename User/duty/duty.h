#ifndef __DUTY_H__
#define __DUTY_H__


/*
	本文件只用于生成窄信号2Hz(频率2Hz，占空比0.3%，与供电信号相差4	ms的相位差),
*/

#include <stm32f10x.h>

#define duty_tim								TIM2
#define duty_tim_APBxClock_CMD				RCC_APB1PeriphClockCmd
#define duty_CLK								RCC_APB1Periph_TIM2

#define duty_PERTOD						(10000-1)
#define duty_PSC							(400-1)



#define duty_GPIO_CLK					RCC_APB2Periph_GPIOA
#define duty_PORT						GPIOA
#define duty_PIN						GPIO_Pin_1

#define	duty_IRQn					TIM2_IRQn
#define	duty_IRQHandler			TIM2_IRQHandler

void duty_Init(void);



#endif
