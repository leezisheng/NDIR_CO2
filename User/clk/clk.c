#include "clk.h"
#include "stm32f10x_rcc.h"
#include "uart.h"


void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUPStatus = 0 ;
	// 把RCC外设初始化成复位状态，这句是必须的
	RCC_DeInit();
	//1、开启HSI ，并等待 HSI 稳定
	RCC_HSICmd(ENABLE);
	//等待HSI稳定
	HSIStartUPStatus = RCC->CR & RCC_CR_HSIRDY ;
	if(HSIStartUPStatus == RCC_CR_HSIRDY)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_0);
		//2、设置 AHB、APB2、APB1的预分频因子
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		//3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2,pllmul);
		//4、开启PLL，并等待PLL稳定
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{}
		//5、把PLLCK切换为系统时钟SYSCLK
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
		while(RCC_GetSYSCLKSource() != 0x08)
		{}
	}
	else
	{
		printf("\r\n ----时钟初始化失败----\r\n");
	}

}



