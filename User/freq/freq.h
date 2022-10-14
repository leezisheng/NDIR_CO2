/**
  ************************************************************************************************************************
  * File Name          : freq.h
  * Description        : ���ͷ�ļ��ǹ��ڶ�ʱ������PWM�Ͷ�ʱ���жϵĺ궨��
  ************************************************************************************************************************
  ���ͷ�ļ��ǹ��ڶ�ʱ������PWM�Ͷ�ʱ���жϵĺ궨��
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FREQ_H__
#define __FREQ_H__

/*
	���ļ�ֻ�������ɸ�Ƶ(Ƶ��4700Hz��ռ�ձ�50%),
*/

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>

/* ʱ��������ú궨�� ----------------------------------------------------------*/

// TIM3ʱ�Ӻ궨��
#define ctrl_tim_APBxClock_CMD				RCC_APB1PeriphClockCmd
#define ctrl_CLK							RCC_APB1Periph_TIM3

// ����PWM�����ĺ궨��
#define ctrl_PERTOD						    (2000-1)
#define ctrl_PSC							(4000-1)
#define ctrl_CCR1							(1000)

// TIM2��ʱ���жϵĺ궨��
#define adc_pwm_ccr1					 	950
#define adc_pwm_ccr2					 	980
#define adc_pwm_ccr3					 	1950
#define adc_pwm_ccr4					 	1980

// TIM3���PWM�����ź궨��
#define ctrl_GPIO_CLK						RCC_APB2Periph_GPIOA
#define ctrl_PORT							GPIOA
#define ctrl_PIN							GPIO_Pin_6

/* Function declaration--------------------------------------------------------*/
// ��ʼ����Դ���ƺ���
void ctrl_freq_init(void);
// ��ʼ����ʱ���жϺ���
void ctrl_pwm_init(void);

#endif /* __FREQ_H__ */
 


