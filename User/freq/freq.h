#ifndef __FREQ_H__
#define __FREQ_H__

/*
	本文件只用于生成高频(频率4700Hz，占空比50%),
*/

#include <stm32f10x.h>

#define ctrl_tim_APBxClock_CMD				RCC_APB1PeriphClockCmd
#define ctrl_CLK								RCC_APB1Periph_TIM3

#define ctrl_PERTOD						    (2000-1)
#define ctrl_PSC							(4000-1)
#define ctrl_CCR1							(1000)

#define adc_pwm_ccr1					 950
#define adc_pwm_ccr2					 980
#define adc_pwm_ccr3					 1950
#define adc_pwm_ccr4					 1980

#define ctrl_GPIO_CLK				RCC_APB2Periph_GPIOA
#define ctrl_PORT						GPIOA
#define ctrl_PIN						GPIO_Pin_6


void ctrl_freq_init(void);


#endif
 


