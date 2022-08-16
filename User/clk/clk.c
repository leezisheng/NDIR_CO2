#include "clk.h"
#include "stm32f10x_rcc.h"
#include "uart.h"


void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUPStatus = 0 ;
	// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
	RCC_DeInit();
	//1������HSI �����ȴ� HSI �ȶ�
	RCC_HSICmd(ENABLE);
	//�ȴ�HSI�ȶ�
	HSIStartUPStatus = RCC->CR & RCC_CR_HSIRDY ;
	if(HSIStartUPStatus == RCC_CR_HSIRDY)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_0);
		//2������ AHB��APB2��APB1��Ԥ��Ƶ����
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		//3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2,pllmul);
		//4������PLL�����ȴ�PLL�ȶ�
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{}
		//5����PLLCK�л�Ϊϵͳʱ��SYSCLK
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
		while(RCC_GetSYSCLKSource() != 0x08)
		{}
	}
	else
	{
		printf("\r\n ----ʱ�ӳ�ʼ��ʧ��----\r\n");
	}

}



