#ifndef BSP_H 
#define BSP_H 
 
// 底层Hal库
#include "stm32h7xx_hal.h"

// 板级驱动库
#include "bsp_uart.h"
#include "bsp_adc.h"
#include "bsp_adcfilter.h"
#include "bsp_gpio.h"

// 中间件
#include "cmsis_os2.h"
#include "printf.h"

// C语言库 
#include <stdio.h>
#include <string.h>

 
#ifdef __cplusplus
extern "C"{
#endif
 



void BSP_Init(void);




#ifdef __cplusplus
}
#endif


#endif	/* BSP_H */




