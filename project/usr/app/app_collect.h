/**
 * @file    main.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   基于CMSIS-RTOS2的应用入口
 *
 * @details 复用驱动与设备层，实现LED闪烁、串口回显和ADC采样任务。
 *          演示环形缓冲区在串口接收中的应用。
 */
#ifndef APP_COLLECT_H
#define APP_COLLECT_H


#include "stdint.h"

#include "board.h"
#include "drv_adc.h"


#include "printf.h"




#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint16_t VoltY; // 星点电压

}Data_t;


void app_collect_VoltY(Data_t *Data);



#ifdef __cplusplus
}
#endif

#endif