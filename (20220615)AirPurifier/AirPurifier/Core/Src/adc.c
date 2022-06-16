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
  /*  버퍼 영역 갱신 
  * 1. 평균값이 버퍼지역에서 벗어날 떄 버퍼 지역 중심값 평균값으로 업데이트
  * 2. 평균값이 버퍼내부에 있을떄 안정적인 공기는 버퍼 업데이트, 불안정한 공기는 업데이트 x
  * @Param : PrevSensorLedStatus 센서 이전값, Blue에서 RED로 변하지 않고 한단계씩 변하도록 이전 상태 기억
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
    
  /*  흡연실 환경 
  * 1. 공기 정화 시 : 공기가 가장 꺠끗한 지점으로 기준값 변경
  * 2. 흡연실 환경에서는 LED를 RED, Orange만 가능하도록 변경
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
      /* 커스터마이징 끝 */
        
    }
  /*  작업장  환경 
  * 1. 공기 정화 시 : 공기가 가장 꺠끗한 지점으로 기준값 변경
  * 2. 작업장 환경은 LED는 Blue , Orange 만 보이도록 변경
  * 3. 예외 :  2.8V 이하 상태에서 작업장 환경으로 진입 시 RED 가 보일 수 있도록 프로그램 수정
  * @Param : PrevStage PreStage 는 2.8V 이하 2.8~3V , 3V 이상 3단계로 나뉘어짐. 2.8V이하에서 공기가 오염되어 2.8V 이상시 빨간색이 안보이는 이슈 해결하기 위함. 
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
    else{ // 2.8 V 이하
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
