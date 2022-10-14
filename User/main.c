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
	���ߣ�����
	���룺˫ͨ�����͵���������̼����������㷨����
	�汾��1.0
 
    ��⣺ ������ʹ���ڲ�ʱ��8M�����Խ�ʡ��·������ռ�ռ�
	ϸ��Ϊ��
				1����Դ�������룬����1Hz��PWM�������ƹ�Դ����
				2��ƫ�õ�ѹ������룬��������DAC�źţ�����̽���������ƫ�õ�ѹ��
				3����ʪ�Ȳɼ����룬��ʪ�ȴ�����ΪSHT20��IICͨѶ��������¶Ⱥ�ʪ�ȡ�
				4��̽�����źŲɼ����룬���õ�Ƭ��Ƭ������ADC�ɼ�������̽�����źš�
				5��Ũ������������룬��λ�����벻ͬ������Կ��Ƹ�ϵͳ�����ͬ�ɷ����ݡ�
	����ṹ��
				����ϵͳʱ�ӳ�ʼ������������ģ���ʼ������Դ�����������PWM�����ƹ�Դ����
				���̽�����źŲɼ����벢�ɼ���Դ������ʱ�Ĵ�����̽�������ֵ��������ƫ��
                ��ѹ��������DAC��ѹֵ��ֱ��ADC�ɼ���ѹΪ�㣻
				�ٴ���λ��������Դ��룬����DMAֱ�ӷ��ʼĴ����������ݣ���ͨ�����ݽ���������
                ����ɷ֣������ɷ���ֵ�����
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
// ��־�����Ƿ�Ϊ��ѹ��ֵ
extern uint8_t Top_Flag ;
// ��־�����Ƿ�Ϊ��ѹ��ֵ
extern uint8_t Valley_Flag ;
// ADC�ɼ����飺ADC_REF��ADC_ACT
extern __IO uint16_t ADC_GetValue[2];
// ���ݱ�־λ��
// 1 : ������Ӧʱ��ɼ���ѹֵ������Ũ��
// 2 : ֱ�������ѹֵ
extern uint8_t Data_flag;
// �¶�ֵ
extern float temperatureC;
// ʪ��ֵ
extern float humidityRH;

/* Global variables -----------------------------------------------------------------------*/
// ��ֵ�˲����ADC��ѹֵ�Ͳο���ѹֵ
__IO uint16_t average_value[2] ;
__IO uint16_t Vpp_Value[2];	

uint8_t Concentration_value[10];
// ���240��ADC��ѹֵ������
__IO uint16_t average_ACT_value[240] ;
// ���240�βο���ѹֵ������
__IO uint16_t average_REF_value[240] ;
// ��־λ
uint8_t Order_Flag ;

#define average_count_num 20
// Ũ���˲�����
uint16_t ppm_value_count = 0;
// Ũ������˲���־λ
uint8_t Average_Flag;
// ���ԭʼŨ��ֵ������
uint32_t average_Ppm_Value[average_count_num];

// ��̬ϵ��
uint32_t Motor_count = 0;

/* Static variables -----------------------------------------------------------------------*/
// Systick������ֵ
static u8  fac_us=0;

/* Function declaration--------------------------------------------------------------------*/
// ��ʱ������ʼ��
void delay_init(void);
// us������ʱ���� 
void delay_us(u32 nus);
// s������ʱ����
void COUNTER_1S(uint16_t delay_1s);
// ��ֵ�˲�
void Get_Aver(uint16_t time,__IO uint16_t *Get_Value);
// �������˲�
float kalman_filter(float inData);
// ��ֵ�˲�
int peak_peak_filter(uint32_t nData);
// Ũ�ȼ���
float Ppm_Value(float ACT_value,float REF_value);
// Ũ�������ֵ�˲�
float Ppm_Value_Average_Filter(uint32_t now_Ppm_Value);
// ����ƽ���˲�
uint32_t Smooth_filter(uint32_t Value);
// �����˲�
uint32_t Mode_filter(uint32_t Now_Data);

