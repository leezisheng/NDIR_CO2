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
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	    
		// ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// �ڴ��ַ(Ҫ����ı�����ָ��)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff;
		// ���򣺴����赽�ڴ�	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// �����С	
		DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE;
		// �����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMAģʽ��һ�λ���ѭ��ģʽ
//		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
		// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// ��ֹ�ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// ����DMAͨ��		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
		// ʹ��DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// �򿪴���GPIO��ʱ��
	uart_GPIO_APBxClkCmd(uart_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	uart_APBxClkCmd(uart_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = uart_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(uart_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
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
		// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(uartx, USART_IT_RXNE, ENABLE);	
	USART_Cmd(uartx, ENABLE);
	USARTx_DMA_Config()	;
  USART_DMACmd(uartx, USART_DMAReq_Rx, ENABLE);	
}

/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ����8λ������ ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(uartx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(uartx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
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
	//���Ũ��ֵ
			case 0x4F:
				data_flag =  0x00;
			break;
		//����¶�
			case 0x5F:
				data_flag = 0x01;
			break;
		//���ʪ��
			case 0x6F:
				data_flag = 0x02;
			break;
		//ADC�ɼ�ֵ
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

/******DMA��������жϺ���************/
void USART_IRQHandler(void)
{
		uint16_t t;
	if(USART_GetITStatus(uartx,USART_IT_IDLE) != RESET)         //����ж��Ƿ���
	{	
/*****�жϿ�ʼ���ݽ����������������****************/
		DMA_Cmd(USART_TX_DMA_CHANNEL,DISABLE);                         //�ر�DMA����	
		t = DMA_GetCurrDataCounter(USART_TX_DMA_CHANNEL);              //��ȡʣ�����������
		
     Usart_SendArray(uartx,ReceiveBuff,RECEIVEBUFF_SIZE-t);       //����Է������ݣ������������� = SENDBUFF_SIZE - ʣ��δ���������������
		
		DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL,RECEIVEBUFF_SIZE);    //�������ô������������
		DMA_Cmd(USART_TX_DMA_CHANNEL,ENABLE);                          //����DMA����
		USART_ReceiveData(uartx);                              //��ȡһ�����ݣ���Ȼ��һֱ���ж�
		USART_ClearFlag(uartx,USART_FLAG_IDLE);                //������ڿ����жϱ�־λ
	}
}










