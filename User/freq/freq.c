#include "freq.h"

uint8_t Top_Flag = 0;
uint8_t Valley_Flag = 0;

static void ctrl_freq_GPIO_Config(void)
{
	
	GPIO_InitTypeDef ctrl_GPIO_InitStructure ;
	
	RCC_APB2PeriphClockCmd(ctrl_GPIO_CLK,ENABLE);
	ctrl_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	ctrl_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	ctrl_GPIO_InitStructure.GPIO_Pin = ctrl_PIN;

	GPIO_Init(ctrl_PORT,&ctrl_GPIO_InitStructure);
}

static void ctrl_MODE_Config(void)
{
	TIM_TimeBaseInitTypeDef ctrl_BASE_InitStructure ;
	
	ctrl_tim_APBxClock_CMD(ctrl_CLK ,ENABLE);
	
	ctrl_BASE_InitStructure.TIM_Period = ctrl_PERTOD ;
	ctrl_BASE_InitStructure.TIM_Prescaler = ctrl_PSC ;
	ctrl_BASE_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	ctrl_BASE_InitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
  ctrl_BASE_InitStructure.TIM_RepetitionCounter =0;
	TIM_TimeBaseInit(TIM3,&ctrl_BASE_InitStructure);
	
	TIM_OCInitTypeDef ctrl_OC_InitStructure ;
	
	ctrl_OC_InitStructure.TIM_OCMode =TIM_OCMode_PWM1 ;
	ctrl_OC_InitStructure.TIM_OutputState =TIM_OutputState_Enable ;
	ctrl_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	ctrl_OC_InitStructure.TIM_Pulse = ctrl_CCR1 ;
	ctrl_OC_InitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OC1Init(TIM3,&ctrl_OC_InitStructure);
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);		
	
	TIM_Cmd(TIM3,ENABLE);	
}
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


void ctrl_freq_init(void)
{
	ctrl_freq_GPIO_Config();
	ctrl_MODE_Config();
	freq_NVIC_Config();
	adc_Pwm_Config();
}	


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
	if(TIM_GetITStatus(TIM2,TIM_IT_CC3) != RESET)
	{
		Top_Flag = 1 ;
		Valley_Flag = 0 ;
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC3);
	}
	if(TIM_GetITStatus(TIM2,TIM_IT_CC4) != RESET)
	{
		Top_Flag = 0 ;
		Valley_Flag = 0 ;
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC4);
	}
}













