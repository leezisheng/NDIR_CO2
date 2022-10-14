/**
  ************************************************************************************************************************
  * File Name          : freq.h
  * Description        : 这个头文件是关于定时器生成PWM和定时器中断的宏定义
  ************************************************************************************************************************
  这个头文件是关于定时器生成PWM和定时器中断的宏定义
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FREQ_H__
#define __FREQ_H__

/*
	本文件只用于生成高频(频率4700Hz，占空比50%),
*/

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>

/* 时钟相关配置宏定义 ----------------------------------------------------------*/

// TIM3时钟宏定义
#define ctrl_tim_APBxClock_CMD				RCC_APB1PeriphClockCmd
#define ctrl_CLK							RCC_APB1Periph_TIM3

// 生成PWM参数的宏定义
#define ctrl_PERTOD						    (2000-1)
#define ctrl_PSC							(4000-1)
#define ctrl_CCR1							(1000)

// TIM2定时器中断的宏定义
#define adc_pwm_ccr1					 	950
#define adc_pwm_ccr2					 	980
#define adc_pwm_ccr3					 	1950
#define adc_pwm_ccr4					 	1980

// TIM3输出PWM的引脚宏定义
#define ctrl_GPIO_CLK						RCC_APB2Periph_GPIOA
#define ctrl_PORT							GPIOA
#define ctrl_PIN							GPIO_Pin_6

/* Function declaration--------------------------------------------------------*/
// 初始化光源控制函数
void ctrl_freq_init(void);
// 初始化定时器中断函数
void ctrl_pwm_init(void);

#endif /* __FREQ_H__ */
 


