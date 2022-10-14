/**
 *********************************************************************************************************************************************
 * File Name          : main.c
 * Description        : This file include:
 * 1. Initialization: Initializes the internal high-speed clock
 * 2. Initialization using peripherals: including PWM clock initialization, serial port initialization, 
 * ADC peripherals initialization, I2C initialization, serial port initialization
 * 3. Driver function initialization: light source driver initialization, bias power driver initialization, SHT2X initialization
 * 4. Data processing process: ADC collection and calibration, ADC collection and concentration conversion,
 * Temperature and humidity collection, serial port output data
 *********************************************************************************************************************************************
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
 *********************************************************************************************************************************************
**/ 

/* Includes ------------------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "main.h"
#include "freq.h"
#include "uart.h"
#include "clk.h"
#include "iic.h"
#include "sht20.h"
#include "adc.h"
#include "math.h"

/* External variables --------------------------------------------------------------------*/
// 标志量：是否为电压峰值
extern uint8_t Top_Flag ;
// 标志量：是否为电压谷值
extern uint8_t Valley_Flag ;
// ADC采集数组：ADC_REF和ADC_ACT
extern __IO uint16_t ADC_GetValue[2];
// 数据标志位：
// 1 : 根据响应时间采集电压值，计算浓度
// 2 : 直接输出电压值
extern uint8_t Data_flag;
// 温度值
extern float temperatureC;
// 湿度值
extern float humidityRH;

/* Global variables -----------------------------------------------------------------------*/
// 均值滤波后的ADC电压值和参考电压值
__IO uint16_t average_value[2] ;
__IO uint16_t Vpp_Value[2];	

uint8_t Concentration_value[10];
// 存放240次ADC电压值的数组
__IO uint16_t average_ACT_value[240] ;
// 存放240次参考电压值的数组
__IO uint16_t average_REF_value[240] ;
// 标志位
uint8_t Order_Flag ;

#define average_count_num 20
// 浓度滤波计数
uint16_t ppm_value_count = 0;
// 浓度输出滤波标志位
uint8_t Average_Flag;
// 存放原始浓度值的数组
uint32_t average_Ppm_Value[average_count_num];

// 动态系数
uint32_t Motor_count = 0;

/* Static variables -----------------------------------------------------------------------*/
// Systick计数器值
static u8  fac_us=0;

/* Function declaration--------------------------------------------------------------------*/
// 延时函数初始化
void delay_init(void);
// us级别延时函数 
void delay_us(u32 nus);
// s级别延时函数
void COUNTER_1S(uint16_t delay_1s);
// 均值滤波
void Get_Aver(uint16_t time,__IO uint16_t *Get_Value);
// 卡尔曼滤波
float kalman_filter(float inData);
// 峰值滤波
int peak_peak_filter(uint32_t nData);
// 浓度计算
float Ppm_Value(float ACT_value,float REF_value);
// 浓度输出均值滤波
float Ppm_Value_Average_Filter(uint32_t now_Ppm_Value);
// 两点平滑滤波
uint32_t Smooth_filter(uint32_t Value);
// 众数滤波
uint32_t Mode_filter(uint32_t Now_Data);

/* Function definition---------------------------------------------------------------------*/
int main()
{
	// 工作模式选择
	Data_flag = 1;
	
	// 局部变量声明
	float ACT_Value_Work ,REF_Value_Work ;
	float ACT_Value_Down ,REF_Value_Down ;
	float Vpp_ACT,Vpp_REF;
	uint16_t Concentration ;
	
	// 内部高速时钟初始化
	HSI_SetSysClock(RCC_PLLMul_2);
	
	// 初始化：PWM输出配置 
	ctrl_freq_init();

	if(Data_flag == 1)
	{
		// 初始化：定时器配置
		ctrl_pwm_init();
	}
	
	// 延时初始化
	delay_init();
	// 串口配置
	USART_Config();
	// 器件上电延时
	COUNTER_1S(360);

	// ADC初始化
	ADCx_Init();
	// I2C初始化
	I2C_Configuration();
	// 温湿度传感器初始化
	SHT2X_Init();

	while(1)
	{
		// 根据响应时间采集电压值，计算浓度
		if(Data_flag==1)
		{
			if(Valley_Flag == 1)
			{
				Order_Flag = 1 ;
				// 对ADC采集电压值进行均值滤波
				Get_Aver(240,ADC_GetValue);
				ACT_Value_Down = average_value[0];
				REF_Value_Down = average_value[1];	
				// 测试温湿度传感器
				SHT2X_TEST();
			}
			if((Top_Flag == 1)&&(Order_Flag == 1))
			{
				// 对ADC采集电压值进行均值滤波
				Get_Aver(240,ADC_GetValue);
				ACT_Value_Work = average_value[0];
				REF_Value_Work = average_value[1];

				Vpp_ACT = ACT_Value_Work - ACT_Value_Down ;
				Vpp_REF = REF_Value_Work - REF_Value_Down ;
				
				Concentration =(uint32_t)Ppm_Value(Vpp_ACT,Vpp_REF)/100;
				
//				// 对峰值电压进行卡尔曼滤波
//				Concentration =(uint16_t)kalman_filter( Ppm_Value(Vpp_ACT,Vpp_REF));
//				// 对峰值电压进行峰值滤波
//				Concentration=peak_peak_filter(Concentration / 100);
				
				// 均值滤波
				Concentration = (uint32_t)(Ppm_Value_Average_Filter(Concentration));
				Concentration = (uint32_t)peak_peak_filter(Concentration);
				Concentration = (uint32_t)Smooth_filter(Concentration);
//				Concentration = (uint32_t)Mode_filter(Concentration);
				
				printf("C%d\r\n",Concentration);
				
//				printf("AC%d\r\n",(uint32_t)Vpp_ACT);
//				printf("AR%d\r\n",(uint32_t)Vpp_REF);		
			}
		}
		// 电压直接输出模式
		else
		{
			ACT_Value_Down = average_value[0];
			REF_Value_Down = average_value[1];	
			printf("A%d\r\n",(uint16_t)(REF_Value_Down - ACT_Value_Down));
			printf("AC%d\r\n",(uint32_t)Vpp_ACT);
			printf("AR%d\r\n",(uint32_t)Vpp_REF);
		}
	}
}

