#include "stm32f10x.h"
#include <stdio.h>
#include "Usart.h"
u8 TxBuffer[256];
u8 TxCounter=0;
u8 count=0;
u8 CtrData=0;
u8 RxBuffer[50];
static u8 RxState = 0;
void Usart_Configuration(void)			//����Usart1 Tx->PA9 Rx->PA10
{
	 GPIO_InitTypeDef GPIO_InitStructure; //GPIO�⺯���ṹ��
	 USART_InitTypeDef USART_InitStructure;//USART�⺯���ṹ��
	 USART_ClockInitTypeDef USART_ClockInitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	 //ʹ�ܴ���1��GPIOA��AFIO����
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1,ENABLE);

	 /* Configure USART1 Tx (PA9) as alternate function push-pull */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//PA9ʱ���ٶ�50MHz
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 /* Configure USART1 Rx (PA10) as input floating */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	 USART_InitStructure.USART_BaudRate =9600; //9600; //������115200
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8λ����
	 USART_InitStructure.USART_StopBits = USART_StopBits_1; //1��ֹͣλ
	 USART_InitStructure.USART_Parity = USART_Parity_No; //��żʹ��
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���͡�����ʹ��
	
	 USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	 USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;//����ʱ��Ϊ�͵�ƽ
	 USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;//ʱ�ӵڶ������ؽ������ݲ���
	 USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;//���һλ���ݵ�ʱ�����岻��SCLK���

	//ʹ�ܴ���1�����ж�
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 

	 USART_ClockInit(USART1, &USART_ClockInitStructure);
	 USART_Init(USART1, &USART_InitStructure);	//��ʼ���ṹ��
	 USART_Cmd(USART1, ENABLE); //ʹ�ܴ���1
	 
	 NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
}

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
/***************************START*********************/
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	USART1->DR = (u8) ch;      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	return ch;
}
#endif 
/***************************END*********************/


void Uart1_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
void Uart1_Put_String(unsigned char *Str)
{
	//�ж�Strָ��������Ƿ���Ч.
	while(*Str)
	{
	//�Ƿ��ǻس��ַ� �����,������Ӧ�Ļس� 0x0d 0x0a
	if(*Str=='\r')Uart1_Put_Char(0x0d);
		else if(*Str=='\n')Uart1_Put_Char(0x0a);
			else Uart1_Put_Char(*Str);
	//ָ��++ ָ����һ���ֽ�.
	Str++;
	}
}
void Uart1_Put_Buf(unsigned char *DataToSend , u8 data_num)
{
	u8 i=0;
	for(i=0;i<data_num;i++)
		TxBuffer[count++] = *(DataToSend+i);
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}

/******************����1�жϷ�����*******************************/
void USART1_IRQHandler()
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//??!????if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)???
    {
        USART_ReceiveData(USART1);
    }
		
	//�����ж�
	if((USART1->SR & (1<<7))&&(USART1->CR1 & USART_CR1_TXEIE))//if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET)
	{
		USART1->DR = TxBuffer[TxCounter++]; //дDR����жϱ�־          
		if(TxCounter == count)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�ж�
			//USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
		}
	}
	//�����ж� (���ռĴ����ǿ�) 
	if(USART1->SR & (1<<5))//if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    
	{
		CtrData = USART1->DR;
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); 
	}
}
