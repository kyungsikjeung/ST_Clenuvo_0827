#include "main.h"
volatile struct _Adc Adc;

uint8_t mode;
uint16_t Time_Pulse;    
uint8_t PrevSensorLedStatus = Blue; 
uint8_t PrevStage = None;
uint16_t OrangeCnt = 0;
uint16_t BlueCnt = 0;
uint16_t RedCnt = 0;


void DoSensor(void)
{
  /*  ���� ���� ���� 
  * 1. ��հ��� ������������ ��� �� ���� ���� �߽ɰ� ��հ����� ������Ʈ
  * 2. ��հ��� ���۳��ο� ������ �������� ����� ���� ������Ʈ, �Ҿ����� ����� ������Ʈ x
  * @Param : PrevSensorLedStatus ���� ������, Blue���� RED�� ������ �ʰ� �Ѵܰ辿 ���ϵ��� ���� ���� ���
  */
  if(Adc.RsCur<Adc.RsBufCur-Pulse || Adc.RsCur>Adc.RsBufCur+Pulse){              
    Time_Pulse=0;
    Adc.RsBufCur=Adc.RsCur;
  }else{ 
    if( (Adc.RsBufCur+Pulse - Adc.RsCur >=0 && Adc.RsBufCur+Pulse - Adc.RsCur <= 65) || (Adc.RsCur - Adc.RsBufCur+Pulse >= 0 && Adc.RsCur - Adc.RsBufCur+Pulse <= 65) ){
      Adc.RsBufCur=Adc.RsCur;
    }
  }
  
  if(Flag.Ave){
    Flag.Ave=0;
    Adc.V2Cur=Adc.AveReal*3.3/4096;
    Adc.V1Cur=(Adc.V2Cur*R1)/R2;                        //3.38V-1.66V -> 1.72V
    Adc.VRLCur=Adc.V1Cur+Adc.V2Cur;                             //3.38V
    if(Adc.VRLCur < 1.8){
      Adc.RsCur=(-2642)*Adc.V2Cur+13244;
    }else{
      Adc.RsCur=(int)(R1+R2)*(V5/Adc.VRLCur-1);
    }
    
  /*  ���� ȯ�� 
  * 1. ���� ��ȭ �� : ���Ⱑ ���� �Ʋ��� �������� ���ذ� ����
  * 2. ���� ȯ�濡���� LED�� RED, Orange�� �����ϵ��� ����
  */ 
    if(Adc.V2Cur>3.0 &&  Mode==Normal_30sec){           
        
      PrevStage = GreaterThanThreePointZero;
        
        if(Adc.RsCur >Adc.RsAirCur){   
          Adc.RsAirCur=Adc.RsCur;
          Time_Pulse=0;
        }
        if(Time_Pulse>=60){         
          Time_Pulse=0;
            Adc.RsAirCur=Adc.RsAirCur-75;
            if(Adc.RsAirCur<=Adc.RsCur){
              Adc.RsAirCur=Adc.RsCur;
            }
        }
        Adc.Result=(float)Adc.RsCur/(float)Adc.RsAirCur;
        
        if(Adc.Result>0.99){
          if(PrevSensorLedStatus == Orange){
            RedCnt = 0;
            OrangeCnt ++;
            BlueCnt = 0;
          }
          if(OrangeCnt >= 60){
            OrangeCnt = 0;
            BlueCnt = 0;
            RedCnt = 0;
            Adc.RsAirCur = Adc.RsCur + 2000;
            SensorLedStatus=Red;
          }else{
            SensorLedStatus=Orange;
          }
        }
        else{
          if(PrevSensorLedStatus == Red){
            OrangeCnt = 0;
            BlueCnt = 0;
            RedCnt ++;
          }
          if(RedCnt >40){
            OrangeCnt = 0;
            BlueCnt = 0;
            RedCnt = 0;
            Adc.RsAirCur = Adc.RsCur;
            SensorLedStatus=Orange;
          }else{
            SensorLedStatus=Red;
          }
        }
         PrevSensorLedStatus = SensorLedStatus; 
      /* Ŀ���͸���¡ �� */
        
    }
  /*  �۾���  ȯ�� 
  * 1. ���� ��ȭ �� : ���Ⱑ ���� �Ʋ��� �������� ���ذ� ����
  * 2. �۾��� ȯ���� LED�� Blue , Orange �� ���̵��� ����
  * 3. ���� :  2.8V ���� ���¿��� �۾��� ȯ������ ���� �� RED �� ���� �� �ֵ��� ���α׷� ����
  * @Param : PrevStage PreStage �� 2.8V ���� 2.8~3V , 3V �̻� 3�ܰ�� ��������. 2.8V���Ͽ��� ���Ⱑ �����Ǿ� 2.8V �̻�� �������� �Ⱥ��̴� �̽� �ذ��ϱ� ����. 
  */ 
    else if(Adc.V2Cur>2.8 &&  Mode==Normal_30sec){      
        if(Adc.RsCur >Adc.RsAirCur){    
          Adc.RsAirCur=Adc.RsCur;
          Time_Pulse=0;
        }
        if(Time_Pulse>=60){         
          Time_Pulse=0;
            Adc.RsAirCur=Adc.RsAirCur-75;
            if(Adc.RsAirCur<=Adc.RsCur){
              Adc.RsAirCur=Adc.RsCur;
            }
        }
        Adc.Result=(float)Adc.RsCur/(float)Adc.RsAirCur;
        
        if(Adc.Result>0.99){
          if(PrevSensorLedStatus == Blue){
              OrangeCnt = 0;
              BlueCnt ++;
              RedCnt  = 0;
           }
          if(OrangeCnt >= 60){
            OrangeCnt = 0;
            BlueCnt = 0;
            RedCnt = 0;
            Adc.RsAirCur = Adc.RsCur + 2000;
            SensorLedStatus=Orange;
          }else{SensorLedStatus=Blue;}
        }
        else{ 
                  if(PrevStage == LessThanTwoPointEight && Adc.Result<0.91 && PrevSensorLedStatus !=Blue ){
                    SensorLedStatus=Red;  
                  }else{
                    if(PrevSensorLedStatus == Orange){
                      OrangeCnt ++;
                      BlueCnt = 0;
                      RedCnt = 0;
                    }
                    if(OrangeCnt>60){
                      OrangeCnt =0;
                      BlueCnt = 0;
                      RedCnt = 0;
                       Adc.RsAirCur=Adc.RsCur;
                       SensorLedStatus=Blue;
                    }else{
                      SensorLedStatus=Orange;
                    }
                    
                  }
        }
        PrevSensorLedStatus = SensorLedStatus; 
    }
    else{ // 2.8 V ����
      PrevStage = LessThanTwoPointEight;
      Time_Status=0;
      BlueCnt = 0;
      if(PrevSensorLedStatus == Orange || PrevSensorLedStatus == Red){
        OrangeCnt = PrevSensorLedStatus == Orange ? OrangeCnt+1 : 0;
        RedCnt = PrevSensorLedStatus  == Red ? RedCnt+1 : 0;
        if(OrangeCnt >100 || RedCnt >90){
          Adc.RsAirCur = OrangeCnt  > RedCnt ? Adc.RsCur : Adc.RsCur/0.95;
          SensorLedStatus = OrangeCnt  > RedCnt  ? Blue : Orange;
          OrangeCnt = 0;
          RedCnt = 0;
        }
      }
      if(Adc.RsCur >Adc.RsAirCur){    
        Adc.RsAirCur=Adc.RsCur;
        Time_Pulse=0;
      }
      if(Time_Pulse>=60){         
        Time_Pulse=0;
          Adc.RsAirCur=Adc.RsAirCur-75;
          if(Adc.RsAirCur<=Adc.RsCur){
            Adc.RsAirCur=Adc.RsCur;
          }
      }
      Adc.Result=(float)Adc.RsCur/(float)Adc.RsAirCur;
      if(Adc.Result>0.99){
        if(PrevSensorLedStatus == Red){
          SensorLedStatus=Orange;  
        }else{
          SensorLedStatus=Blue;  
        }
      }
      else if(Adc.Result>0.91){
          SensorLedStatus=Orange;
      }
      else{ 
        if(PrevSensorLedStatus  == Blue){
          SensorLedStatus=Orange;
        }else{
          SensorLedStatus=Red;
        }
      }
      PrevSensorLedStatus = SensorLedStatus; 
    }   
  }
} 
