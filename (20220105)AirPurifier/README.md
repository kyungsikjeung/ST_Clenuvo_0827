# 수정 사항(2021-08-27)
1. 카트리지 접촉 불량(Read/Write)에러시 파워 LED 빨간색으로 변경
2. 정상모드에서 팬 100%로 변경(김민규 과장님이 보내주신 팬 PWM 은 나이트 모드에만 적용)
3. 10min 이 없는 부분 수정
4. 그이외 07/29 버젼과 동일

# 수정 사항(2021-07-06)
1. 정상모드에서 카트리지를 뺐다가 다시 꼽을 경우 혹은 초기 전원 인가시 연결되어 있는 EEPROM 을 3번 읽어서 같은 데이터인지 확인, 만약 10번 확인 후 에러면 오렌지색 LED를 0.5초마다 깜빡인다.
2. E2PTime의 초 변수가 0이될 때(1분마다) EEPROM에 데이터를 쓰게 된다. 만약 저장한 데이터와 읽은 데이터가 다르면 10번 확인 후 에러면 파란색 LED를 0.5초마다 깜빡인다.
3. EEPROM 시간이 90일이 지날 경우 적색 LED ON, FAN OFF, IONIZER OFF, 카트리지 LED 점멸.
4. 90일 지난 후 카트리지를 다시 연결 시 Time(전원 스위치 누른 이후 동작하는 시간) 초기화? -> 타임 초기화. 카트리지 날짜 데이터 읽어서 LED 표시, FAN ON, IONIZER ON
5. Power Off 일 때 리드 에러 띄울 것인가??? 띄울 경우 파워 변수 부분 잘 체크 해야됨. -> power on 시 에러 띄움.
6. EEPROM 리드 에러, 라이트 에러 시 파워 버튼을 누르면 파워가 꺼지고 다시 한번 누르면 전원 ON, FAN ON, IONIZER ON, 상태 표시 LED ON 시간은 초기화
7. EEPROM 리드 에러, 라이트 에러 시 카트리지를 뺐다가 다시 꼽을 경우에 전부 초기화 하고 EEPROM 상태 읽어서 재 동작. Time도 초기화.

# 메모리 모드, PWM 변경.

# 수정 사항(2021-06-21)
소프트 웨어 I2C로 변경 카트리지 연결 후 0.1초만에 인식 후 EEPROM Read
EEPROM Write 시간 5ms 대기로 인한 루틴 수정. 메모리 주소 256인부분 255로 수정.

## 이력관리
1. 10회 읽고 쓴 데이터가 다를 시 Red 깜박임에서 Blue 깜박임 변경
2. 메모리 모드(0x1a) 에서 새 IDCHIP 127에서 초기화
3. PWM 제어 방식 변경  
- frequency 변경(2000->125)
- 스피드 변경 watchdog에서 할수있도록 전역변수
- 메모리 모드


## 변경소스  1 (메모리 주소 초기화, DoE2P 함수)


      if( !(Read_data[0] == 255 || Read_data[0] == 127)   && Read_data[0]>60 && Read_data[0]<256 
          || !(Read_data[1] == 255 || Read_data[1] == 127) && Read_data[1]>60 && Read_data[1]<256 
          || !(Read_data[2] == 255 || Read_data[2] == 127) && Read_data[2]>24 && Read_data[2]<256
          || !(Read_data[3] == 255 || Read_data[3] == 127) && Read_data[3]>90 && Read_data[3]<256){ // 비정상 데이터
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


## 변경소스 2(PWM, DoFan 함수:나이트 모드 소음)
- 1. 기존의 팬스피드 70프로를 FanOnTime과 Fan OffTime으로 제어(7대10비율로 조정)
- 2. PWM On/OFF 주기 빠르게 변경 (Time_FanPWM 125로 변경)
- 3. Fan 이 규칙적인 멈춘 소리가 들려 Sinewave로 PWM 제어하도록 코드 변경. Acuastic noise 제거. 
- ON 7초 Off 3초 , ON 8초 Off 2초, ON 7초 Off 3초, ON 6초 Off 4초
- TODO: 잠잘떄 들리는 알파 웨이브는  8–12 Hz
- 8Hz = 0.125초 , 11Hz = 0.09초 
- On+Off 주기를 0.09초에 들리도록
- 0.09초/(20초_PWMONOFF) = 0.0045초_PWMTIME
- 1초:1000 = 0.0045초 : PWMTIME => PWMTIME ~= 4.5

uint8_t FanOnTime = 14; 
uint8_t FanOffTime = 20; // 팬 on+off 시간

int8_t sign = 1; 
int8_t max  = 2;
int8_t alpha = 0;

      
      if(Time_FanPWM<FanOnTime+alpha){         //Fan 스피드 부분 FanSpeed를 60으로 바꾸면 나이트 모드일 경우에 60퍼 속도로 돔.
        Fan_On();
      }
      else if(Time_FanPWM<FanOffTime){
        Fan_Off();
        if(alpha>max-1 || alpha<-(max-1) ){
          sign = alpha <max ? 1 : -1;
        }
        alpha = sign > 0 ? alpha+1 : alpha -1;
      }


## 변경소스 2 Unit Test(PWM, DoFan 함수)
var a = []; <br />
var sign = 1; <br />
var max = 7; <br />
var alpha = 0; <br />
 for(var i =0; i<100;i++){ <br />
 if(alpha > max-1 || alpha < -(max-1)){ sign = alpha < max ? 1 : -1 } alpha = sign > 0 ? alpha+1 : alpha-1; a.push(alpha) } <br />
"-6에서 6까지 계속 진동하는 웨이브" <br />
a <br />

위의 소스 복붙하기.
링크 : https://stephengrider.github.io/JSPlaygrounds/


