/*
 * app.c
 *
 *  Created on: Mar 27, 2023
 *      Author: ferna
 */

#include "app.h"
#include "main.h"
#include "cmsis_os.h"
#include "arm_math.h"
#include "stdio.h"
#include "stdlib.h"
#include "ls.h"
#include "pid_controller.h"


extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart3;
extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim6;

#define DAC_REFERENCE_VALUE_HIGH 4300 // 1023 = 3.3V, 666 = 2.15V
#define DAC_REFERENCE_VALUE_LOW 100	  // 1023 = 3.3V, 356 = 1.15V

void receiveData(float *buffer);

t_ILSdata* tILS1;


uint8_t* p_prueba;
uint32_t data;
volatile GPIO_PinState state;
uint8_t buffer_data[30];
uint8_t p;
uint16_t dacValue;
uint8_t signal_out;
float U;
float X;
float y = 0.0f;
float r = 0.0f;
float u_1 = 0.0f;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(signal_in_GPIO_Port, signal_in_Pin);
	signal_out=HAL_GPIO_ReadPin(signal_in_GPIO_Port,signal_in_Pin);
}

void ILS_Task (void* taskParmPtr)
{
	t_ILSdata* tILS;
	tILS = (t_ILSdata*) taskParmPtr;

	while(1)
	{
        ILS_Run(tILS);
        vTaskDelay(tILS->ts_Ms);
	}
}

void task_pid(void *parameter)
{

	PIDController_t PsPIDController;
	uint32_t h_ms = 4;
	float h_s = ((float)h_ms)/1000.0f;
	pidInit(&PsPIDController,
			5.0f,		// Kp
			1.0f / h_s, // Ki
			0.1f * h_s, // Kd
			h_s,		// h en [s]
			20.0f,		// N
			1.0f,		// b
			0.0f,		// u_min
			3.3f		// u_max
	);
	while (1)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 0);
		y =(float)(HAL_ADC_GetValue(&hadc1) * (3.3 / 4095));
		HAL_ADC_Stop(&hadc1);
		r = (HAL_GPIO_ReadPin(signal_in_GPIO_Port, signal_in_Pin) *2.0);
		u_1 = pidCalculateControllerOutput(&PsPIDController, y, r);
		//sprintf(buffer_data, "%f\n",u_1);
		u_1 =u_1*1240.9090f;//(4095 / 3.3);
		//u_1 =u_1*2730.0f;//(4095 / 1.5)
		sprintf(buffer_data, "%f\n",y);
		HAL_UART_Transmit(&huart3, buffer_data, strlen(buffer_data), 1);
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R,u_1 );
		pidUpdateController(&PsPIDController, y, r);
		//vTaskDelay(4);
	}
}



void receiveData(float *buffer)
{

	dacValue = DAC_REFERENCE_VALUE_LOW + rand() % (DAC_REFERENCE_VALUE_HIGH + 1 - DAC_REFERENCE_VALUE_LOW);
    U = (float)dacValue * (3.3 / 4095.0);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dacValue);
	//sprintf(buffer_data, "%f\n",U);
	//HAL_UART_Transmit(&huart3, buffer_data, strlen(buffer_data), 10);
	//vTaskDelay(5);
	//U = (float)HAL_GPIO_ReadPin(signal_output_GPIO_Port, signal_output_Pin);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 0);
	X = (float)HAL_ADC_GetValue(&hadc1) * (3.3 / 4095.0);
	sprintf(buffer_data, "%f\n",X);
	HAL_UART_Transmit(&huart3, buffer_data, strlen(buffer_data), 10);
	HAL_ADC_Stop(&hadc1);

	buffer[0] = U;
	buffer[1] = X;
}


void app(void)
{
	BaseType_t res;
	tILS1 = (t_ILSdata*) pvPortMalloc (sizeof(t_ILSdata));
	HAL_TIM_Base_Start_IT(&htim6);
	ILS_Init(tILS1, 200, 10, receiveData);

	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

	//res = xTaskCreate(ILS_Task, (const char *)"ILS_Task", configMINIMAL_STACK_SIZE, (void *)tILS1, tskIDLE_PRIORITY + 1, NULL);
	//configASSERT(res == pdPASS);

	res = xTaskCreate(task_pid, (const char *)"tarea pid ", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	configASSERT(res == pdPASS);

	osKernelStart();

}
