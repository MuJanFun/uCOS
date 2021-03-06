#include "include.h"
#include "FlyControl.h"
#include "Pwm.h"








/**** int main.c ****/
extern u8 CtrData;
extern float Pitch, Roll, Yaw;
extern float PWM[4];




float COS=0;
float FeedBack(float,int);
float InfMax(float);
float FeedBackRollOut = 0, FeedBackRollIn = 0, FeedBackPitchOut = 0,FeedBackPitchIn = 0, FeedBackYawIn = 0, FeedBackYawOut=0;
float M=830;


float deltaT;

/* data it usese,
*  CtrData 
*  data from MPU, 
*/

float CPitch=0;
float CRoll=0;
float CYaw=0;

void Control()
{

	switch(CtrData)
	{
	case CTRL_EMPTY://空周期
		;
		break;
	case CTRL_PITCH_UP:
		CPitch+=CtrSize;//俯仰，机头抬升
		break;
	case CTRL_PITCH_DOWN:
		CPitch-=CtrSize;//俯仰，机头下降
		break;
	case CTRL_ROLL_LEFT:
		CRoll+=CtrSize;//横滚，偏左
		break;
	case CTRL_ROLL_RIGHT:
		CRoll-=CtrSize;//横滚，偏右
		break;
	case CTRL_UP:
		M+=CtrSize;//马力增加，抬升
		if(M>=950)
		{
			while(1)
			{
				PWM[0]=PWM[1]=PWM[2]=PWM[3]=0;
				PWMControl(PWM);
			}
		}
		break;
	case CTRL_DOWN:
		M-=CtrSize;//马力减小，下降
		break;
	case CTRL_STOP://停止
		while(1)
		{
			PWM[0]=PWM[1]=PWM[2]=PWM[3]=0;
			PWMControl(PWM);
		}
		break;
	}
	IMUupdate(GX_F,GY_F,GZ_F,AX_F,AY_F,AZ_F);
	//IMUupdate(0,0,0,AX_F,AY_F,AZ_F);
	FeedBackPitchOut=FeedBack(Pitch-CPitch,1);
	FeedBackRollOut=FeedBack(Roll-CRoll,0);
	FeedBackYawOut=FeedBack(Yaw-CYaw,2);
	
	FeedBackPitchIn=FeedBack(FeedBackPitchOut+GY_F,4);
	FeedBackRollIn=FeedBack(FeedBackRollOut+GX_F,3);
	FeedBackYawIn=FeedBack(FeedBackYawOut+GZ_F,5);
	COS=1;//pow(cos(Roll/57.3)*cos(Pitch/57.3),2);
	
	
	PWM[0]=InfMax((M-FeedBackPitchIn/2-FeedBackYawIn/4)/COS);
	PWM[1]=InfMax((M+FeedBackRollIn/2+FeedBackYawIn/4)/COS);
	PWM[2]=InfMax((M-FeedBackRollIn/2+FeedBackYawIn/4)/COS);
	PWM[3]=InfMax((M+FeedBackPitchIn/2-FeedBackYawIn/4)/COS);
	
//	PWM[0]=InfMax((M-FeedBackPitchIn)/COS);
//	PWM[1]=InfMax((M+FeedBackRollIn)/COS);
//	PWM[2]=InfMax((M-FeedBackRollIn)/COS);
//	PWM[3]=InfMax((M+FeedBackPitchIn)/COS);
	
	
//	PWM[0]=InfMax((M-FeedBackPitchOut)/COS);
//	PWM[1]=InfMax((M+FeedBackRollOut)/COS);
//	PWM[2]=InfMax((M-FeedBackRollOut)/COS);
//	PWM[3]=InfMax((M+FeedBackPitchOut)/COS);
		
	
//	PWM[0]=InfMax((M+FeedBackYawIn+FeedBackRollIn-FeedBackPitchIn)/COS);
//	PWM[1]=InfMax((M-FeedBackYawIn+FeedBackRollIn+FeedBackPitchIn)/COS);
//	PWM[2]=InfMax((M-FeedBackYawIn-FeedBackRollIn-FeedBackPitchIn)/COS);
//	PWM[3]=InfMax((M+FeedBackYawIn-FeedBackRollIn+FeedBackPitchIn)/COS);
	
	
	
//	PWM[0]=InfMax((M+FeedBackYawOut+FeedBackRollOut-FeedBackPitchOut)/COS);
//	PWM[1]=InfMax((M-FeedBackYawOut+FeedBackRollOut+FeedBackPitchOut)/COS);
//	PWM[2]=InfMax((M-FeedBackYawOut-FeedBackRollOut-FeedBackPitchOut)/COS);
//	PWM[3]=InfMax((M+FeedBackYawOut-FeedBackRollOut+FeedBackPitchOut)/COS);

//	if(schedulercnt_20ms > 60)
//	{
//		//cuo0=0;cuo1=0;
//		//printf("123564");
//		//show4(PWM[0],PWM[1],PWM[2],PWM[3]);
//		//send1(1.1);
//		//show1(data - lastdata[i]);	
//		show4(Roll,CRoll,Pitch,CPitch	);	
//		schedulercnt_20ms = 0;
//	}
//	
	
}
float InfMax(float x)
{
	if(x>999)
		return 999;
	else
		return x;
}
float FeedBack(float data,int i){
	static float lastdata[6] = {0,0,0,0,0,0};
	static float alldata[6] = {0,0,0,0,0,0};
	float result;
	float P, D, I;
	deltaT = getDeltaT1(GetSysTime_us(),i);
	if(i<2)
	{
		if(i==0)
		{
			P = PB__roll; 
			D = DB__roll;
			I = IB__roll;
		}
		else if(i==1)
		{
			P = PB__pitch;
			D = DB__pitch;
			I = IB__pitch;
		}
		else
		{
			P = PB__yaw;
			D = DB__yaw;
			I = IB__yaw;
		}
	}
	else
	{
		if(i==3)
		{
			P = PB__roll_in; 
			D = DB__roll_in;
			I = IB__roll_in;
		}
		else if(i==4)
		{
			P = PB__pitch_in; 
			D = DB__pitch_in;
			I = IB__pitch_in;
		}
		else
		{
			P = PB__yaw_in; 
			D = DB__yaw_in;
			I = IB__yaw_in;
		}
	}
	if (data*(data - lastdata[i])>0||(data - lastdata[i]==0 && data!=0))
		alldata[i]+=data;
	result = P*data + D*(data - lastdata[i])+ I*alldata[i];
	lastdata[i] = data;		
	return result;
}
