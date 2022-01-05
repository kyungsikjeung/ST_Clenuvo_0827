/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

#define ADC_CONVERTED_DATA_BUFFER_SIZE   ((uint32_t)  2)   
uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];

uint16_t ADC_data;
uint8_t Power,Led_Cnt,SensorLedStatus,Error;
uint8_t Mode,BuzMode,WindowMode;

/* 시간 변수 
* Time_FanLed : 팬 불량일떄 LED 토글링하기 위한 타이머
* Time_Key_Delay : 키 Debounce Time
* Time_Key : 키 눌린 시간
* Time_Buz : 부저 시간
*/
uint32_t Time_Key_Delay,Time_Key,Time_Buz,Time_I2C,TimeWindowLed,Time_Fan,Time_FanLed,Time_FanPWM,Time_Read,Time_Ionizer,Time_90D,Time_Er_LED,Time_CheckWrite,Time_Uart;
// uint32_t cnt_test=0; 
uint16_t cnt;
uint16_t Cov_Cnt;
uint16_t Test_Time;
uint32_t Time_Status;

struct _Flag Flag;
struct _Time Time;
struct _Time  E2PTime;
struct _ErCheck ErCheck;
uint8_t data[4]={20,12,3,4};
//uint8_t TestData[4]={255,255,255,255};
uint8_t TestData[4]={0,0,0,0};               //시간 테스트용 변수    초,분,시,날
uint8_t Read_data[4]={0};
uint8_t Read_data_1[4]={0};
uint8_t Read_data_2[4]={0};
uint8_t SaveTime[4]={0}; // 저장할때 기억을 한다.시간
uint8_t SaveRead[4]={0}; // 
uint8_t FanSpeed = 70;
uint8_t FanOnTime = 14;
uint8_t FanOffTime = 21;
//uint8_t test_num=1;

/* 시간(일,시,분,초) 초기화 */
void ClrTime(void)
{
  Time.s=0;Time.min=0;Time.hour=0;Time.day=0;
}

/*  카트리지  잔량 표시등  OFF*/
void CartridgeLedOff(void)
{  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_5,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_1|GPIO_PIN_9|GPIO_PIN_14|GPIO_PIN_13,GPIO_PIN_RESET);  
}

/*  파워 LED OFF  */
void ClrWindowLed(void)
{
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8|GPIO_PIN_15,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
}

/* 카트리지 사용에 따른 카트리지 LED 변경 */
void DoCartridge(void)
{
  if(WindowMode == E2PWriteCheckEr || WindowMode == E2PReadCheckEr){
    CartridgeLedOff();
    return;
  }
  if(Flag.Cartridge && Power && !(!Flag.Fan && Flag.Error) && Flag.I2CRun){
      //Flag.Fan=1;
      //Flag.Ionizer=1;
      if(E2PTime.day<10){ 
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_SET);    
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);     
      }    
      else if(E2PTime.day<20){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_7,GPIO_PIN_SET);    
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0|GPIO_PIN_5,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);         
      }
      else if(E2PTime.day<30){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);    
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_5,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);
      }
      else if(E2PTime.day<40){
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_7,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);
      }
      else if(E2PTime.day<50){       
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_7,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);    
      }
      else if(E2PTime.day<60){
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_7,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11|GPIO_PIN_1|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);   
      }
      else if(E2PTime.day<70){
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_7,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_1|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);              
      }
      else if(E2PTime.day<80){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_9,GPIO_PIN_RESET);           
      }
      else if(E2PTime.day<90){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);       
      }
      else{
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);    
        ClrTime();
        if(Time_90D<250){ // 0.25초 카트리지 R LED ON
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
          return;
        }
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
        if(Time_90D>500){ // vice versa                                                           
          Time_90D=0;
        }        
      }         
  }
  else{
    if(!Flag.I2CRun){
               CartridgeLedOff();
    }
    else if(!Flag.WindowLed && Power && E2PTime.day>=80 && E2PTime.day<90  ){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);  
    }
    else if(!Flag.WindowLed && Power && E2PTime.day == 90){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);    
        ClrTime();
        if(Time_90D<250){ // 0.25초 카트리지 R LED ON
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
          return;
        }
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
        if(Time_90D>500){ // vice versa                                                           
          Time_90D=0;
        }
    }else{
      if(Power && !Flag.WindowLed && E2PTime.day == 90  && Flag.I2C ){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);    
        ClrTime();
        if(Time_90D<250){ // 0.25초 카트리지 R LED ON
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
          return;
        }
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
        if(Time_90D>500){ // vice versa                                                           
          Time_90D=0;
        }
      }else if(Power && !Flag.WindowLed && E2PTime.day < 90 && E2PTime.day >= 80 && Flag.I2C ){
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);      
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_0,GPIO_PIN_RESET);             
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_9,GPIO_PIN_RESET);    
      }else{
         CartridgeLedOff();
      }
    }   
  }
}

