#include "adc.h"
#include "clk.h"

__IO uint16_t ADC_GetValue[2];


/*------------------------GPIO配置-----------------------------------*/		
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	adc_GPIO_APBxClock_FUN ( adc_GPIO_CLK, ENABLE );

	GPIO_InitStructure.GPIO_Pin = adc_PIN_1|adc_PIN_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_Init(adc_PORT, &GPIO_InitStructure);				
}

static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;	
	
	adc_DMA_AHBClock_FUN(adc_DMA_CLK,ENABLE);
	adc_APBxClock_FUN ( adc_CLK, ENABLE );
	
/*------------------------DMA配置-----------------------------------*/	
	DMA_DeInit(adc_DMA_CHANNEL);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(adcx->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_GetValue ;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;
	DMA_InitStructure.DMA_BufferSize = NOFCHANEL;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable ;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(adc_DMA_CHANNEL,&DMA_InitStructure);
	DMA_Cmd(adc_DMA_CHANNEL,ENABLE);
/*------------------------ADC配置-----------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE	; 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
	ADC_Init(adcx, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div2); 
	ADC_RegularChannelConfig(adcx, adc_CHANNEL_1, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(adcx, adc_CHANNEL_2, 2, ADC_SampleTime_239Cycles5);
/*------------------------建立联系-----------------------------------*/
	ADC_Cmd(adcx, ENABLE);
	ADC_ResetCalibration(adcx);
	while(ADC_GetResetCalibrationStatus(adcx));
	ADC_StartCalibration(adcx);
	while(ADC_GetCalibrationStatus(adcx)); 
	ADC_SoftwareStartConvCmd(adcx, ENABLE);
	ADC_DMACmd(adcx,ENABLE);

}

void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();

}


