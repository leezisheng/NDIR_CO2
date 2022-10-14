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
	
	ͷ�ļ���Ҫ�ǲ�ͬ�Ĵ���������
 *********************************************************************************************************************************************
**/ 

#ifndef __MAIN_H__
#define __MAIN_H__

/* ����ʽ-----------------------------------------------*/ 

#define SENSOR_12

#ifdef  SENSOR_1

#define 	x_coef 		(float)(4.65*100)
#define 	y_coef 		(float)(-1.82*1000)

#define 	x2_coef 	(float)(-4.73*1)
#define 	xy_coef		(float)(-5.45*0.1)
#define 	y2_coef		(float)(1.79*1)

#define     intercept   (461126)

#endif


#ifdef  SENSOR_6

#define 	x_coef 		(float)(1.18*10000)
#define 	y_coef 		(float)(-1.79*10000)

#define 	x2_coef 	(float)(-1.66*1)
#define 	xy_coef		(float)(-4.08*10)
#define 	y2_coef		(float)(3.33*10)

#define     intercept   (2416930)

#endif


#ifdef  SENSOR_7

#define 	x_coef 		(4289)
#define 	y_coef 		(-8524)

#define 	x2_coef 	(1.557)
#define 	xy_coef		(-1.552*10)
#define 	y2_coef		(1.65*10)

#define     intercept   (1100802)

#endif


#ifdef  SENSOR_8

#define 	x_coef 		(float)(2.92*1000)
#define 	y_coef 		(float)(-5.21*1000)

#define 	x2_coef 	(float)(3.20)
#define 	xy_coef		(float)(-1.5*10)
#define 	y2_coef		(float)(1.38*10)

#define     intercept   (490719)

#endif


#ifdef  SENSOR_9

#define 	x_coef 		(float)(13721)
#define 	y_coef 		(float)(-15047)

#define 	x2_coef 	(float)(60.83)
#define 	xy_coef		(float)(-74.18)
#define 	y2_coef		(float)(38.31)

#define     intercept   (1481471)

#endif


#ifdef  SENSOR_11

#define 	x_coef 		(float)(4095)
#define 	y_coef 		(float)(-6593)

#define 	x2_coef 	(float)(-6.10)
#define 	xy_coef		(float)(-1.33*10)
#define 	y2_coef		(float)(1.242*10)

#define     intercept   (875436)

#endif


#ifdef  SENSOR_12

#define 	x_coef 		(float)(2753)
#define 	y_coef 		(float)(-3568)

#define 	x2_coef 	(float)(13)
#define 	xy_coef		(float)(-8.45)
#define 	y2_coef		(float)(5.06)

#define     intercept   (613997)

#endif


#endif /* __FREQ_H__ */

