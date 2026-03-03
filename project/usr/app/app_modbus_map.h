/**
 * @file    app_modbus_map.h
 * @author  Dylan
 * @date    2026-03-02
 * @brief   Modbus寄存器映射应用层接口
 */

#ifndef APP_MODBUS_MAP_H
#define APP_MODBUS_MAP_H

#include <stdint.h>
#include "app_digital_sample.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   更新Modbus保持寄存器映射
 *
 * @param[in]   regs  保持寄存器数组指针
 * @param[in]   Data  采集数据结构体指针
 *
 * @return  None
 */
void app_modbus_update_regs(uint16_t *regs, Data_t *Data);

#ifdef __cplusplus
}
#endif

#endif /* APP_MODBUS_MAP_H */