/* Function definition---------------------------------------------------------------------*/
int main()
{
	// ����ģʽѡ��
	Data_flag = 1;
	
	// �ֲ���������
	float ACT_Value_Work ,REF_Value_Work ;
	float ACT_Value_Down ,REF_Value_Down ;
	float Vpp_ACT,Vpp_REF;
	uint16_t Concentration ;
	
	// �ڲ�����ʱ�ӳ�ʼ��
	HSI_SetSysClock(RCC_PLLMul_2);
	
	// ��ʼ����PWM������� 
	ctrl_freq_init();

	if(Data_flag == 1)
	{
		// ��ʼ������ʱ������
		ctrl_pwm_init();
	}
	
	// ��ʱ��ʼ��
	delay_init();
	// ��������
	USART_Config();
	// �����ϵ���ʱ
	COUNTER_1S(360);

	// ADC��ʼ��
	ADCx_Init();
	// I2C��ʼ��
	I2C_Configuration();
	// ��ʪ�ȴ�������ʼ��
	SHT2X_Init();

	while(1)
	{
		// ������Ӧʱ��ɼ���ѹֵ������Ũ��
		if(Data_flag==1)
		{
			if(Valley_Flag == 1)
			{
				Order_Flag = 1 ;
				// ��ADC�ɼ���ѹֵ���о�ֵ�˲�
				Get_Aver(240,ADC_GetValue);
				ACT_Value_Down = average_value[0];
				REF_Value_Down = average_value[1];	
				// ������ʪ�ȴ�����
				SHT2X_TEST();
			}
			if((Top_Flag == 1)&&(Order_Flag == 1))
			{
				// ��ADC�ɼ���ѹֵ���о�ֵ�˲�
				Get_Aver(240,ADC_GetValue);
				ACT_Value_Work = average_value[0];
				REF_Value_Work = average_value[1];

				Vpp_ACT = ACT_Value_Work - ACT_Value_Down ;
				Vpp_REF = REF_Value_Work - REF_Value_Down ;
				
				Concentration =(uint32_t)Ppm_Value(Vpp_ACT,Vpp_REF)/100;
				
//				// �Է�ֵ��ѹ���п������˲�
//				Concentration =(uint16_t)kalman_filter( Ppm_Value(Vpp_ACT,Vpp_REF));
//				// �Է�ֵ��ѹ���з�ֵ�˲�
//				Concentration=peak_peak_filter(Concentration / 100);
				
				// ��ֵ�˲�
				Concentration = (uint32_t)(Ppm_Value_Average_Filter(Concentration));
				Concentration = (uint32_t)peak_peak_filter(Concentration);
				Concentration = (uint32_t)Smooth_filter(Concentration);
//				Concentration = (uint32_t)Mode_filter(Concentration);
				
				printf("C%d\r\n",Concentration);
				
//				printf("AC%d\r\n",(uint32_t)Vpp_ACT);
//				printf("AR%d\r\n",(uint32_t)Vpp_REF);		
			}
		}
		// ��ѹֱ�����ģʽ
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

/* ��ʱ�������-----------------------------------------------------------------------------*/

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

/* �˲��㷨���-----------------------------------------------------------------------------*/

/*************ƽ��ֵ�˲�**********************/
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

/****************�������˲�***************************/
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

/****************��ֵ�˲�***************************/
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

/****************ƽ������***************************/
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

/****************�����˲�***************************/
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

/****************���յ�Ũ�ȼ���***************************/
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
	
	// ����
	if(Concentration<4500)
	{
		Concentration = 100 + Concentration/15;
	}
	
	return Concentration ;
}

/****************Ũ�Ⱦ�ֵ�˲�***************************/
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
	
	// Ũ��ֵ����С��5
	if(Average_Flag == 0)
	{
		average_Ppm_Value[ppm_value_count] = now_Ppm_Value;
		ppm_value_count++;
		
		return now_Ppm_Value;
	}
	// Ũ��ֵ��������5
	else
	{
		// ȡ������ppm_value_count��0��average_count_num-1ѭ��
		ppm_value_count = ppm_value_count%average_count_num;
		//�������滻������
		average_Ppm_Value[ppm_value_count] = now_Ppm_Value;
		
		for(i = 0;i<average_count_num;i++)
		{
			Total_Ppm_Value = Total_Ppm_Value + average_Ppm_Value[i];
		}
		now_Ppm_Value = (uint32_t)Total_Ppm_Value/average_count_num;
		
		// ����������ٷŻ�ԭ��������
		average_Ppm_Value[ppm_value_count] = now_Ppm_Value;
		
		Total_Ppm_Value = 0;
		
		// ����
		ppm_value_count++;
			
	    return now_Ppm_Value;
	}
}


