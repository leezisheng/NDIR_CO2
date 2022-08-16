#include "power.h"

static void power_GPIO_Config(void)
{
	GPIO_InitTypeDef power_InitStructure ;
	
	RCC_APB2PeriphClockCmd(power_CH_GPIO_CLK,ENABLE);
	
	power_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
	power_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	power_InitStructure.GPIO_Pin = power_CH1_PIN| power_CH2_PIN;
	GPIO_Init(power_CH_PORT,&power_InitStructure);
}


static void power_MODE_Config(void)
{
	TIM_TimeBaseInitTypeDef power_BASE_InitStructure ;
	
	power_APBxClock_CMD(power_CLK ,ENABLE);
	
	power_BASE_InitStructure.TIM_Period = power_PERTOD ;
	power_BASE_InitStructure.TIM_Prescaler = power_PSC ;
	power_BASE_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	power_BASE_InitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	power_BASE_InitStructure.TIM_RepetitionCounter = 0 ;
	TIM_TimeBaseInit(power_tim,&power_BASE_InitStructure);

	
	TIM_OCInitTypeDef power_OC_InitStructure ;
	
	power_OC_InitStructure.TIM_OCMode =TIM_OCMode_PWM1 ;
	power_OC_InitStructure.TIM_OutputState =TIM_OutputState_Enable ;
	power_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	power_OC_InitStructure.TIM_Pulse = power_CCR1 ;//²Ù×÷CCR¼Ä´æÆ÷
	
	TIM_OC1Init(power_tim,&power_OC_InitStructure);
	TIM_OC1PreloadConfig(power_tim,TIM_OCPreload_Enable);	

	power_OC_InitStructure.TIM_Pulse = power_CCR2 ;
	
	TIM_OC2Init(power_tim,&power_OC_InitStructure);
	TIM_OC2PreloadConfig(power_tim,TIM_OCPreload_Enable);
	TIM_Cmd(power_tim,ENABLE);
	TIM_CtrlPWMOutputs(power_tim,ENABLE);
	
}


void power_init(void)
{
	power_GPIO_Config();
	power_MODE_Config();
}





