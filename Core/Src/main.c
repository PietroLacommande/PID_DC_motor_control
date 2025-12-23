/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h> // Required for atoi
#include <stdio.h>   // for sprintf()
#include <string.h>  // for strlen()
#include "pid.h"
#include "lowLevel.h"

#ifdef DEBUG
	volatile int32_t diff_global = 0;
	volatile float rpsSpeedVal = 0;
#endif



#define UART_BUFFER_SIZE 64
uint8_t rxByte;
uint8_t uartBuf[UART_BUFFER_SIZE];
volatile uint8_t uart_ready = 0;
volatile uint16_t uartIndex = 0;


volatile float desiredSpeed =0; //value can be modified in many places
volatile float control=0; //value can be modified in many places

PidController motorPID = {
    .Kp = 500.0f,
    .Ki = 1000.0f,
    .Kd = 0.5f,
    .currentError = 0,
    .pastError = 0,
    .pastError2 = 0,
    .integral = 0,
};



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   if (huart->Instance == USART2)
    {
        // End of line received
    	if (rxByte == '\r') {
    	            // ignore, wait for '\n'
		}
		else if (rxByte == '\n') {
			uartBuf[uartIndex] = '\0';
			uart_ready = 1;
			uartIndex = 0;
		}
        else if (uartIndex < UART_BUFFER_SIZE - 1)
        {
            uartBuf[uartIndex++] = rxByte; // store byte
        }

		HAL_UART_Receive_IT(&huart2, &rxByte, 1);
    }
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM5){
		static int32_t previousCount=0; //static pour que ca garde l'ancienne valuere
		uint16_t currentCount = __HAL_TIM_GET_COUNTER(&htim1);
		int32_t diff = currentCount-previousCount;

		//overflow positif
		if(diff > 32767){
			diff -= 65536;
		}

		//overflow negatif
		else if(diff < -32768){
			diff += 65536;
		}

		previousCount = currentCount;

#ifdef DEBUG
		diff_global = diff;
#endif


		float instantSpeed = (float)diff * DELTA_K;
		rpsSpeedVal = instantSpeed;

		control = PID(&motorPID,desiredSpeed, instantSpeed);

		if (desiredSpeed > 0.05) {
		  control += 2500; //en dessous il n'y a pas de pwm
		} else {
		  control = 0; // Turn off motor if desiredSpeed is 0
		}


		if(control<0){
			control=0;
		}
		else if(control>3999){
			control =3999;
		}
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (uint32_t)control);

	}

}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  initAllLowLevel();
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); // Set PC6 high
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // Set PC8 low
  HAL_UART_Receive_IT(&huart2, &rxByte, 1);

  /* Infinite loop */
  while (1)
  {

	  if (uart_ready==1)
	  {
	      uart_ready = 0;

	      if (strncmp((char*)uartBuf, "Kp=", 3) == 0) {
	          motorPID.Kp = atof((char*)uartBuf + 3);
	          motorPID.integral = 0;
	          motorPID.pastError = 0;
	          motorPID.pastError2 = 0;
	      }
	      else if (strncmp((char*)uartBuf, "Ki=", 3) == 0) {
	          motorPID.Ki = atof((char*)uartBuf + 3);
	          motorPID.integral = 0;
	      }
	      else if (strncmp((char*)uartBuf, "Kd=", 3) == 0) {
	          motorPID.Kd = atof((char*)uartBuf + 3);
	          motorPID.pastError = 0;
	          motorPID.pastError2 = 0;
	      }
	      else if (strncmp((char*)uartBuf, "REF=", 4) == 0)
			  desiredSpeed = atof((char*)uartBuf + 4);

	  }

  }
}