/* 延时函数相关-----------------------------------------------------------------------------*/

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

/* 滤波算法相关-----------------------------------------------------------------------------*/

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

/****************卡尔曼滤波***************************/
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

/****************峰值滤波***************************/
int peak_peak_filter(uint32_t nData)
{
	static int prevData = 0 ;
	int NewData;
	int Diff_value;
	
	NewData = nData;
	Diff_value = (int)(NewData - prevData);
	if((Diff_value > 25)&&(Diff_value < -25))
	{
		NewData = prevData + (Diff_value/20);
	}
	prevData = NewData;
	return prevData;
}

/****************平滑处理***************************/
uint32_t Smooth_filter(uint32_t Value)
{
	static uint32_t Last_Data = 0 ;
	uint32_t Now_Data;
	uint32_t Total_Data = 0;
	
	Now_Data = Value;
	Total_Data = (int)(Now_Data + Last_Data);
	
	Last_Data = Now_Data;
	
	Now_Data  = (uint32_t)Total_Data/2;
	

	return Now_Data;
}

/****************众数滤波***************************/
uint32_t Mode_filter(uint32_t Now_Data)
{
	#ifdef  SENSOR_12
	
	if((temperatureC<29)&&(temperatureC>25))
	{
		if(Now_Data>330)
		{
			Now_Data = Now_Data+60;
		}
		
	}
	
	
	#endif

	return Now_Data;
}

/****************最终的浓度计算***************************/
float Ppm_Value(float ACT_value,float REF_value)
{
	float Concentration ;
	uint32_t y;
	uint32_t x = (uint32_t)temperatureC;
	y = (uint32_t)REF_value-(uint32_t)ACT_value;
	Concentration = 0;
	
	printf("A%d\r\n",y);	
	
	Concentration = 
	    (x*x_coef + y*y_coef )
      + (x*x*x2_coef + y*y*y2_coef + x*y*xy_coef ) 
	  + intercept;
	
	if(Concentration<0)
	{
		Concentration = -Concentration;
	}
	
	// 调零
	if(Concentration<4500)
	{
		Concentration = 100 + Concentration/15;
	}
	
	return Concentration ;
}

/****************浓度均值滤波***************************/
float Ppm_Value_Average_Filter(uint32_t now_Ppm_Value)
{
	uint32_t Total_Ppm_Value = 0;
	int i = 0;
	
	if(ppm_value_count<(average_count_num-1))
	{   
		Average_Flag = 0;
	}
	else
	{
		Average_Flag = 1;
	}
	
	// 浓度值个数小于5
	if(Average_Flag == 0)
	{
		average_Ppm_Value[ppm_value_count] = now_Ppm_Value;
		ppm_value_count++;
		
		return now_Ppm_Value;
	}
	// 浓度值个数等于5
	else
	{
		// 取余数，ppm_value_count从0到average_count_num-1循环
		ppm_value_count = ppm_value_count%average_count_num;
		//新数据替换旧数据
		average_Ppm_Value[ppm_value_count] = now_Ppm_Value;
		
		for(i = 0;i<average_count_num;i++)
		{
			Total_Ppm_Value = Total_Ppm_Value + average_Ppm_Value[i];
		}
		now_Ppm_Value = (uint32_t)Total_Ppm_Value/average_count_num;
		
		// 计算后数据再放回原来数组中
		average_Ppm_Value[ppm_value_count] = now_Ppm_Value;
		
		Total_Ppm_Value = 0;
		
		// 递增
		ppm_value_count++;
			
	    return now_Ppm_Value;
	}
}


