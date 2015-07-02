#include "include.h"
#include "includes.h"

#define PWM1(X) TIM2->CCR3 = X
#define PWM2(X)	TIM2->CCR4 = X
#define PWM3(X)	TIM1->CCR1 = X
#define PWM4(X) TIM1->CCR4 = X



float GX_F, GY_F, GZ_F, T_F, AX_F, AY_F, AZ_F;			// global var ,for mpu6050_read()
float Roll, Yaw, Pitch;
float PWM[4];

u8 CtrData;		

/*************************
* OS task 
* 1. a task to initiate all hardware
* 	  then stop it,  start * and *
* 2. a task to call read_MPU6050()
* 3, a task to call IMU
* 4, a task to print data 
*
**************************
*/


/* test code */

#define TASK_STK_SIZE 512
OS_STK MyTaskStk[TASK_STK_SIZE];
OS_STK YourTaskStk[TASK_STK_SIZE];
void YourTask(void *pdata);
void MyTask(void *pdata);

void main(void)
{
	OSInit();
	delay_init();
	NVIC_Configuration();
	OSTaskCreate(
		MyTask,
		(void *)0,
		&MyTaskStk[TASK_STK_SIZE - 1],
		0);
	OSStart();
}

void MyTask(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	pdata = pdata;
	Usart_Configuration();
	OSTaskCreate(
		YourTask, 
		(void *)0, 
		&YourTaskStk[TASK_STK_SIZE - 1],
		2);
	
	for (;;) {
		OS_ENTER_CRITICAL();
		printf("hahahaa\t");
		OS_EXIT_CRITICAL();
		OSTimeDlyHMSM(0, 0, 3, 0);
	}
}

void YourTask(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	pdata = pdata;
	for (;;) {
		OS_ENTER_CRITICAL();
		printf("xixi\t");
		OS_EXIT_CRITICAL();
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}




/*

int main(void)
{

	int  cnt = 5000;
	Usart_Configuration();
	MPU6050_Configuration();
	PWM_Configuration();
	
	
	SysTick_Config(SystemCoreClock / 1000); // ms
	
	CtrData = 1;  
	
	
	PWM[0] = PWM[1] = PWM[2] = PWM[3] = 700;
	
	while (1) {
		
		if(schedulercnt_2ms >= 2)
		{	 
			cnt--;
			if (cnt <= 0) {
				PWM[0] = PWM[1] = PWM[2] = PWM[3] = 0 ;
				PWMControl(PWM);
			}
			
			READ_MPU6050();
			Control();
			PWMControl(PWM);
			
			schedulercnt_2ms = 0;

		}
		
	}
	
}
*/