/* 부저 울림  - 타이머 동작에 의한 제어 
* @ Param BuzMode {0 :  , 2: , 3:, 4:, 5:, 6:}
* @ Param 
* 180 - 시간 단위 확인 할것
*/
void DoBuz(void)
{
  if(Flag.Buz){
    switch(BuzMode){
      case 0:
        HAL_TIM_Base_Start_IT(&htim6);
        ++BuzMode;
        break;
      case 1:
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Stop_IT(&htim6);
        ++BuzMode;
        break;
      case 2:
        //if(Flag.Fan==0 && Power && (E2PTime.day<90)){ // 팬고장일시 부저 2번
        if(Flag.Fan==0 && Power){ // 팬고장일시 부저 2번
          BuzMode=3;Time_Buz=0;
          break;
        }
        BuzMode=0;
        Flag.Buz=0;
        break;
      case 3: // 휴식시간
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Start_IT(&htim6);
        ++BuzMode;
        Time_Buz=0;
        break;
      case 4: // 소리 다시
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Stop_IT(&htim6);
        ++BuzMode;
        BuzMode=0;
        Flag.Buz=0;
        break;
      case 5:
        HAL_TIM_Base_Start_IT(&htim6);
        if(Flag.Memory){
          BuzMode=6;
        }        
        else{ // 메모리 모드 비활성화
          if(Time_Buz<500){break;}
          HAL_TIM_Base_Stop_IT(&htim6);
          BuzMode=0;
          Flag.Buz=0;
        }
        break;
      case 6:
          if(Time_Buz<180){break;}
          HAL_TIM_Base_Stop_IT(&htim6);
          Time_Buz=0;
          ++BuzMode;
        break;
      case 7:
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Start_IT(&htim6);
        Time_Buz=0;
        ++BuzMode;
        break;
      case 8:
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Stop_IT(&htim6);
        Time_Buz=0;
        ++BuzMode;
        break;
      case 9:
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Start_IT(&htim6);
        Time_Buz=0;
        ++BuzMode;
        break;  
      case 10:
        if(Time_Buz<180){break;}
        HAL_TIM_Base_Stop_IT(&htim6);
        BuzMode=0;
        Time_Buz=0;
        Flag.Buz=0;
        break;        
      default:BuzMode=0;
        break;      
    }  
  }
  else{
    Time_Buz=0;BuzMode=0;
    Buz_Off();
  }
}

/* ID CHIP */
void DoI2C(void)
{
  if(Flag.I2C){
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)){ // 카트리지 미삽입
      if(Time_I2C>200){
        Flag.I2C=0;
        Time_I2C = 0;
        //ClrTime();            //20210117 - EEPROM을 뺏다 꼽아도 동작 시간 클리어 X
        if(Power){Flag.Buz=1;} 
        WindowMode=Er;
        Flag.Read=0; 
        Flag.I2CRun=0;
        Time_Read = 0;
        // CartridgeLedOff();
        //if(Flag.Change){E2PTime.s=0;E2PTime.min=0;E2PTime.hour=0;E2PTime.day=0;Flag.Change=0;}
      }
    }
    else{
      Time_I2C=0;
      
    }  
  }
  else{
    if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)){ // 카트리지 삽입
      if(Time_I2C>500){
        Flag.Read=1;       
        Flag.I2C=1;
        if(Power){Flag.Buz=1; }    
        //WindowMode=Clr;
        Flag.Error=0;
      }
    }
    else{
      Time_I2C=0;
      Flag.Error=1;
    }       
  }
}

