#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define    adc_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    adcx                           ADC1
#define    adc_CLK                        RCC_APB2Periph_ADC1

#define 	adc_DMA_AHBClock_FUN			RCC_AHBPeriphClockCmd
#define 	adc_DMA_CLK								RCC_AHBPeriph_DMA1
#define	adc_DMA_CHANNEL							DMA1_Channel1

#define    adc_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    adc_GPIO_CLK                   RCC_APB2Periph_GPIOA  
#define    adc_PORT                     	GPIOA
	
#define    adc_PIN_1                       GPIO_Pin_1
#define    adc_PIN_2                       GPIO_Pin_2

#define    adc_CHANNEL_1                	   ADC_Channel_1
#define    adc_CHANNEL_2                	   ADC_Channel_2

#define 	NOFCHANEL							 2

void ADCx_Init(void);

#endif /* __ADC_H */


