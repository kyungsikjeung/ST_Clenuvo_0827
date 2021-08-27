/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "buzzer.h"
#include "key.h"
#include "i2c.h"
  
#define PowerOn 0
#define PowerWait 1
#define PreHeating 2
#define ReferenceValue 3
#define Normal_5min  4
#define Normal_30sec  5
  
#define On 1
#define Off 0

#define Blue 1
#define Orange 2
#define Red 3

/* PrevStage */
#define None 0
#define LessThanTwoPointEight 1
#define GreaterThanTwoPointEight 2
#define GreaterThanThreePointZero 3

#define Blue_Led_On() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET)
#define Blue_Led_Off() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET)
#define Orange_Led_On() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)
#define Orange_Led_Off() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)
#define Red_Led_On() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)
#define Red_Led_Off() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)
#define Red_Led_Toggle() HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_10) 


#define Ionizer_On() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET)
#define Ionizer_Off() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET)

#define Fan_On() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET)
#define Fan_Off() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET)
#define Fan_Read() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)
  
#define ToggleLed 0
#define SensorStatus 1
#define FanError 2
#define Er 3
#define Clr 4 
#define E2PWriteCheckEr 5
#define E2PReadCheckEr 6  
#define DelayRead 1000

  
struct _ErCheck{
  uint8_t WriteFlag;
  uint8_t WriteNum;
  uint8_t ReadFlag;
  uint8_t ReadNum;
  uint8_t ClrFlag;
  uint8_t CheckNum;
};
struct _Time{     
  uint8_t s;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
};
extern struct _Time Time;
extern struct _Time E2PTime;


struct _Flag{
  uint8_t Error;
  uint8_t Buz;
  uint8_t I2C;
  uint8_t Cartridge;
  uint8_t WindowLed;
  uint8_t Ionizer;
  uint8_t I2CTest;
  uint8_t Fan;
  //uint8_t Uart;
  uint8_t Sensor;
  uint8_t Read;
  uint8_t Write;
  uint8_t Time;
  uint8_t I2CRun;
  uint8_t Ave;
  uint8_t MulTouch;
  uint8_t Memory;
  uint8_t FlashE2P;
  uint8_t FlashClr;
  uint8_t UartSaveRead;
  uint8_t UartAdc;
};
extern struct _Flag Flag;
extern uint8_t test,Flash;
extern uint32_t Time_Status;
extern uint8_t Mode,BuzMode,WindowMode;
extern uint8_t Power,Led_Cnt,SensorLedStatus,Error;
extern uint32_t Time_Key_Delay,Time_Key,Time_Buz,Time_I2C,TimeWindowLed;
extern void ClrTime(void);
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