int8_t sign = 1;
int8_t max  = 2;
int8_t alpha = 0;

void DoFan(void){
  if(Flag.Error || WindowMode == E2PWriteCheckEr || WindowMode == E2PReadCheckEr || E2PTime.day>=90){
    Fan_Off();
    Time_Fan=0;
    return;
  }
  if(Flag.Fan && Power && Flag.I2CRun){                        //I2CRun 추가 해야됨.    // Fan off 시 PB4에 High가 들어옴 Fan 100% 동작 시 대략 4.5ms high 4.5ms low 반복. 
    if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)){  //팬이 동작할 때 5초간 안들어 올 경우 에러.
      Time_Fan=0;
    }
    if(Time_Fan>5000){
      Flag.Buz=1;
      Flag.Fan=0;
      Flag.Error=1;    
    }
    if(!Flag.WindowLed){
      if(Time_FanPWM<4){         //4ms          //나이트 모드 
        Fan_On();
      }
      else if(Time_FanPWM<11){  //11ms
        Fan_Off();
      }
      else{Time_FanPWM=0;}
    }
    else{
        Fan_On(); // 정상모드
        Time_FanPWM=0;
    }
  }
  else{
    Fan_Off();
    Time_Fan=0;
  }
}


void DoIonizer(void)
{
  if(Flag.Error || WindowMode == E2PWriteCheckEr || WindowMode == E2PReadCheckEr || E2PTime.day>=90){
    Ionizer_Off();
    Time_Ionizer=0;
    return;
  }
  if(Flag.Ionizer && Power){
    if(!Flag.WindowLed){ // 나이트 
      if(Time_Ionizer<3000){ Ionizer_On(); }
      else if(Time_Ionizer<8000){ Ionizer_Off(); }
      else{Time_Ionizer=0;}
    }
    else{ // 정상                                                                                   
      Ionizer_On();                                  
      Time_Ionizer=0;                              
    }
  }
  else{
    Ionizer_Off();
    Time_Ionizer=0;
  }
}

/* 제품 작동 시간에 따른  모드 변경( preheating, Orange, Blue) */
void DoMode()
{
  if(Flag.Error || WindowMode==E2PWriteCheckEr || WindowMode==E2PReadCheckEr){return;}
  //if(Time.min<5 && Time.hour == 0 && Time.day ==0){                       //5분 동작
  if(Time.min<5 && Time.hour == 0 && Time.day ==0){
    Mode=PreHeating;
    WindowMode=ToggleLed;
  }
  //else  if(Time.min<25 && Time.hour == 0 && Time.day ==0){            //20분 동작
    else  if(Time.min<25 && Time.hour == 0 && Time.day ==0){
    Mode=ReferenceValue;
    WindowMode=SensorStatus;
    SensorLedStatus=Orange;
  }
  //else  if(Time.min<30 && Time.hour == 0 && Time.day ==0){            //5분 동작
  else  if(Time.min<30 && Time.hour == 0 && Time.day ==0){
    Mode=Normal_5min;
    WindowMode=SensorStatus;
    SensorLedStatus=Blue;
  }
  else{
    Mode=Normal_30sec;
    WindowMode=SensorStatus;
  }
}

/* 강제 IDCHIP 시간 변경 : Flag.I2cTest , TestData 변경  */
void DoI2CTest(void)
{
  if(Flag.I2CTest){    
    I2C_Mem_Write(0xa0,MemAdd,TestData,4);
    E2PTime.s=TestData[0];
    E2PTime.min=TestData[1];
    E2PTime.hour=TestData[2];
    E2PTime.day=TestData[3];
    Flag.I2CTest=0;
  }
}

