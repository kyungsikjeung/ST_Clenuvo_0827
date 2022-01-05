#include "main.h"

void i2c_init(void) {
    GPIOB->MODER = (GPIOB->MODER | 0x00005000);
    SDA_HIGH();SCL_HIGH();
}
void i2c_start(void) { //start
    GPIOB->MODER = (GPIOB->MODER | 0x00005000);		//INPUT
    
    SDA_HIGH();SCL_HIGH(); // Set SCL, SDA High
    nop();nop(); SDA_LOW(); // Clear SDA
    nop();nop(); SCL_LOW(); // Clear SCL
    nop();nop();
}
void i2c_stop(void) {
    GPIOB->MODER = (GPIOB->MODER | 0x00005000);
    SDA_LOW(); // Clear SDA Low
    nop();nop(); SCL_HIGH(); // Set SCL High
    nop();nop(); SDA_HIGH(); // Set SDA High
}
void write_i2c_byte(unsigned char byte)
{
    unsigned char i = 0;
    GPIOB->MODER = (GPIOB->MODER | 0x00004000);

    for (i = 0; i < 8 ; i++) {
      if((byte & 0x80)==0x80){ SDA_HIGH();} // Set SDA High
      else{     SDA_LOW();} // Clear SDA Low
      nop();nop();nop();
      SCL_HIGH(); // Set SCL High, Clock data
      nop();nop();nop(); byte = byte << 1; // Shift data in buffer right one
      SCL_LOW(); // Clear SCL
      nop();nop();nop();
    }
    GPIOB->MODER = (GPIOB->MODER & 0xffff3fff);
    nop();nop();
    SCL_LOW();
    nop();nop();
    SCL_HIGH(); nop();nop();SCL_LOW();
    GPIOB->MODER = (GPIOB->MODER | 0x00004000);
}
unsigned char read_i2c_byte(unsigned char ch)
{
  unsigned char i;
  char buff=0;
  GPIOB->MODER = (GPIOB->MODER & 0xffff3fff);
  nop();nop();nop();nop();	
  for(i=0; i<8; i++) {
    nop();nop();nop(); SCL_HIGH(); nop();nop();nop();// Set SCL High, Clock bit out
    buff <<= 1; // Read data on SDA pin
    if((GPIOB -> IDR & (1<<7))!=0){       buff |= 0x01;     }
    SCL_LOW(); // Clear SCL
    nop();nop();nop();
  }
  GPIOB->MODER = (GPIOB->MODER | 0x00004000);
  if(ch == 1) //NACK
  {
      SDA_HIGH(); //SDA HIGH.
  }
  else //ACK.
  {
      SDA_LOW(); //SDA LOW.
  }
  nop();nop();
  SCL_HIGH(); nop();nop();nop(); SCL_LOW(); //SCL LOW.
  SDA_HIGH(); //SDA HIGH.
  nop();nop();nop();
  return buff;
}
void I2C_Mem_Write(unsigned char DevAddress,unsigned char MemAddress,unsigned char *pData,unsigned char Size)		//변수 사이즈 주의
{
  char i;

  i2c_start();
  write_i2c_byte(DevAddress); //write + Device add
  write_i2c_byte(MemAddress);

  for(i=0;i<Size;i++){
    write_i2c_byte(*(pData+i));
  }
  i2c_stop();
}
void I2C_Mem_Read(unsigned char DevAddress,unsigned char MemAddress,unsigned char *pData,unsigned char Size)		//변수 사이즈 주의
{
  char i;
  i2c_start();
  write_i2c_byte(DevAddress);
  write_i2c_byte(MemAddress);
  i2c_start();
  write_i2c_byte(DevAddress+1);  
  for(i=0;i<Size;i++){
    *(pData+i)=read_i2c_byte(0);
  }
  i2c_stop();
}