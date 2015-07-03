#include "include.h"
#include "includes.h"


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
**************************/


/* test code */

#define TASK1_PRIO 2
#define TASK1_STACK_SIZE 128
OS_STK task1Stack[TASK1_STACK_SIZE];
void task1(void *pdata);

#define TASK2_PRIO 3
#define TASK2_STACK_SIZE 128
OS_STK task2Stack[TASK2_STACK_SIZE];
void task2(void *pdata);

#define START_TASK_PRIO 10
#define START_TASK_STACK_SIZE 128
OS_STK startTaskStack[START_TASK_STACK_SIZE];
void startTask(void *pdata);



int main(void)
{
	delay_init();
	led_Configuration();
	MPU6050_Configuration();
	PWM_Configuration();
	
	
	OSInit();
	OSTaskCreate(
		startTask,
		(void *)0,
		&startTaskStack[START_TASK_STACK_SIZE - 1],
		START_TASK_PRIO);
		
	OSStart();
}

void startTask(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	pdata = pdata;
	OS_ENTER_CRITICAL();
	OSTaskCreate(
		task1,
		(void *)0,
		&task1Stack[TASK1_STACK_SIZE - 1],
		TASK1_PRIO);
	OSTaskCreate(
		task2,
		(void *)0,
		&task2Stack[TASK2_STACK_SIZE - 1],
		TASK2_PRIO);
	OSTaskSuspend(START_TASK_PRIO);
	OS_EXIT_CRITICAL();
}
	
void task1(void *pdata)
{
	for (;;) {
		LED1_TOGGLE;
		delay_ms(220);
	}
}

void task2(void *pdata)
{
	for (;;) {
		LED2_TOGGLE;
		delay_ms(300);
	}
}



/*
int main(void)
{

	int led_loop = 50000;
	int  cnt = 500000;
	led_Configuration();
//	Usart_Configuration();
	MPU6050_Configuration();
	PWM_Configuration();
	
	
	SysTick_Config(SystemCoreClock / 1000); // ms
	
	CtrData = 1;
	
                         
	PWM[0] = PWM[1] = PWM[2] = PWM[3] = 700;
	
	while (1) {
			 
		cnt--;
		if (cnt <= 0) {
			PWM[0] = PWM[1] = PWM[2] = PWM[3] = 0 ;
			PWMControl(PWM);
		}
		
		READ_MPU6050();
//		Control();
//		PWMControl(PWM);
		schedulercnt_2ms = 0;
		
		led_loop--;
		if (led_loop == 0) {
			GPIO_SetBits(GPIOB, GPIO_Pin_1);
			led_loop = 50000 * 2;
		}
		if (led_loop == 50000) {
			GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		}

		
	}
	
}
*/

