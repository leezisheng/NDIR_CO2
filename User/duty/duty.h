#ifndef __DUTY_H__
#define __DUTY_H__


/*
	���ļ�ֻ��������խ�ź�2Hz(Ƶ��2Hz��ռ�ձ�0.3%���빩���ź����4	ms����λ��),
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