/* 이벤트에 따른  WindowLed변경 (고장, 파워,  */
void DoWindowLed(void)
{
  if((Flag.Error && Power && Flag.WindowLed) || (E2PTime.day>=90 && Power ) ){            
    Blue_Led_Off();
    Orange_Led_Off();
    
    if(!Flag.Fan && Flag.Error && Power){
      if(Time_FanLed<250){      //Fan 에러일 경우에 Led 토글링 하는 스피드 현재는 0.25초 On 0.25초 Off.
        Red_Led_On();
        return;
      }
      Red_Led_Off();
      if(Time_FanLed>500){
        Time_FanLed=0;
      }
      return;
    }
    else{
      Red_Led_On();
      Time_FanLed=0;
      return;
    }  
  }
  if(WindowMode==E2PWriteCheckEr){
    Orange_Led_Off();
    Blue_Led_Off();
    Red_Led_On();
  }
  else if(WindowMode==E2PReadCheckEr){
    Orange_Led_Off();
    Blue_Led_Off();
    Red_Led_On();    
  }
  Time_FanLed=0;
  if(Flag.WindowLed && Power){
    switch(WindowMode){
      case ToggleLed:
        if(Led_Cnt==0){
          Blue_Led_On();
          Red_Led_Off();
          Orange_Led_Off();
        }
        else if(Led_Cnt==1){    
          Blue_Led_Off();
          Red_Led_Off();
          Orange_Led_On();
        }
        else if(Led_Cnt==2){
          Blue_Led_Off();
          Red_Led_On();
          Orange_Led_Off();
        }
        else{Led_Cnt=0;}
        
        if(TimeWindowLed<1000){break;}
        TimeWindowLed=0;           
        if(++Led_Cnt>2){Led_Cnt=0;}
        break;
        
      case SensorStatus:
        if(SensorLedStatus==Blue){
          Blue_Led_On();
          Red_Led_Off();
          Orange_Led_Off();
        }
        else if(SensorLedStatus==Orange){
          Blue_Led_Off();
          Red_Led_Off();
          Orange_Led_On();          
        }
        else if(SensorLedStatus==Red){
          Blue_Led_Off();
          Red_Led_On();
          Orange_Led_Off();          
        }
        break;
        
      case FanError:        
        break;
        
      case Er:
        Red_Led_On();
        Blue_Led_Off();
        Orange_Led_Off();  
        break;
      case Clr:
        Led_Cnt=0;TimeWindowLed=0;WindowMode=ToggleLed;
        break;  
    }
  }
  else{
    ClrWindowLed();             
  }
}

