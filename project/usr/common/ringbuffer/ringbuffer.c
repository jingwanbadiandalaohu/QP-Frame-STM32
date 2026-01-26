/**
 * @file    ringbuffer.c
 * @author  Dylan
 * @date    2026-01-26
 * @brief   环形缓冲区实现
 *
 * @details 提供线程安全的环形缓冲区，支持字节流和数据块操作
 */

#include "ringbuffer.h"
#include <string.h>

/**
 * @brief   初始化环形缓冲区
 */
void RingBuffer_Init(RingBuffer_t *rb, uint8_t *buffer, uint32_t size)
{
  if(rb == NULL || buffer == NULL || size == 0)
  {
    return;
  }

  rb->buffer = buffer;
  rb->size = size;
  rb->head = 0;
  rb->tail = 0;
  rb->isFull = false;
}

/**
 * @brief   重置环形缓冲区
 */
void RingBuffer_Reset(RingBuffer_t *rb)
{
  if(rb == NULL)
  {
    return;
  }

  rb->head = 0;
  rb->tail = 0;
  rb->isFull = false;
}

/**
 * @brief   写入单个字节
 */
bool RingBuffer_WriteByte(RingBuffer_t *rb, uint8_t data)
{
  if(rb == NULL || rb->isFull)
  {
    return false;
  }

  rb->buffer[rb->head] = data;
  rb->head = (rb->head + 1) % rb->size;

  if(rb->head == rb->tail)
  {
    rb->isFull = true;
  }

  return true;
}

/**
 * @brief   读取单个字节
 */
bool RingBuffer_ReadByte(RingBuffer_t *rb, uint8_t *data)
{
  if(rb == NULL || data == NULL)
  {
    return false;
  }

  if(RingBuffer_IsEmpty(rb))
  {
    return false;
  }

  *data = rb->buffer[rb->tail];
  rb->tail = (rb->tail + 1) % rb->size;
  rb->isFull = false;

  return true;
}

/**
 * @brief   写入数据块
 */
uint32_t RingBuffer_Write(RingBuffer_t *rb, const uint8_t *data, uint32_t len)
{
  if(rb == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  uint32_t freeSpace = RingBuffer_GetFree(rb);
  uint32_t writeLen = (len > freeSpace) ? freeSpace : len;

  for(uint32_t i = 0; i < writeLen; i++)
  {
    rb->buffer[rb->head] = data[i];
    rb->head = (rb->head + 1) % rb->size;
  }

  if(rb->head == rb->tail && writeLen > 0)
  {
    rb->isFull = true;
  }

  return writeLen;
}

/**
 * @brief   读取数据块
 */
uint32_t RingBuffer_Read(RingBuffer_t *rb, uint8_t *data, uint32_t len)
{
  if(rb == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  uint32_t available = RingBuffer_GetAvailable(rb);
  uint32_t readLen = (len > available) ? available : len;

  for(uint32_t i = 0; i < readLen; i++)
  {
    data[i] = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
  }

  rb->isFull = false;

  return readLen;
}

/**
 * @brief   查看数据但不移除
 */
uint32_t RingBuffer_Peek(const RingBuffer_t *rb, uint8_t *data, uint32_t len)
{
  if(rb == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  uint32_t available = RingBuffer_GetAvailable(rb);
  uint32_t peekLen = (len > available) ? available : len;
  uint32_t tempTail = rb->tail;

  for(uint32_t i = 0; i < peekLen; i++)
  {
    data[i] = rb->buffer[tempTail];
    tempTail = (tempTail + 1) % rb->size;
  }

  return peekLen;
}

/**
 * @brief   获取可用数据长度
 */
uint32_t RingBuffer_GetAvailable(const RingBuffer_t *rb)
{
  if(rb == NULL)
  {
    return 0;
  }

  if(rb->isFull)
  {
    return rb->size;
  }

  if(rb->head >= rb->tail)
  {
    return rb->head - rb->tail;
  }
  else
  {
    return rb->size - rb->tail + rb->head;
  }
}

/**
 * @brief   获取剩余空间
 */
uint32_t RingBuffer_GetFree(const RingBuffer_t *rb)
{
  if(rb == NULL)
  {
    return 0;
  }

  return rb->size - RingBuffer_GetAvailable(rb);
}

/**
 * @brief   检查缓冲区是否为空
 */
bool RingBuffer_IsEmpty(const RingBuffer_t *rb)
{
  if(rb == NULL)
  {
    return true;
  }

  return (!rb->isFull && (rb->head == rb->tail));
}

/**
 * @brief   检查缓冲区是否已满
 */
bool RingBuffer_IsFull(const RingBuffer_t *rb)
{
  if(rb == NULL)
  {
    return false;
  }

  return rb->isFull;
}
