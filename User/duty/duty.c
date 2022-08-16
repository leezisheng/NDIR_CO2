#include "duty.h"
u8 flag = 0;
u32 capture;
static void duty_GPIO_Config(void)
{

	GPIO_InitTypeDef duty_GPIO_InitStructure ;
	RCC_APB2PeriphClockCmd(duty_GPIO_CLK,ENABLE);
	duty_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	duty_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	duty_GPIO_InitStructure.GPIO_Pin =duty_PIN;

	GPIO_Init(duty_PORT,&duty_GPIO_InitStructure);
}



static void duty_MODE_Config(void)
{
	TIM_TimeBaseInitTypeDef duty_BASE_InitStructure ;
	
	duty_tim_APBxClock_CMD(duty_CLK ,ENABLE);
	
	duty_BASE_InitStructure.TIM_Period = duty_PERTOD ;
	duty_BASE_InitStructure.TIM_Prescaler = duty_PSC ;
	duty_BASE_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	duty_BASE_InitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
    duty_BASE_InitStructure.TIM_RepetitionCounter =0;
	TIM_TimeBaseInit(duty_tim,&duty_BASE_InitStructure);
	
	TIM_OCInitTypeDef duty_OC_InitStructure ;
	
	duty_OC_InitStructure.TIM_OCMode =TIM_OCMode_Toggle ;
	duty_OC_InitStructure.TIM_OutputState =TIM_OutputState_Enable ;
	duty_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	duty_OC_InitStructure.TIM_Pulse = 9780 ;
	duty_OC_InitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	
	TIM_OC2Init(duty_tim,&duty_OC_InitStructure);
	TIM_OC2PreloadConfig(duty_tim,TIM_OCPreload_Disable);		

	TIM_ITConfig(duty_tim,TIM_IT_CC2,ENABLE);
	
	duty_OC_InitStructure.TIM_Pulse = 9840 ;
	TIM_OC1Init(duty_tim,&duty_OC_InitStructure);
	TIM_OC1PreloadConfig(duty_tim,TIM_OCPreload_Disable);		
	TIM_ITConfig(duty_tim,TIM_IT_CC1,ENABLE);
	
	TIM_Cmd(duty_tim,ENABLE);	
}


static void duty_NVIC_Config(void)
{
	NVIC_InitTypeDef duty_Structure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	duty_Structure.NVIC_IRQChannel = duty_IRQn ;
	duty_Structure.NVIC_IRQChannelPreemptionPriority = 0;
	duty_Structure.NVIC_IRQChannelSubPriority = 3;
	duty_Structure.NVIC_IRQChannelCmd =ENABLE ;

	NVIC_Init(&duty_Structure);
}

void duty_Init(void)
{
	duty_GPIO_Config();
	duty_NVIC_Config();
	duty_MODE_Config();
}


void duty_IRQHandler(void)
{
	if(TIM_GetITStatus(duty_tim,TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(duty_tim,TIM_IT_CC2); 
		 capture = TIM_GetCapture2(duty_tim);
		if(capture == 9780)
		{
			TIM_SetCompare2(duty_tim,9810);
		}
		if(capture == 9810)
		{
			TIM_SetCompare2(duty_tim,9780);
		}
	}
	if(TIM_GetITStatus(duty_tim,TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(duty_tim,TIM_IT_CC1); 
		 capture = TIM_GetCapture1(duty_tim);
		if(capture == 9840)
		{
			TIM_SetCompare1(duty_tim,9920);
			flag = 1;
		}
		if(capture == 9920)
		{
			TIM_SetCompare1(duty_tim,9840);
			flag = 0;
		}
	}

}










