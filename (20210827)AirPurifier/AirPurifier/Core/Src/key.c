#include "main.h"

struct _Key Key;

void KeyRead(void)
{
  if(Time_Key_Delay<10){return;}
  Time_Key_Delay=0;
  Key.Read =  0;
  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)){Key.Read =  1;}
}

void KeyCheck(void)
{
  switch(Key.Step){
    case 0 :  
      if(!Key.Read)break;
      Time_Key = 0;  
      ++Key.Step;       
      break;

    case 1:
      if(!Key.Read){
        Key.Step = 0;
        break;
      }    
      if(Time_Key<100){break;}         //파워 off에서 키 눌리는 시간을 바꾸려면 이곳 수정 1)
      Time_Key = 0;      
      Key.Step = 2;
      if(!Power){
        Key.Flag = 1;
        Key.Buf = ShortKey;
        Key.Step=4;
      }      
      break;          

    case 2:
      if(!Key.Read){
        Key.Step = 0; 
        Key.Flag = 1;
        Key.Buf = ShortKey;
        Time_Key = 0; 
        break;
      }
      if(Time_Key<2900){break;}         //2) 롱키 1)+2)으로 수정 예를 들어 롱키 3로초 만들고 싶을 경우 1)+2)인 0.1초+2.9초
      Time_Key = 0; 
      Key.Buf = LongKey;
      Key.Flag = 1;    
      Key.Step = 3;
      break;

    case 3:  
      if(Key.Read)break;       
      Key.Step = 0;
      break;   
    case 4:
      if(!Key.Read){
        Key.Step = 0; 
        break;
      }
      break;
    
    default :
      Key.Step = 0;
      break;
    }
}


void KeyAction(void)
{       
  Flag.Buz=1;
  if(Power&&Key.Buf == LongKey){                       //Power On과 LED Off 모드 일 경우에 5초 아래로 누르면 동작 확인하고 수정해야할것
    if(Flag.WindowLed && Flag.I2C==0){
      Flag.Memory^=1;      
      BuzMode=5;
      Flag.FlashE2P=1;
      return;
    }       
    Flag.WindowLed^=1;
    Flag.Cartridge^=1;
    if(Flag.Memory){
      Flag.FlashE2P=1;
    }
    return;
  }
  Power^=1;
  if(Power){
    Flag.WindowLed=1;
    Flag.Cartridge=1;
    Flag.Fan=1;
    Flag.Ionizer=1;
    WindowMode= Clr;            //LED관련 변수들 초기화 나중에 바꿔도 됨 일단 편하게 만들어 놓음
  }
  else{
    if(WindowMode == E2PWriteCheckEr || WindowMode == E2PReadCheckEr){
      Flag.Read=1;
    }
    WindowMode=0;
    ClrTime();
    Flag.Error=0;
    Flag.Ionizer=0;
    Flag.Fan=0;
    Flag.WindowLed=0;
    Flag.Cartridge=0;
  }
  if(Flag.Memory){
    Flag.FlashE2P=1;
  }
}

void KeyCheckAction(void)
{
  if(!Key.Flag)return;
  Key.Flag = 0;  
  KeyAction(); 
}

void DoKey(void)
{  
  KeyRead();
  KeyCheck();
  KeyCheckAction();
}