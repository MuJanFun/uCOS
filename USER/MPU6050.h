#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "stm32f10x.h"

#define SCL_H         GPIOB->BSRR = GPIO_Pin_10
#define SCL_L         GPIOB->BRR  = GPIO_Pin_10 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_11
#define SDA_L         GPIOB->BRR  = GPIO_Pin_11

#define SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define SDA_read      GPIOB->IDR  & GPIO_Pin_11

#define I2C_Speed 						400000
#define I2C1_MPU6050					0xd0


/***********�궨��MPU6050�ڲ��Ĵ�����ַ****************/
#define	SMPLRT_DIV					0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIGM							0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG					0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG				0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H				0x3B
#define	ACCEL_XOUT_L				0x3C
#define	ACCEL_YOUT_H				0x3D
#define	ACCEL_YOUT_L				0x3E
#define	ACCEL_ZOUT_H				0x3F
#define	ACCEL_ZOUT_L				0x40
#define	TEMP_OUT_H					0x41
#define	TEMP_OUT_L					0x42
#define	GYRO_XOUT_H					0x43
#define	GYRO_XOUT_L					0x44	
#define	GYRO_YOUT_H					0x45
#define	GYRO_YOUT_L					0x46
#define	GYRO_ZOUT_H					0x47
#define	GYRO_ZOUT_L					0x48
#define	PWR_MGMT_1					0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I						0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)  ֮ǰ��75
#define	MPU6050_Addr   0xD0	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ��Ų�ͬ�޸�


//ȫ�ֱ�������
#define Gyro_Gr	0.0010653				//���ٶȱ�ɻ���	�˲�����Ӧ����2000��ÿ��

extern unsigned char BUF[20];       	//�������ݻ�����
extern char  test; 					 	//IIC

extern short GX,GY,GZ,T,AX,AY,AZ;
extern float GX_F,GY_F,GZ_F,T_F,AX_F,AY_F,AZ_F;



void I2C_Congiguration(void);
int Init_MPU6050(void);

unsigned char Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);		     //void;
unsigned char I2C_RadeByte(void);
void I2C_SendByte(u8 SendByte);
unsigned char I2C_WaitAck(void) ;
void I2C_NoAck(void);
void I2C_Ack(void);
void I2C_Stop(void);
void READ_MPU6050(void);
void I2C_GPIO_Config(void);
unsigned char I2C_Start(void);
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);
#endif

