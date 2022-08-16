#include "rever.h"

static void rever_GPIO_Config(void)
{
	GPIO_InitTypeDef rever_InitStructure ;
	
	RCC_APB2PeriphClockCmd(rever_CH_GPIO_CLK,ENABLE);
	
	rever_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
	rever_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	rever_InitStructure.GPIO_Pin = rever_CH1_PIN;
	GPIO_Init(rever_CH_PORT,&rever_InitStructure);
}


static void rever_MODE_Config(void)
{
	TIM_TimeBaseInitTypeDef rever_BASE_InitStructure ;
	
	rever_APBxClock_CMD(rever_CLK ,ENABLE);
	
	rever_BASE_InitStructure.TIM_Period = rever_PERTOD ;
	rever_BASE_InitStructure.TIM_Prescaler = rever_PSC ;
	rever_BASE_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	rever_BASE_InitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	rever_BASE_InitStructure.TIM_RepetitionCounter = 0 ;
	TIM_TimeBaseInit(rever_tim,&rever_BASE_InitStructure);

	
	TIM_OCInitTypeDef rever_OC_InitStructure ;
	
	rever_OC_InitStructure.TIM_OCMode =TIM_OCMode_PWM1 ;
	rever_OC_InitStructure.TIM_OutputState =TIM_OutputState_Enable ;
	rever_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	rever_OC_InitStructure.TIM_Pulse = rever_CCR1 ;//²Ù×÷CCR¼Ä´æÆ÷
	
	TIM_OC1Init(rever_tim,&rever_OC_InitStructure);
	TIM_OC1PreloadConfig(rever_tim,TIM_OCPreload_Enable);	

	TIM_Cmd(rever_tim,ENABLE);
	TIM_CtrlPWMOutputs(rever_tim,ENABLE);
	
}


void rever_init(void)
{
	rever_GPIO_Config();
	rever_MODE_Config();
}