// LED 판정 ( 샘플링 타임:0.1sec, 샘플링 횟수 :  SamplingNumber, 공기질 판정시간: 샘플링 타임 * 샘플링 횟수 * AveTest)
uint8_t AveTest=30;  
uint8_t SamplingNumber=10;
uint16_t Max = 0;
uint16_t Min = 13000;
uint16_t AveMax = 0;
uint16_t AveMin = 13000;
uint16_t consPwm = 125;
uint32_t TimeUs,TimeMs,TimeS;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{ 
  if(htim->Instance == htim6.Instance){ // Tim 6   
    Buz_Toggle();
  }
  if(htim->Instance == htim7.Instance){ // Tim 7(ms)  
    ++Time_Key_Delay;++Time_Key;
    ++Time_Buz;++Time_I2C;++TimeWindowLed;
    ++Time_Fan;++Time_FanLed;
    ++Time_CheckWrite;
    ++Time_Uart;
    if(++Time_Read>20000){Time_Read=20000;}
    if(++Time_FanPWM>consPwm){Time_FanPWM=consPwm;}
    if(++Time_Ionizer>30000){Time_Ionizer=30000;} 
    if(++Time_90D>10000){Time_90D=10000;} 
    if(++Time_Er_LED>2000){Time_Er_LED=2000;}
    if(Mode==Normal_5min|Mode==Normal_30sec){ // 샘플링된 센서 값의  Max와 Min 값 제거 후 평균
      if(++Adc.Cnt>100){
        Adc.Cnt=0; 
        Max = Max > aADCxConvertedData[0] ? Max : aADCxConvertedData[0];
        Min = Min < aADCxConvertedData[0]? Min : aADCxConvertedData[0]; 
        Adc.Sum[0]+=aADCxConvertedData[0];    
        if(++Adc.Sample>=SamplingNumber){
          Adc.Sample=0;
          Adc.Sum[0] = Adc.Sum[0] -  Max - Min;
          Max = 0;
          Min = 10000;
          Adc.Ave[0]=Adc.Sum[0]/(SamplingNumber-2);  
          Adc.Sum[0]=0; 
          Flag.UartAdc=1;
          Adc.AveSum+=Adc.Ave[0];
          AveMax = AveMax > Adc.Ave[0] ? AveMax : Adc.Ave[0];
          AveMin = AveMin< Adc.Ave[0]? AveMin: Adc.Ave[0]; 
          if(++Adc.AveCnt>=AveTest){
            Adc.AveCnt=0;
            Adc.AveReal=(Adc.AveSum-AveMax -AveMin)/(AveTest-2);
            AveMax = 0;
            AveMin  = 10000;
            Adc.AveSum=0;
            Flag.Ave=1;
          }          
        }      
      }
    }
    else{
      Adc.AveCnt=0;
      Adc.Cnt=0;
      Adc.Ave[0]=0;
      Adc.Sum[0]=0;
    }
    if(Mode==Normal_5min|Mode==Normal_30sec){ // Time Pulse 1초
      if(++Cov_Cnt>1000){
        Cov_Cnt=0;
        ++Time_Pulse;
      }
    }
    /* ADC 테스트 */
    if(++Adc.TestCnt>100){
      Adc.TestCnt=0;
      Adc.TestSum[0]+=aADCxConvertedData[0];    
      if(++Adc.TestSample>=SamplingNumber){
        Adc.TestSample=0;       
        Adc.TestAve[0]=Adc.TestSum[0]/SamplingNumber;
        Adc.TestSum[0]=0;      
      }      
    }
  }
}

