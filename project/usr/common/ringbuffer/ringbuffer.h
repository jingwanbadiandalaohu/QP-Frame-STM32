/**
 * @file    ringbuffer.h
 * @author  Dylan
 * @date    2026-01-26
 * @brief   环形缓冲区实现
 *
 * @details 提供线程安全的环形缓冲区，支持字节流和数据块操作
 *          适用于UART、DMA等数据缓冲场景
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief 环形缓冲区结构体
 */
typedef struct
{
  uint8_t  *buffer;     /**< 缓冲区指针 */
  uint32_t size;        /**< 缓冲区大小 */
  uint32_t head;        /**< 写入位置 */
  uint32_t tail;        /**< 读取位置 */
  bool     isFull;      /**< 满标志 */
} RingBuffer_t;

/**
 * @brief   初始化环形缓冲区
 *
 * @param[in,out] rb      环形缓冲区结构体指针
 * @param[in]     buffer  缓冲区内存指针
 * @param[in]     size    缓冲区大小
 *
 * @return  None
 *
 * @note    buffer必须由调用者分配，生命周期需覆盖整个使用期
 */
void RingBuffer_Init(RingBuffer_t *rb, uint8_t *buffer, uint32_t size);

/**
 * @brief   重置环形缓冲区
 *
 * @param[in,out] rb  环形缓冲区结构体指针
 *
 * @return  None
 */
void RingBuffer_Reset(RingBuffer_t *rb);

/**
 * @brief   写入单个字节
 *
 * @param[in,out] rb    环形缓冲区结构体指针
 * @param[in]     data  待写入的字节
 *
 * @retval  true   写入成功
 * @retval  false  缓冲区已满
 */
bool RingBuffer_WriteByte(RingBuffer_t *rb, uint8_t data);

/**
 * @brief   读取单个字节
 *
 * @param[in,out] rb    环形缓冲区结构体指针
 * @param[out]    data  读取的字节存储位置
 *
 * @retval  true   读取成功
 * @retval  false  缓冲区为空
 */
bool RingBuffer_ReadByte(RingBuffer_t *rb, uint8_t *data);

/**
 * @brief   写入数据块
 *
 * @param[in,out] rb    环形缓冲区结构体指针
 * @param[in]     data  待写入的数据指针
 * @param[in]     len   数据长度
 *
 * @return  实际写入的字节数
 *
 * @note    如果空间不足，只写入部分数据
 */
uint32_t RingBuffer_Write(RingBuffer_t *rb, const uint8_t *data, uint32_t len);

/**
 * @brief   读取数据块
 *
 * @param[in,out] rb    环形缓冲区结构体指针
 * @param[out]    data  读取数据的存储位置
 * @param[in]     len   期望读取的长度
 *
 * @return  实际读取的字节数
 */
uint32_t RingBuffer_Read(RingBuffer_t *rb, uint8_t *data, uint32_t len);

/**
 * @brief   查看数据但不移除
 *
 * @param[in]  rb    环形缓冲区结构体指针
 * @param[out] data  读取数据的存储位置
 * @param[in]  len   期望读取的长度
 *
 * @return  实际读取的字节数
 *
 * @note    数据保留在缓冲区中，不改变读取位置
 */
uint32_t RingBuffer_Peek(const RingBuffer_t *rb, uint8_t *data, uint32_t len);

/**
 * @brief   获取可用数据长度
 *
 * @param[in] rb  环形缓冲区结构体指针
 *
 * @return  可读取的字节数
 */
uint32_t RingBuffer_GetAvailable(const RingBuffer_t *rb);

/**
 * @brief   获取剩余空间
 *
 * @param[in] rb  环形缓冲区结构体指针
 *
 * @return  可写入的字节数
 */
uint32_t RingBuffer_GetFree(const RingBuffer_t *rb);

/**
 * @brief   检查缓冲区是否为空
 *
 * @param[in] rb  环形缓冲区结构体指针
 *
 * @retval  true   缓冲区为空
 * @retval  false  缓冲区非空
 */
bool RingBuffer_IsEmpty(const RingBuffer_t *rb);

/**
 * @brief   检查缓冲区是否已满
 *
 * @param[in] rb  环形缓冲区结构体指针
 *
 * @retval  true   缓冲区已满
 * @retval  false  缓冲区未满
 */
bool RingBuffer_IsFull(const RingBuffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFFER_H */
