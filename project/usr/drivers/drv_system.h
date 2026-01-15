/**
 * @file drv_system.h
 * @brief 系统驱动层抽象接口
 * @note 平台无关的系统初始化和错误处理接口定义
 */

#ifndef DRV_SYSTEM_H
#define DRV_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* 返回值约定: 0 表示成功, 负值表示失败 */
#ifndef DRV_OK
#define DRV_OK          0
#define DRV_ERROR      -1
#endif

/**
 * @brief 系统初始化（HAL_Init + 时钟配置）
 * @return 0 成功, 负值失败
 */
int DRV_System_Init(void);

/**
 * @brief 系统错误处理
 * @note 此函数不返回，进入无限循环
 */
void DRV_System_ErrorHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* DRV_SYSTEM_H */