uint16_t b;
uint8_t buffer[200]={0};
void HAL_SYSTICK_Callback(void)
{ 
  //++cnt_test;
  if(++Test_Time>60){Test_Time=0;++Time_Status;}
  
  //if(Flag.Error || !Power || !Flag.I2CRun ){return;}
  if(Flag.Error || !Power || !Flag.I2CRun || WindowMode == E2PWriteCheckEr || WindowMode == E2PReadCheckEr){return;}
  if(E2PTime.day>=90){return;}
  if(++Time.s>=60){
    Time.s=0;
    if(++Time.min>=60){
      Time.min=0;
      if(++Time.hour>=24){                  
        Time.hour=0;
        if(++Time.day>=90){
             
        }
      }                      
    }                  
  }
  if(++E2PTime.s>=60){                  //저장 얼마마다 한번씩 할껀지 정할것!!! 아마 1분마다 할듯
    E2PTime.s=0;
    if(++E2PTime.min>=60){
      E2PTime.min=0;
      if(++E2PTime.hour>=24){                  
        E2PTime.hour=0;
        if(++E2PTime.day>=90){
          //Flag.Fan=0;
          //Flag.Ionizer=0;
          //여기에 추가 카트리지 90일 지나면 동작 추가 할 것
        }
      }                      
    }    
    SaveTime[0]=E2PTime.s;
    SaveTime[1]=E2PTime.min;
    SaveTime[2]=E2PTime.hour;
    SaveTime[3]=E2PTime.day;
    if(E2PTime.min==0||E2PTime.min==5||E2PTime.min==10||E2PTime.min==15||E2PTime.min==20||E2PTime.min==25||E2PTime.min==30||E2PTime.min==35||E2PTime.min==40||E2PTime.min==45||E2PTime.min==50||E2PTime.min==55 ){Flag.Write=1;}
    //1분마다일 경우 여기에 저장 & 계속 분,시,날짜를 저장하면 프로그램에 노드가 많이 걸리니깐 DMA가 따로 동작하는지 확인해볼것
  }
}
uint8_t E2PReadCheck_Cnt,E2PWriteCheck_Cnt = 0;
uint8_t write_mode;
int ttt,tt1;
void DoE2P(void)
{
  // 재 검사하는 프로그램 추가해야됨. DMA로 I2C 읽는 프로그램,쓰는 프로그램 추가해서 프로그램 로드 걸리는 일을 없애야됨.
  if(Flag.Read){
    if(Time_Read<100){return;}    //E2PRom 보드가 연결 후 일정 시간 이후에 Read.
    I2C_Mem_Read(0xa0,MemAdd,Read_data,4); 
    I2C_Mem_Read(0xa0,MemAdd,Read_data_1,4); 
    I2C_Mem_Read(0xa0,MemAdd,Read_data_2,4); 
    if(ttt){Read_data_2[0]=5;}// 테스트 소스
    if(Read_data[0]==Read_data_1[0] && Read_data[0]==Read_data_2[0] && Read_data[1]==Read_data_1[1] && Read_data[1]==Read_data_2[1] && Read_data[2]==Read_data_1[2]&& Read_data[2]==Read_data_2[2]){
      
      if( (!(Read_data[0] == 255 || Read_data[0] == 127)   && (Read_data[0]>60) && (Read_data[0]<255)) 
          || (!(Read_data[1] == 255 || (Read_data[1] == 127)) && (Read_data[1]>60) && (Read_data[1]<255)) 
          || (!(Read_data[2] == 255 || (Read_data[2] == 127)) && (Read_data[2]>24) && (Read_data[2]<255))
          || (!(Read_data[3] == 255 || (Read_data[3] == 127)) && (Read_data[3]>90) && (Read_data[3]<255))){ // 비정상 데이터
            ++E2PReadCheck_Cnt;
           //  Flag.Read=1; 정상데이터가 아닐경우 다시 한번 읽을 수 있도록 처리, 중복되므로 제거
      }
      else{ // 정상데이터
        Flag.Read=0;
        E2PTime.s = ( Read_data[0] == 255 || Read_data[0] == 127 ) ? 0 : Read_data[0]; 
        E2PTime.min = ( Read_data[1] == 255 || Read_data[1] == 127 ) ? 0 : Read_data[1];
        E2PTime.hour = ( Read_data[2] == 255 || Read_data[2] == 127 ) ? 0 : Read_data[2];
        E2PTime.day = ( Read_data[3] == 255 || Read_data[3] == 127 ) ? 0 : Read_data[3];
        Flag.I2CRun=1;
        E2PReadCheck_Cnt=0;
      }
      //ClrTime();                        //5초? 뒤에 제대로 확인되면 현제 시간도 같이 초기화해서 동작
    } 
    else{++E2PReadCheck_Cnt;}                                                   //읽은 2개의 데이터가 
    if(E2PReadCheck_Cnt>50){
      if(Power){
        WindowMode=E2PReadCheckEr;
        ClrTime();E2PReadCheck_Cnt=0;
      }
    }
  }
  else{
    Flag.Read=0;Time_Read=0;
  }
  if(Flag.Write && Flag.I2CRun && Power){
    switch(write_mode){
      case 0:
        I2C_Mem_Write(0xa0,MemAdd,SaveTime,4);
        Time_CheckWrite=0;
        ++write_mode;
        break;
      case 1:
        if(Time_CheckWrite>50){++write_mode;}
        break;
      case 2:
        I2C_Mem_Read(0xa0,MemAdd,SaveRead,4); 
        ++write_mode;
        break;
      case 3:
        if(tt1){SaveRead[1]=5;}
        if(SaveRead[1]==SaveTime[1] && SaveRead[2]==SaveTime[2] && SaveRead[3]==SaveTime[3]){
          Flag.Write=0;E2PWriteCheck_Cnt=0;Flag.UartSaveRead=1;
        }
        else{++E2PWriteCheck_Cnt;}  //저장한 데이터와 다시 읽은 데이터가 다를 경우 카운트 증가
        write_mode=0;
        break;
      default:
        write_mode=0;
        break;
    }                                                    
    if(E2PWriteCheck_Cnt>50){                                                     //10번 잘못 되었을 경우 에러 표시 LED 1초 ON/1초 OFF      
      if(Power){
        WindowMode = E2PWriteCheckEr;
        Flag.Write=0;ClrTime();E2PWriteCheck_Cnt=0;
      }
    }
  }
  else{Time_CheckWrite=0;}
}
void DoUart()
{
  if(Time_Uart>100){
    Time_Uart=0;
     if(Flag.UartSaveRead){
       sprintf(buffer,"SaveTimeMin= %.2d SaveTimeHour= %2d SaveTimeDay=%2d\r\n",SaveRead[1],SaveRead[2],SaveRead[3]);
       HAL_UART_Transmit_DMA(&huart1,buffer,strlen(buffer));
       Flag.UartSaveRead=0;
    }
    else{
      if(Flag.UartAdc){
        Adc.V2=(V3_3*Adc.TestAve[0])/4096;           //1.66V?
        Adc.V1=(Adc.V2*R1)/R2;                        //3.38V-1.66V -> 1.72V
        Adc.VRL=Adc.V1+Adc.V2;                             //3.38V
        Adc.Rs=(int)(R1+R2)*(V5/Adc.VRL-1);
        
        if(!Flag.Fan && Flag.Error &&Power){
           sprintf(buffer,"Volt5= %.4f Rs5= %4d FanEr!! ErCheck=%d\r\n", Adc.V2,Adc.Rs,ErCheck.CheckNum);         
        }
        else if(!Flag.I2C && Flag.Error){
           sprintf(buffer,"Volt5= %.4f Rs5= %4d E2PEr!! ErCheck=%d\r\n", Adc.V2,Adc.Rs,ErCheck.CheckNum);         
        }
        else{
            if(Mode==Normal_30sec){
              if(SensorLedStatus==Blue){
                sprintf(buffer,"Volt5= %.4f Rs5= %4d RsCur5=%4d RsAir5=%4d LED5=B slope=%.4f buffer=%4d Pulse=%3d ErCheck=%d\r\n",Adc.V2,Adc.Rs,Adc.RsCur,Adc.RsAirCur, (float)Adc.RsCur/(float)Adc.RsAirCur,Adc.RsBufCur,Pulse,ErCheck.CheckNum);       //테스트 하는 모듈 번호를 Volt8과 Rs8에 수정
              }
              else if(SensorLedStatus==Orange){
                sprintf(buffer,"Volt5= %.4f Rs5= %4d RsCur5=%4d RsAir5=%4d LED5=O slope=%.4f buffer=%4d Pulse=%3d ErCheck=%d\r\n",Adc.V2,Adc.Rs,Adc.RsCur,Adc.RsAirCur,(float)Adc.RsCur/(float)Adc.RsAirCur,Adc.RsBufCur,Pulse,ErCheck.CheckNum);       //테스트 하는 모듈 번호를 Volt8과 Rs8에 수정
              }
              else if(SensorLedStatus==Red){
                sprintf(buffer,"Volt5= %.4f Rs5= %4d RsCur5=%4d RsAir5=%4d LED5=R slope=%.4f buffer=%4d Pulse=%3d ErCheck=%d\r\n",Adc.V2,Adc.Rs,Adc.RsCur,Adc.RsAirCur,(float)Adc.RsCur/(float)Adc.RsAirCur,Adc.RsBufCur,Pulse,ErCheck.CheckNum);       //테스트 하는 모듈 번호를 Volt8과 Rs8에 수정
              }          
            }
            else{
              sprintf(buffer,"Volt5= %.4f Rs5= %4d RsCur5=0 RsAir5=0 LED5=N ErCheck=%d\r\n",Adc.V2,Adc.Rs,ErCheck.CheckNum);                  //테스트 하는 모듈 번호를 Volt8과 Rs8에 수정
            }
        }
        HAL_UART_Transmit_DMA(&huart1,buffer,strlen(buffer));
        Flag.UartAdc=0;
      }
    }  
  }
}
#define StartAddr ((unsigned int)0x0801F800)
uint8_t test;
uint8_t Flash;
void DoFlashE2P()
{
  if(Flag.FlashE2P){
    Flag.FlashE2P=0;
    HAL_FLASH_Unlock();

    FLASH_PageErase(0x0801F800);
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F800, Flag.Memory);
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F804, Flag.WindowLed);   
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F808, Flag.Cartridge);   
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F80C, Power);    
    HAL_FLASH_Lock();
  }
  if(Flag.FlashClr){
    Flag.FlashClr=0;
    HAL_FLASH_Unlock();

    FLASH_PageErase(0x0801F800);
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F800, 0);
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F804, 0);   
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F808, 0);   
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801F80C, 0);   
    HAL_FLASH_Lock();
  }
}
void DoErrorCheck()
{
  uint8_t buf;
  if(ErCheck.WriteFlag && ErCheck.WriteNum){            //라이팅 하고 싶은 데이터값을 ErCheck.WriteNum에 넣고 ErCheck.WriteFlag을 1로 셋팅해야 들어가짐. 0으로 쓰고 싶으면 ErCheck.ClrFlag를 1로 셋팅하면됨.
      HAL_FLASH_Unlock();
      FLASH_PageErase(0x0801fff0);
      CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
      HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801fff0, ErCheck.WriteNum);   
      HAL_FLASH_Lock();
      buf=*(uint32_t*) 0x0801fff0;
      if(buf==ErCheck.WriteNum){ErCheck.WriteFlag=0;ErCheck.WriteNum=0;}
  }
  if(ErCheck.ReadFlag){                                         //리딩 테스트 함수 ErCheck.ReadFlag을 1로 하면 ErCheck.ReadNum에 값이 들어옴.
      ErCheck.ReadNum = *(uint32_t*) 0x0801fff0;
      buf=*(uint32_t*) 0x0801fff0;
      if(buf==ErCheck.ReadNum){ErCheck.ReadFlag=0;}
  }  
  if(ErCheck.ClrFlag){
    HAL_FLASH_Unlock();
    FLASH_PageErase(0x0801fff0);
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x0801fff0, 0); 
    HAL_FLASH_Lock();
    buf= *(uint32_t*) 0x0801fff0;
    if(buf==0){ErCheck.ClrFlag=0;}
  }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq());
  HAL_TIM_Base_Start_IT(&htim7);
  if (HAL_ADC_Start_DMA(&hadc,
                      (uint32_t *)aADCxConvertedData,
                      ADC_CONVERTED_DATA_BUFFER_SIZE
                     ) != HAL_OK)
  {
    Error_Handler();
  }
  /////////변수 테스트 삭제.
  
  Flag.Buz=1; 
  
  Flag.Memory = *(uint32_t*) 0x0801F800;
  if(Flag.Memory==0xfe){Flag.Memory=1;}
  else if(Flag.Memory==0xff){Flag.Memory=0;}
  if(Flag.Memory){
    Flag.WindowLed = *(uint32_t*) 0x0801F804;
    Flag.Cartridge = *(uint32_t*) 0x0801F808;
    Power = *(uint32_t*) 0x0801F80C;
    if(Power){
      Flag.Fan=1;
      Flag.Ionizer=1;
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////버전 추가
  //sprintf(buffer,"0812크래누보 버젼  \r\n");        
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    DoSensor();
    DoCartridge();
    DoBuz();
    DoKey();
    DoI2C();
    DoWindowLed();
    DoIonizer();
    DoI2CTest();
    DoMode();
    DoFan();
    DoE2P();
    //DoUart();
    DoFlashE2P();
    DoErrorCheck(); 
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_71CYCLES_5;  //이거 추가해야 동작함....왜??
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 8-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 180-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 8-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000-1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 2-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 400-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel4_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA5 PA6
                           PA7 PA8 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB10 PB11 PB12
                           PB13 PB14 PB15 PB6
                           PB7 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
