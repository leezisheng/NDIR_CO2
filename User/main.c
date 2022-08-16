/*
	作者：刘灿
	代码：双通道热释电红外二氧化碳传感器拟合算法程序
	版本：1.0
 
  详解： 本程序使用内部时钟8M，可以节省电路部件所占空间
	细分为：
				1、光源驱动代码，生成1Hz的PWM波，调制光源亮灭。
				2、偏置电压输出代码，输出合理的DAC信号，抵消探测器输出的偏置电压。
				3、温湿度采集代码，温湿度传感器为SHT20（IIC通讯）可输出温度和湿度。
				4、探测器信号采集代码，是用单片机片上外设ADC采集处理后的探测器信号。
				5、浓度数据输出代码，上位机输入不同代码可以控制该系统输出不同成分数据。
	程序结构：
				首先系统时钟初始化，各个功能模块初始化，光源驱动代码输出PWM波控制光源亮灭；
				其次探测器信号采集代码并采集光源不工作时的处理后的探测器输出值，并调节偏置
电压输出合理的DAC电压值，直至ADC采集电压为零；
				再次上位机输入调试代码，采用DMA直接访问寄存器处理数据，并通过数据解析，分析
代码成分，调整成分数值输出；
*/ 
#include <stm32f10x.h>
#include "freq.h"
#include "uart.h"
#include "clk.h"
#include "iic.h"
#include "sht20.h"
#include "adc.h"
#include "math.h"


extern uint8_t Top_Flag ;
extern uint8_t Valley_Flag ;
extern __IO uint16_t ADC_GetValue[2];
extern uint8_t Data_flag;
extern float temperatureC;
extern float humidityRH;

__IO uint16_t average_value[2] ;
__IO uint16_t Vpp_Value[2];	
uint8_t Concentration_value[10];
__IO uint16_t average_ACT_value[240] ;
__IO uint16_t average_REF_value[240] ;
uint8_t Order_Flag ;
static u8  fac_us=0;

void delay_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	
	fac_us=SystemCoreClock/8000000;		
}
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 			  		 
	SysTick->VAL=0x00;        					
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		 
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	
	SysTick->VAL =0X00;      					 	 
}
void COUNTER_1S(uint16_t delay_1s)
{
	uint16_t delay_er, counter;
	for(counter = 0;counter < delay_1s;counter++)
	{
		delay_us(3000000);
		delay_er = delay_1s - counter;
		printf("Source of Stable Time:%d\r\n", delay_er);
	}
}
/*************平均值滤波**********************/
void Get_Aver(uint16_t time,__IO uint16_t *Get_Value)
{
	uint32_t initial_ACT_value =0,initial_REF_value =0 ;
	uint16_t t;
	uint16_t ACT_temp,REF_temp;
	for(t = 0 ; t < time; t++)
	{
		ACT_temp = Get_Value[0];
		average_ACT_value[t] = Get_Value[0];
		REF_temp = Get_Value[1];
		average_REF_value[t] = Get_Value[1];
		initial_ACT_value += ACT_temp;
		initial_REF_value += REF_temp;
		delay_us(60);
	}
		average_value[0] = initial_ACT_value / time;
		average_value[1] = initial_REF_value / time;
}
float kalman_filter(float inData)
{
	static float prevData = 0 ;
	static float p=7200,q=0.001,r = 0.03,KGain = 0;
	p=p+q;
	KGain = p/(p+r);
	inData = prevData + (KGain * (inData-prevData));
	p=(1-KGain)*p;
	prevData = inData;
	return inData ;
}
uint16_t peak_peak_filter(uint16_t nData)
{
	static uint16_t prevData = 0 ;
	uint16_t NewData;
	int16_t Diff_value;
	
	NewData = nData;
	Diff_value =  NewData - prevData;
	if((Diff_value > 10)&&(Diff_value < -10))
	{
		NewData = prevData + (Diff_value/10);
	}
	prevData = NewData;
	return prevData;
}
/****************最终的浓度计算***************************/
float Ppm_Value(float ACT_value,float REF_value)
{
	float Concentration ;
	uint16_t ratio;
	ratio = REF_value-ACT_value;
	Concentration = 0;
	
	printf("A%d\r\n",ratio);	

	if((temperatureC < 1.5)&&(temperatureC > -1.5))
	{
		if(ratio >= 435.5)
		{Concentration = ratio*(-93)+50892;}
		else if((ratio >= 398)&(ratio < 435.5))
		{Concentration = ratio*(-266)+126133;}
		else if((ratio >= 387)&(ratio < 398))
		{Concentration = ratio*(-463)+204527;}
		else if((ratio >= 376)&(ratio < 387))
		{Concentration = ratio*(-445)+197491;}	
		else if((ratio >= 361)&(ratio < 376))
		{Concentration = ratio*(-645)+272581;}
		else
		{Concentration = ratio*(-909)+367727;}
	}
	else if((temperatureC < 6.5)&&(temperatureC > 3.5))
	{
		if(ratio > 420)
		{Concentration = ratio*ratio*0.8587-978.1*ratio + 278800;}
		else
		{Concentration = ratio*ratio*7.022-6101*ratio + 1344000;}
	}
	else if((temperatureC < 11.5)&&(temperatureC > 8.5))
	{
		if(ratio > 423)
		{Concentration = ratio*ratio*0.766-891.1*ratio + 259400;}
		else
		{Concentration = ratio*ratio*5.028-4522*ratio + 1033000;}
	}
		else if((temperatureC < 16.5)&&(temperatureC > 13.5))
	{
		if(ratio > 422)
		{Concentration = ratio*ratio*0.7457-868.6*ratio + 253200;}
		else
		{Concentration = ratio*ratio*5.252-4668*ratio + 1055000;}
	}
	else if((temperatureC < 21.5)&&(temperatureC > 18.5))
	{
		if(ratio > 425)
		{Concentration = ratio*ratio*0.7508-881.2*ratio + 258700;}
		else
		{Concentration = ratio*ratio*6.024-5321*ratio + 1194000;}
	}
	else if((temperatureC < 26.5)&&(temperatureC > 23.5))
	{
		if(ratio >= 455.5)
		{Concentration = ratio*(-89)+50489;}
		else if((ratio >= 413.5)&(ratio < 455.5))
		{Concentration = ratio*(-238)+118452;}
		else if((ratio >= 399)&(ratio < 413.5))
		{Concentration = ratio*(-351)+165438;}
		else if((ratio >= 384.5)&(ratio < 399))
		{Concentration = ratio*(-337)+159934;}	
		else if((ratio >= 367)&(ratio < 384.5))
		{Concentration = ratio*(-588)+256176;}
		else
		{Concentration = ratio*(-769)+322692;}
		
	}
	else if((temperatureC < 31.5)&&(temperatureC > 28.5))
	{
		if(ratio > 429)
		{Concentration = ratio*ratio*0.656-785.9*ratio + 235700;}
		else
		{Concentration = ratio*ratio*3.93-3666*ratio + 869400;}
	}
	else if((temperatureC < 36.5)&&(temperatureC > 33.5))
	{
		if(ratio > 431)
		{Concentration = ratio*ratio*0.7172-853.2*ratio + 254000;}
		else
		{Concentration = ratio*ratio*4.479-4056*ratio + 936500;}
	}		
	else if((temperatureC < 41.5)&&(temperatureC > 38.5))
	{
		if(ratio >= 452)
		{Concentration = ratio*(-89)+50357;}
		else if((ratio >= 407)&(ratio < 452))
		{Concentration = ratio*(-222)+110444;}
		else if((ratio >= 391)&(ratio < 407))
		{Concentration = ratio*(-318)+149731;}
		else if((ratio >= 379)&(ratio < 391))
		{Concentration = ratio*(-408)+184758;}	
		else if((ratio >= 361.5)&(ratio < 379))
		{Concentration = ratio*(-571)+246571;}
		else
		{Concentration = ratio*(-689)+289310;}
	}	
	else
	{
		ratio = ratio-(-8.085+0.003332*ratio+0.3125*temperatureC);
		if(ratio > 424)
		{Concentration = ratio*ratio*0.7542-882.2*ratio + 258200;
		Concentration=Concentration*0.93;}
		else
		{Concentration = ratio*ratio*4.968-4463*ratio + 1020000;
		Concentration=Concentration*0.8;}
		Concentration=Concentration*0.93;
	}	
	if(Concentration > 53000)
	{
		Concentration = Concentration-3000;
	}
	else if(Concentration < -1000)
	{
		Concentration = Concentration+500;
	}
	return Concentration ;
}


