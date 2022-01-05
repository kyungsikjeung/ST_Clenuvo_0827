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
      if(Time_Key<100){break;}         //�Ŀ� off���� Ű ������ �ð��� �ٲٷ��� �̰� ���� 1)
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
      if(Time_Key<2900){break;}         //2) ��Ű 1)+2)���� ���� ���� ��� ��Ű 3���� ����� ���� ��� 1)+2)�� 0.1��+2.9��
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
  if(Power&&Key.Buf == LongKey){                       //Power On�� LED Off ��� �� ��쿡 5�� �Ʒ��� ������ ���� Ȯ���ϰ� �����ؾ��Ұ�
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
    WindowMode= Clr;            //LED���� ������ �ʱ�ȭ ���߿� �ٲ㵵 �� �ϴ� ���ϰ� ����� ����
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