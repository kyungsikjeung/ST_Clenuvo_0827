#define MemAdd		0x1a
#define SDA_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
#define SDA_LOW() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
#define SCL_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
#define SCL_LOW() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
#define nop()   __asm volatile("NOP");
extern void I2C_Mem_Read(unsigned char DevAddress,unsigned char MemAddress,unsigned char *pData,unsigned char Size);
extern void I2C_Mem_Write(unsigned char DevAddress,unsigned char MemAddress,unsigned char *pData,unsigned char Size);
