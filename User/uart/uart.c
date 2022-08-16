#include "uart.h"

uint8_t Data_flag;
uint8_t ReceiveBuff[RECEIVEBUFF_SIZE];
		
 static void UART_NVIC_Config(void)
{
	NVIC_InitTypeDef ctrl_Structure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	ctrl_Structure.NVIC_IRQChannel = USART_IRQ ;
	ctrl_Structure.NVIC_IRQChannelPreemptionPriority = 0;
	ctrl_Structure.NVIC_IRQChannelSubPriority = 2;
	ctrl_Structure.NVIC_IRQChannelCmd =ENABLE ;

	NVIC_Init(&ctrl_Structure);
}
static void USARTx_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		// 开启DMA时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	    
		// 设置DMA源地址：串口数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// 内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff;
		// 方向：从外设到内存	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// 传输大小	
		DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE;
		// 外设地址不增	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// 内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// 外设数据单位	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		// 内存数据单位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMA模式，一次或者循环模式
//		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
		// 优先级：中	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// 禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// 配置DMA通道		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
		// 使能DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	uart_GPIO_APBxClkCmd(uart_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	uart_APBxClkCmd(uart_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = uart_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(uart_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = uart_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(uart_RX_GPIO_PORT, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = uart_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(uartx, &USART_InitStructure);
	UART_NVIC_Config();
		// 使能串口接收中断
	USART_ITConfig(uartx, USART_IT_RXNE, ENABLE);	
	USART_Cmd(uartx, ENABLE);
	USARTx_DMA_Config()	;
  USART_DMACmd(uartx, USART_DMAReq_Rx, ENABLE);	
}

/*****************  发送一个字节 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(uartx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(uartx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(uartx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(uartx);
}

void Data_Analysis(uint8_t *Data)
{
	uint8_t data_flag;
	if((0x0A == Data[0])&&(0x02 == Data[1])&&(0xA0 == Data[3])&&(0x20== Data[5]))
	{
		switch(Data[2])
		{
	//输出浓度值
			case 0x4F:
				data_flag =  0x00;
			break;
		//输出温度
			case 0x5F:
				data_flag = 0x01;
			break;
		//输出湿度
			case 0x6F:
				data_flag = 0x02;
			break;
		//ADC采集值
			case 0x7F:
				data_flag = 0x03;
			break;
			default :
				data_flag = 0x04;	
			break;
		}	
	}
	Data_flag = data_flag;
	
}

/******DMA传输完成中断函数************/
void USART_IRQHandler(void)
{
		uint16_t t;
	if(USART_GetITStatus(uartx,USART_IT_IDLE) != RESET)         //检查中断是否发生
	{	
/*****中断开始数据解析，调整输出内容****************/
		DMA_Cmd(USART_TX_DMA_CHANNEL,DISABLE);                         //关闭DMA传输	
		t = DMA_GetCurrDataCounter(USART_TX_DMA_CHANNEL);              //获取剩余的数据数量
		
     Usart_SendArray(uartx,ReceiveBuff,RECEIVEBUFF_SIZE-t);       //向电脑返回数据（接收数据数量 = SENDBUFF_SIZE - 剩余未传输的数据数量）
		
		DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL,RECEIVEBUFF_SIZE);    //重新设置传输的数据数量
		DMA_Cmd(USART_TX_DMA_CHANNEL,ENABLE);                          //开启DMA传输
		USART_ReceiveData(uartx);                              //读取一次数据，不然会一直进中断
		USART_ClearFlag(uartx,USART_FLAG_IDLE);                //清除串口空闲中断标志位
	}
}










