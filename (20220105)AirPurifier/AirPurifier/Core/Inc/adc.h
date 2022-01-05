extern void DoSensor(void);
struct _Adc{
  uint16_t Cnt;
  uint8_t Flag;
  uint16_t Sample;
  uint32_t Sum[1];
  uint16_t Ave[1];
  uint16_t Rs;
  float V2;
  float V1;
  float VRL;
  int16_t RsBuf;
  int16_t RsAir;
  
  float Result;
  uint16_t AveCnt; 
  uint32_t AveSum;
  uint16_t AveReal;
  float V2Ref;
  
  float V2Cur;
  float V1Cur;
  float VRLCur;
  int16_t RsBufCur;
  int16_t RsAirCur;
  int16_t RsCur;
    
  uint16_t TestCnt;
  uint32_t TestSum[1];
  uint16_t TestAve[1];
  uint16_t TestSample;
};
extern volatile struct _Adc Adc;
extern uint16_t Time_Pulse;

//#define SampleNum 50

#define R1 3600         //PCB에 연결된 저항값
#define R2 6800         //PCB에 연결된 저항값


#define V5 4.982        //PCB에서 나오는 5V의 실제 전압        
#define Pulse 80
#define V3_3 3.303        //PCB에서 나오는 3.3V의 실제 전압
