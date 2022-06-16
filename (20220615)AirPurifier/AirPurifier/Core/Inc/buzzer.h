#define Buz_On() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET)        
#define Buz_Off() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET)
#define Buz_Toggle() HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_10)