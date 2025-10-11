/*
 * lowLevel.h
 *
 *  Created on: Oct 11, 2025
 *      Author: Pietr
 */

#ifndef INC_LOWLEVEL_H_
#define INC_LOWLEVEL_H_

#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;


void initAllLowLevel(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM5_Init(void);
void MX_TIM6_Init(void);


#endif /* INC_LOWLEVEL_H_ */
