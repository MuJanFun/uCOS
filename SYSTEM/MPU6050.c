#include "stm32f10x.h"
#include "mpu6050.h"
#include "math.h"




int MPU6050_Configuration(void)
{
	u16 i;
	I2C_GPIO_Configuration();
	do{
		i=Single_Read(0xD0,WHO_AM_I);
		Single_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//解除休眠状态
		Single_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);	// 
		Single_Write(MPU6050_Addr,CONFIGM, 0x06);		//
		Single_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);	//
		Single_Write(MPU6050_Addr,ACCEL_CONFIG, 0x01);	//	
	}
	while (i != 0x68);

	return 1;
}

short GX, GY, GZ, T, AX, AY, AZ;
unsigned char BUF[20];       //buffer for mpu_read module

void READ_MPU6050(void)
{
	float transform = sqrt(2)/2;
	float axf, ayf, gxf, gyf;//,gzf,azf;
	BUF[0]=Single_Read(0xD0,GYRO_XOUT_L); 
	BUF[1]=Single_Read(MPU6050_Addr,GYRO_XOUT_H);
	GX = (BUF[1]<<8)|BUF[0];
	gyf = GY_F = GY / 16.4; 						             //读取计算GX数据

	BUF[2]=Single_Read(MPU6050_Addr,GYRO_YOUT_L);
	BUF[3]=Single_Read(MPU6050_Addr,GYRO_YOUT_H);
	GY = (BUF[3]<<8)|BUF[2];
	gxf = GX_F = GX / 16.4; 						             //读取计算GY数据

	BUF[4]=Single_Read(MPU6050_Addr,GYRO_ZOUT_L);
	BUF[5]=Single_Read(MPU6050_Addr,GYRO_ZOUT_H);
	GZ =	(BUF[5]<<8)|BUF[4];
	GZ_F = GZ / 16.4; 					                 //读取计算GZ数据

	BUF[6]=Single_Read(MPU6050_Addr,TEMP_OUT_L); 
	BUF[7]=Single_Read(MPU6050_Addr,TEMP_OUT_H); 
	T = (BUF[7]<<8)|BUF[6];
	T = 35+ ((double) (T + 13200)) / 280;     //读取计算合成 温度

	BUF[8]=Single_Read(MPU6050_Addr,ACCEL_XOUT_L);
	BUF[9]=Single_Read(MPU6050_Addr,ACCEL_XOUT_H);
	AX =	(BUF[9]<<8)|BUF[8];
	axf = AX_F = AX / 16384.00; 					                 //读取计算AX数据

	BUF[10]=Single_Read(MPU6050_Addr,ACCEL_YOUT_L);
	BUF[11]=Single_Read(MPU6050_Addr,ACCEL_YOUT_H);
	AY =	(BUF[11]<<8)|BUF[10];
	ayf = AY_F = AY / 16384.00; 					                 //读取计算AY数据

	BUF[12]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_L);
	BUF[13]=Single_Read(MPU6050_Addr,ACCEL_ZOUT_H);
	AZ =	(BUF[13]<<8)|BUF[12];
	AZ_F = AZ / 16384.00; 					                 //读取计算AZ数据
   
	GX_F =   transform * gxf + transform * gyf;
	GY_F = - transform * gxf + transform * gyf;
	AX_F =   transform * axf + transform * ayf;
	AY_F = - transform * axf + transform * ayf;          //将X型转换成+型
}

/*
******************************************************************************
* Function Name  : I2C_GPIO_Configuration
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** 
*/
void I2C_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE  );

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

char  test; 				 	//IIC  , don't understand yet

//****单字节写入*******************************************

unsigned char Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return 0;
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return 0;}
    I2C_SendByte(REG_Address);   //设置低起始地址      
    I2C_WaitAck();	
    I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    delay5ms();
    return 1;
}
//****单子节读出*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   
    unsigned char REG_data;  
	  if(!I2C_Start())return 0;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop();test=1; return 0;}
    I2C_SendByte((u8) REG_Address);   //设置低起始地址     
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);
    I2C_WaitAck();
	  REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
	 return REG_data;

}						      



/*
******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** 
*/
void I2C_delay(void)
{
		
   u8 i=30; //这里可以优化速度	,经测试最低到5还能写入
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
   int i=5000;  
   while(i) 
   { 
     i--; 
   }  
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** 

*/
unsigned char I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return 0;	//SDA线为低电平,则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return 0;	//SDA线为高电平,则总线出错,退出
	SDA_L;
	I2C_delay();
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** 

*/
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** 

*/
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** 

*/
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** 

*/
unsigned char I2C_WaitAck(void) 	 //返回:=1有ACK, =0无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
	  I2C_delay();
      return 0;
	}
	SCL_L;
	I2C_delay();
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** 

*/
void I2C_SendByte(u8 SendByte) //数据从高位到低位//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
		if(SendByte&0x80)
			SDA_H;  
		else 
			SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** 

*/
unsigned char I2C_RadeByte(void)  //数据从高位到低位//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 




