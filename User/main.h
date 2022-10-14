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
	
	头文件主要是不同的传感器参数
 *********************************************************************************************************************************************
**/ 

#ifndef __MAIN_H__
#define __MAIN_H__

/* 多项式-----------------------------------------------*/ 

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

