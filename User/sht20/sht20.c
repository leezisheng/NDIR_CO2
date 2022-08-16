#include "sht20.h"
#include "uart.h"
#include "math.h"
#include "iic.h"


 float temperatureC;
 float humidityRH;
 
 u8 sndata1[8];
 u8 sndata2[6];
 u32 SN1; 
 u32 SN2; 

static void delay_us( uint16_t time_us )
{
  uint16_t i,j;
  for( i=0;i<time_us;i++ )
  {
		for( j=0;j<11;j++ );//1us
  }
}

static void delay_ms(uint16_t time_ms)
{
	 uint16_t i;
  for( i=0;i<time_ms;i++ )
  {
		delay_us(11000);
  }
	
}

FunctionalState SHT2X_IIC_WriteByte(uint8_t WriteAddress,uint8_t SendByte)
{		
    if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(WriteAddress);   /* 设置低起始地址 */      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    return ENABLE;
}	

FunctionalState SHT2X_IIC_ReadByte( uint8_t ReadAddress, uint16_t length  ,uint8_t* pBuffer)
{		
	if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); /* 设置高起始地址+器件地址 */ 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(ReadAddress);   /* 设置低起始地址 */      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(0x81);
    I2C_WaitAck();
    while(length)
    {
			*pBuffer = I2C_ReceiveByte();
			if(length == 1)
			I2C_NoAck();
			else I2C_Ack(); 
			pBuffer++;
			length--;
    }
    I2C_Stop();
    return ENABLE;
} 

FunctionalState SHT2x_CheckCrc(u8 data[],u8 startBytes,u8 number, u8 checksum)
{
	u8 bit=0;
	u8 crc = 0;	
  u8 byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = startBytes; byteCtr < startBytes+number; ++byteCtr)
  { crc ^= (data[byteCtr]);
    for (bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ 0x131;
      else crc = (crc << 1);
    }
  }
  if (crc ==checksum) 
	return ENABLE;
  else
	return DISABLE;
}

u8 SHT2x_ReadUserRegister(void)
{
	u8 data[1]={0};
  SHT2X_IIC_ReadByte( USER_REG_R, 1 ,data);
  return data[0];	
}

FunctionalState SHT2x_WriteUserRegister(u8 userdata)
{
	SHT2X_IIC_WriteByte(USER_REG_W ,userdata);
	if(userdata==SHT2x_ReadUserRegister())
	return ENABLE;
	else 
	return DISABLE;
}

FunctionalState SHT2x_Calc_T(void)
{
	 u8 length=0;
	 u8 Tdata[3]={0};
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(I2C_ADR_W); 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(TRIG_T_MEASUREMENT_POLL);      
    I2C_WaitAck();
		delay_us(20);
	  I2C_Stop();
		do
		{
		I2C_Start();
    I2C_SendByte(I2C_ADR_R);
		}
		while(!I2C_WaitAck());
		for(length=0;length<=3;length++)
		{
			Tdata[length]=I2C_ReceiveByte();
			I2C_Ack(); 		
		};
		I2C_NoAck();		
		I2C_Stop();		
		if(((Tdata[0]+Tdata[1]+Tdata[2])>0)&&SHT2x_CheckCrc(Tdata,0,2,Tdata[2]))	
    temperatureC= (-46.85 + (175.72/65536 )*((float)((((u16)Tdata[0]<<8)+(u16)Tdata[1])&0xfffc)));
		else
		return DISABLE;
	  return ENABLE;
}

FunctionalState SHT2x_Calc_RH(void)
{	
   u8 length=0;
	 u8 RHdata[3]={0};
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(I2C_ADR_W); 
    if(!I2C_WaitAck())
		{I2C_Stop(); return DISABLE;}
    I2C_SendByte(TRIG_RH_MEASUREMENT_POLL);      
    I2C_WaitAck();
		delay_us(20);
	  I2C_Stop();
		do
		{
		I2C_Start();
    I2C_SendByte(I2C_ADR_R);
		}
		while(!I2C_WaitAck());
		for(length=0;length<=3;length++)
		{
			RHdata[length]=I2C_ReceiveByte();
			I2C_Ack(); 		
		};
		I2C_NoAck();		
		I2C_Stop();
		if(((RHdata[0]+RHdata[1]+RHdata[2])>0)&&SHT2x_CheckCrc(RHdata,0,2,RHdata[2]))		
	  humidityRH = -6.0 + 125.0/65536 * ((float)((((u16)RHdata[0]<<8)+(u16)RHdata[1])&0xfff0)); 
		else
		return DISABLE;
	  return ENABLE;
}

FunctionalState SHT2x_SoftReset(void)
{
	 if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(SOFT_RESET);       
    I2C_WaitAck();	  
    I2C_Stop(); 
  	delay_ms(1500);
		return ENABLE;	
}

u8 SHT2x_GetSerialNumber(u8 *pBuffer1,u8 *pBuffer2)
{   u8 length=8;
	  if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(0xfa);       
    I2C_WaitAck();	
    I2C_SendByte(0x0f);
    I2C_WaitAck();   
		if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x81); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    while(length)
    {
      *pBuffer1 = I2C_ReceiveByte();
      if(length == 1)
				I2C_NoAck();
      else I2C_Ack(); 
      pBuffer1++;
      length--;
    }
     I2C_Stop();
		length=6;
	  if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x80); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte(0xfc);     
    I2C_WaitAck();	
    I2C_SendByte(0xc9);
    I2C_WaitAck();   
		if(!I2C_Start())return DISABLE;
    I2C_SendByte(0x81); 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    while(length)
    {
      *pBuffer2 = I2C_ReceiveByte();
      if(length == 1)
				I2C_NoAck();
      else I2C_Ack(); 
      pBuffer2++;
      length--;
    }
     I2C_Stop();	
    return ENABLE;
}

void SHT2X_Init(void)
{
		SHT2x_GetSerialNumber(sndata1,sndata2);
		if((sndata1[0]+sndata1[1]+sndata1[3]+sndata1[4]+sndata1[5]+sndata1[6]+sndata1[7])>0)
		{		
		if(
		SHT2x_CheckCrc(sndata1,0,1,sndata1[1])&&
		SHT2x_CheckCrc(sndata1,2,1,sndata1[3])&&
		SHT2x_CheckCrc(sndata1,4,1,sndata1[5])&&
		SHT2x_CheckCrc(sndata1,6,1,sndata1[7])&&
		SHT2x_CheckCrc(sndata2,0,2,sndata2[2])&&
		SHT2x_CheckCrc(sndata2,3,2,sndata2[5])
		)
		{
		printf("SHT2X CRC ok\r\n");
		SN1=((sndata2[3]<<24)+(sndata2[4]<<16)+(sndata1[0]<<8)+sndata1[2]);
		SN2=((sndata1[4]<<24)+(sndata1[6]<<16)+(sndata2[0]<<8)+sndata2[1]);
		printf("SHT2X SN:0x%x%x\r\n",SN1,SN2);
		}
		else
		printf("SHT2X CRC error\r\n");	
		}
		else
		{
		SHT2x_GetSerialNumber(sndata1,sndata2);
//		printf("SHT2X通信错误\r\n");
		};

//		if(SHT2x_WriteUserRegister(0x3a))
//		printf("设置成功\r\n");
//		else 
//		printf("设置错误\r\n");
}

void SHT2X_TEST(void)
{
		if(SHT2x_Calc_T())
		{ 
			 printf("温度：%f\r\n",temperatureC);
		}
		if(SHT2x_Calc_RH())
		{
			printf("湿度：%f%%\r\n",humidityRH);
		}	
}


