/**
  ************************************************************************************************************************
  * File Name          : freq.c
  * Description        : The document includes:
  * 1.TIM3 generates a 2Hz PWM wave to control the periodic flicker of the light source;
  * 2.TIM2 timer interrupt configuration and interrupt processing function: after the ADC acquisition voltage reaches the peak value, 
  * it will mark the position and process the collected voltage value for filtering and concentration calculation
  ************************************************************************************************************************
  该文件包括：
  1.TIM3生成频率为2Hz的PWM波以控制光源周期性闪烁；
  2.TIM2定时器中断配置和中断处理函数：在ADC采集电压到达峰值后，将标志位置位，对采集的电压值进行处理进行滤波和浓度计算
  
*/

/* Includes ------------------------------------------------------------------*/
#include "freq.h"

/* Global variable------------------------------------------------------------*/
// ADC采集电压峰值标志量
uint8_t Top_Flag = 0;
// ADC采集电压谷值标志量
uint8_t Valley_Flag = 0;

/* Function definition--------------------------------------------------------*/

/* PWM输出引脚配置 ------------------------------------*/
static void ctrl_freq_GPIO_Config(void)
{
	
	GPIO_InitTypeDef ctrl_GPIO_InitStructure ;
	
	RCC_APB2PeriphClockCmd(ctrl_GPIO_CLK,ENABLE);
	ctrl_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	ctrl_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	ctrl_GPIO_InitStructure.GPIO_Pin = ctrl_PIN;
	
	// 引脚为PA6
	GPIO_Init(ctrl_PORT,&ctrl_GPIO_InitStructure);
}

/* PWM输出参数配置 ------------------------------------*/
static void ctrl_MODE_Config(void)
{
	// 时钟配置
	TIM_TimeBaseInitTypeDef ctrl_BASE_InitStructure ;
	
	ctrl_tim_APBxClock_CMD(ctrl_CLK ,ENABLE);
	
	ctrl_BASE_InitStructure.TIM_Period = ctrl_PERTOD ;
	ctrl_BASE_InitStructure.TIM_Prescaler = ctrl_PSC ;
	ctrl_BASE_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	ctrl_BASE_InitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	ctrl_BASE_InitStructure.TIM_RepetitionCounter =0;
	TIM_TimeBaseInit(TIM3,&ctrl_BASE_InitStructure);
	
	TIM_OCInitTypeDef ctrl_OC_InitStructure ;
	
	// 通道配置
	ctrl_OC_InitStructure.TIM_OCMode =TIM_OCMode_PWM1 ;
	ctrl_OC_InitStructure.TIM_OutputState =TIM_OutputState_Enable ;
	ctrl_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	ctrl_OC_InitStructure.TIM_Pulse = ctrl_CCR1 ;
	ctrl_OC_InitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OC1Init(TIM3,&ctrl_OC_InitStructure);
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);		
	
	TIM_Cmd(TIM3,ENABLE);	
}

/* TIM2参数配置 --------------------------------------*/
static void adc_Pwm_Config(void)
{
	TIM_TimeBaseInitTypeDef adc_Pwm_BASE_InitStructure ;
	
	ctrl_tim_APBxClock_CMD(RCC_APB1Periph_TIM2 ,ENABLE);
	
	adc_Pwm_BASE_InitStructure.TIM_Period = ctrl_PERTOD ;
	adc_Pwm_BASE_InitStructure.TIM_Prescaler = ctrl_PSC ;
	adc_Pwm_BASE_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	adc_Pwm_BASE_InitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
    adc_Pwm_BASE_InitStructure.TIM_RepetitionCounter =0;
	TIM_TimeBaseInit(TIM2,&adc_Pwm_BASE_InitStructure);
	
	TIM_OCInitTypeDef adc_Pwm_OC_InitStructure ;
	
	adc_Pwm_OC_InitStructure.TIM_OCMode =TIM_OCMode_PWM1 ;
	adc_Pwm_OC_InitStructure.TIM_OutputState =TIM_OutputState_Enable ;
	adc_Pwm_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	adc_Pwm_OC_InitStructure.TIM_Pulse = adc_pwm_ccr1 ;
	adc_Pwm_OC_InitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OC1Init(TIM2,&adc_Pwm_OC_InitStructure);
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);	
	
	adc_Pwm_OC_InitStructure.TIM_Pulse = adc_pwm_ccr2 ;
	TIM_OC2Init(TIM2,&adc_Pwm_OC_InitStructure);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);	
	
	adc_Pwm_OC_InitStructure.TIM_Pulse = adc_pwm_ccr3 ;
	TIM_OC3Init(TIM2,&adc_Pwm_OC_InitStructure);
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);	
	
	adc_Pwm_OC_InitStructure.TIM_Pulse = adc_pwm_ccr4 ;
	TIM_OC4Init(TIM2,&adc_Pwm_OC_InitStructure);
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);	
	TIM_ITConfig(TIM2,TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);
	
	TIM_Cmd(TIM2,ENABLE);	
}

/* TIM2中断参数配置 ------------------------------------*/
static void freq_NVIC_Config(void)
{
	NVIC_InitTypeDef ctrl_Structure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	ctrl_Structure.NVIC_IRQChannel = TIM2_IRQn ;
	ctrl_Structure.NVIC_IRQChannelPreemptionPriority = 0;
	ctrl_Structure.NVIC_IRQChannelSubPriority = 2;
	ctrl_Structure.NVIC_IRQChannelCmd =ENABLE ;
	NVIC_Init(&ctrl_Structure);
}

/* 初始化：PWM输出配置 -------------------*/
void ctrl_freq_init(void)
{
	ctrl_freq_GPIO_Config();
	ctrl_MODE_Config();
}	

/* 初始化：定时器配置 -------------------*/
void ctrl_pwm_init(void)
{
	freq_NVIC_Config();
	adc_Pwm_Config();
}

/* TIM2中断处理函数 -----------------------------------*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_CC1) != RESET)
	{
		Valley_Flag = 1 ;
		Top_Flag = 0;
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
	}
	if(TIM_GetITStatus(TIM2,TIM_IT_CC2) != RESET)
	{
		Valley_Flag = 0 ;
		Top_Flag = 0 ;
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);
	}
	// ADC采集到达峰值，开始采集
	if(TIM_GetITStatus(TIM2,TIM_IT_CC3) != RESET)
	{
		// 对电压峰值标志量进行置位
		Top_Flag = 1 ;
		Valley_Flag = 0 ;
		// 清除中断标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC3);
	}
	if(TIM_GetITStatus(TIM2,TIM_IT_CC4) != RESET)
	{
		Top_Flag = 0 ;
		Valley_Flag = 0 ;
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC4);
	}
}