int main()
{
	float ACT_Value_Work ,REF_Value_Work ;
	float ACT_Value_Down ,REF_Value_Down ;
	float Vpp_ACT,Vpp_REF;
	uint16_t Concentration ;
	
	HSI_SetSysClock(RCC_PLLMul_2);
	ctrl_freq_init();
	delay_init();
	USART_Config();
	COUNTER_1S(360);

	ADCx_Init();
	I2C_Configuration();
	SHT2X_Init();

	while(1)
	{
//		printf("ACT_GetValue：%d\r\n",ADC_GetValue[0]);
//		printf("REF_GetValue：%d\r\n",ADC_GetValue[1]);
		if(Valley_Flag == 1)
		{
			Order_Flag = 1 ;
			Get_Aver(240,ADC_GetValue);
			ACT_Value_Down = average_value[0];
			REF_Value_Down = average_value[1];
//			printf("ACT_Value_Down：%f\r\n",ACT_Value_Down);
//			printf("REF_Value_Down：%f\r\n",REF_Value_Down);			
			SHT2X_TEST();
//			printf("ACT_Value_Down：%d\r\n",ADC_GetValue[0]);
//  		printf("REF_Value_Down：%d\r\n",ADC_GetValue[1]);	
		}
		if((Top_Flag == 1)&&(Order_Flag == 1))
		{
//			printf("ACT_Value_Work：%d\r\n",ADC_GetValue[0]);
//			printf("REF_Value_Work：%d\r\n",ADC_GetValue[1]);
			Get_Aver(240,ADC_GetValue);
			ACT_Value_Work = average_value[0];
			REF_Value_Work = average_value[1];

			Vpp_ACT = ACT_Value_Work - ACT_Value_Down ;
			Vpp_REF = REF_Value_Work - REF_Value_Down ;

			Concentration =(uint16_t)kalman_filter( Ppm_Value(Vpp_ACT,Vpp_REF));
			Concentration=peak_peak_filter(Concentration / 100);
  		printf("C%d\r\n",Concentration);			
		}

		/******根据数据解析结果，输出不同成分*******/
//		switch(Data_flag)
//		{	
//			case 0x00:
//							printf("Concentration：%f\r\n",Concentration);
//							break;
//			case 0x01:
//							printf("temperature：%f\r\n",temperatureC);
//							break;
//			case 0x02:
//							printf("humidity：%f%%\r\n",humidityRH);
//							break;
//			case 0x03:
//							printf("ACT_GET_Value：%f\r\n",Vpp_ACT);
//							printf("REF_GET_Value：%f\r\n",Vpp_REF);
//							break;
//			case 0x04:
//							printf("error\r\n");
//							break;
//			default:
//							break ;
//		}

	}
}




