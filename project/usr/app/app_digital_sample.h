/**
 * @file    app_digital_sample.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   数字板采样应用层接口
 */
#ifndef APP_DIGITAL_SAMPLE_H
#define APP_DIGITAL_SAMPLE_H

#include "cmsis_os2.h"

#include "stdint.h"

#include "board.h"
#include "drv_adc.h"


#include "printf.h"




#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint16_t CL;     // 诊断电流
    uint16_t CH;     // 诊断电流
    uint16_t PI;     // 入口压力
    uint16_t PD;     // 出口压力
    uint16_t TI;     // 入口温度
    uint16_t TM;     // 电机温度
    uint16_t VX;     // X轴振动
    uint16_t VY;     // Y轴振动
    uint16_t CLA;    // 漏电流
    uint16_t VoltY;  // 星点电压
}Data_t;


void app_digital_sample_volty(Data_t *Data);



#ifdef __cplusplus
}
#endif /* APP_DIGITAL_SAMPLE_H */

#endif

