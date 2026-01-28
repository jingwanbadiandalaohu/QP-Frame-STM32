/**
 * @file    ringbuffer.c
 * @author  Dylan
 * @date    2026-01-26
 * @brief   环形缓冲区实现
 *
 * @details 提供线程安全的环形缓冲区，支持字节流和数据块操作
 *
 * ============================================================================
 *                        生产者-消费者模型说明
 * ============================================================================
 *
 * 环形缓冲区是经典的生产者-消费者模型实现：
 *
 * 【模型结构】
 *   生产者（Producer）  →  [环形缓冲区]  →  消费者（Consumer）
 *       ↓                      ↓                    ↓
 *   写入数据              临时存储              读取数据
 *   (Write)              (Buffer)              (Read)
 *
 * 【在本系统中的应用】
 *
 *   生产者：UART DMA + IDLE中断
 *   ├─ 硬件：UART接收数据 → DMA写入临时缓冲区
 *   └─ 中断：IDLE触发 → 调用RingBuffer_Write()写入环形缓冲区
 *
 *   缓冲区：环形缓冲区（本模块）
 *   ├─ 作用：解耦生产者和消费者的速度差异
 *   ├─ 大小：64字节（可配置）
 *   └─ 模式：覆盖模式（满了自动覆盖旧数据）
 *
 *   消费者：应用任务（UartRxTask）
 *   ├─ 周期：每10ms检查一次
 *   └─ 操作：调用RingBuffer_Read()读取数据并处理
 *
 * 【数据流程】
 *
 *   1. 生产阶段（中断上下文）：
 *      UART接收 → DMA缓冲区 → IDLE中断 → RingBuffer_Write()
 *      ├─ 速度：快速、突发性
 *      ├─ 频率：不定时（取决于串口数据到达）
 *      └─ 优先级：高（中断优先级5）
 *
 *   2. 缓冲阶段（环形缓冲区）：
 *      head指针：生产者写入位置
 *      tail指针：消费者读取位置
 *      isFull标志：区分空和满状态
 *
 *   3. 消费阶段（任务上下文）：
 *      RingBuffer_Read() → 应用处理 → 串口回显
 *      ├─ 速度：相对较慢
 *      ├─ 频率：固定周期（10ms）
 *      └─ 优先级：普通（任务优先级Normal）
 *
 * 【关键特性】
 *
 *   1. 异步解耦：
 *      - 生产者和消费者独立运行，互不阻塞
 *      - 生产者可以快速写入后立即返回
 *      - 消费者按自己的节奏读取处理
 *
 *   2. 速度匹配：
 *      - 缓冲区吸收速度差异
 *      - 生产快时：数据暂存在缓冲区
 *      - 消费快时：缓冲区逐渐清空
 *
 *   3. 数据保护：
 *      - 覆盖模式：满了自动覆盖旧数据（适合实时数据流）
 *      - 阻塞模式：满了拒绝写入（适合关键数据）
 *
 * 【线程安全考虑】
 *
 *   在本系统中：
 *   - 生产者：运行在中断上下文（USART1_IRQHandler）
 *   - 消费者：运行在任务上下文（UartRxTask）
 *
 *   潜在竞争条件：
 *   - 写入和读取可能同时访问head/tail指针
 *
 *   保护机制：
 *   - 单生产者-单消费者：无需额外锁（原子性由硬件保证）
 *   - 多生产者或多消费者：需要添加互斥锁或关中断保护
 *
 * 【使用示例】
 *
 *   // 生产者（中断中）
 *   void USART1_IRQHandler(void)
 *   {
 *     if(IDLE中断)
 *     {
 *       uint32_t len = 计算接收长度;
 *       RingBuffer_Write(&ringbuf, dma_buf, len);  // 生产数据
 *     }
 *   }
 *
 *   // 消费者（任务中）
 *   void UartRxTask(void)
 *   {
 *     while(1)
 *     {
 *       if(RingBuffer_GetAvailable(&ringbuf) > 0)
 *       {
 *         len = RingBuffer_Read(&ringbuf, buf, size);  // 消费数据
 *         处理数据(buf, len);
 *       }
 *       osDelay(10);
 *     }
 *   }
 *
 * ============================================================================
 */

#include "ringbuffer.h"
#include <string.h>

/**
 * @brief   初始化环形缓冲区
 *
 * @details 设置缓冲区指针和大小，初始化读写指针
 *          - head: 写入位置，指向下一个可写入的位置
 *          - tail: 读取位置，指向下一个可读取的位置
 *          - isFull: 满标志，用于区分空和满状态（head == tail时）
 */
void RingBuffer_Init(RingBuffer_t *rb, uint8_t *buffer, uint32_t size)
{
  if(rb == NULL || buffer == NULL || size == 0)
  {
    return;
  }

  rb->buffer = buffer;
  rb->size = size;
  rb->head = 0;    // 写指针初始化为0
  rb->tail = 0;    // 读指针初始化为0
  rb->isFull = false;  // 初始状态为空
}

/**
 * @brief   重置环形缓冲区
 *
 * @details 清空缓冲区，恢复到初始化状态
 *          注意：不清除缓冲区内容，只重置读写指针
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
 *
 * @details 环形缓冲区写入逻辑（覆盖模式）：
 *          1. 如果缓冲区已满，自动覆盖最旧的数据
 *             - 移动tail指针，丢弃最旧的一个字节
 *          2. 将数据写入head位置
 *          3. head指针前进一位（模运算实现环形）
 *          4. 如果写入后head追上tail，标记为满
 *
 *          状态判断：
 *          - 空：head == tail && !isFull
 *          - 满：head == tail && isFull
 *          - 部分填充：head != tail
 */
bool RingBuffer_WriteByte(RingBuffer_t *rb, uint8_t data)
{
  if(rb == NULL)
  {
    return false;
  }

  // 如果满了，自动覆盖最旧的数据
  if(rb->isFull)
  {
    rb->tail = (rb->tail + 1) % rb->size;  // 移动读指针，丢弃最旧数据
  }

  // 写入数据到head位置
  rb->buffer[rb->head] = data;
  
  // head指针前进（模运算实现环形回绕）
  rb->head = (rb->head + 1) % rb->size;

  // 检查是否写满：head追上tail表示满
  if(rb->head == rb->tail)
  {
    rb->isFull = true;
  }

  return true;
}

/**
 * @brief   读取单个字节
 *
 * @details 环形缓冲区读取逻辑：
 *          1. 检查缓冲区是否为空
 *          2. 从tail位置读取数据
 *          3. tail指针前进一位（模运算实现环形）
 *          4. 读取后必然不满，清除满标志
 *
 *          注意：读取不清除缓冲区内容，只移动tail指针
 */
bool RingBuffer_ReadByte(RingBuffer_t *rb, uint8_t *data)
{
  if(rb == NULL || data == NULL)
  {
    return false;
  }

  // 检查是否为空
  if(RingBuffer_IsEmpty(rb))
  {
    return false;
  }

  // 从tail位置读取数据
  *data = rb->buffer[rb->tail];
  
  // tail指针前进（模运算实现环形回绕）
  rb->tail = (rb->tail + 1) % rb->size;
  
  // 读取后必然不满
  rb->isFull = false;

  return true;
}

/**
 * @brief   写入数据块
 *
 * @details 批量写入逻辑（覆盖模式）：
 *          1. 逐字节写入数据
 *          2. 每次写入前检查是否已满
 *          3. 如果满了，自动移动tail指针覆盖旧数据
 *          4. 写入后检查head是否追上tail，更新满标志
 *
 *          示例（size=8）：
 *          初始：[_ _ _ _ _ _ _ _]  head=0, tail=0, isFull=false
 *          写入3字节：[A B C _ _ _ _ _]  head=3, tail=0, isFull=false
 *          写满8字节：[A B C D E F G H]  head=0, tail=0, isFull=true
 *          再写2字节：[I J C D E F G H]  head=2, tail=2, isFull=true (覆盖A,B)
 */
uint32_t RingBuffer_Write(RingBuffer_t *rb, const uint8_t *data, uint32_t len)
{
  if(rb == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  // 逐字节写入
  for(uint32_t i = 0; i < len; i++)
  {
    // 如果满了，自动覆盖最旧的数据
    if(rb->isFull)
    {
      rb->tail = (rb->tail + 1) % rb->size;  // 移动读指针，丢弃最旧数据
    }

    // 写入数据到head位置
    rb->buffer[rb->head] = data[i];
    
    // head指针前进（模运算实现环形回绕）
    rb->head = (rb->head + 1) % rb->size;

    // 检查是否写满：head追上tail表示满
    if(rb->head == rb->tail)
    {
      rb->isFull = true;
    }
  }

  return len;  // 返回实际写入的字节数（全部写入）
}


/**
 * @brief   读取数据块
 *
 * @details 批量读取逻辑：
 *          1. 计算可读取的数据量（取min(请求长度, 可用数据)）
 *          2. 逐字节从tail位置读取
 *          3. 每次读取后tail指针前进
 *          4. 读取完成后清除满标志
 *
 *          示例（size=8）：
 *          初始：[A B C D E _ _ _]  head=4, tail=0, available=4
 *          读取2字节：返回[A B]，tail=2, head=4, available=2
 *          读取5字节：只能读取2字节[C D]，tail=4, head=4, available=0
 */
uint32_t RingBuffer_Read(RingBuffer_t *rb, uint8_t *data, uint32_t len)
{
  if(rb == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  // 计算实际可读取的字节数
  uint32_t available = RingBuffer_GetAvailable(rb);
  uint32_t readLen = (len > available) ? available : len;

  // 逐字节读取
  for(uint32_t i = 0; i < readLen; i++)
  {
    data[i] = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;  // tail指针前进
  }

  // 读取后必然不满
  rb->isFull = false;

  return readLen;  // 返回实际读取的字节数
}

/**
 * @brief   查看数据但不移除
 *
 * @details 非破坏性读取逻辑：
 *          1. 使用临时指针tempTail遍历缓冲区
 *          2. 读取数据但不修改实际的tail指针
 *          3. 不改变缓冲区状态（isFull保持不变）
 *
 *          用途：预览数据内容，判断协议帧是否完整等
 *
 *          示例（size=8）：
 *          初始：[A B C D E _ _ _]  head=5, tail=0
 *          Peek 3字节：返回[A B C]，但tail仍为0，数据未被消费
 */
uint32_t RingBuffer_Peek(const RingBuffer_t *rb, uint8_t *data, uint32_t len)
{
  if(rb == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  // 计算实际可读取的字节数
  uint32_t available = RingBuffer_GetAvailable(rb);
  uint32_t peekLen = (len > available) ? available : len;
  
  // 使用临时指针，不修改实际的tail
  uint32_t tempTail = rb->tail;

  // 逐字节读取（使用临时指针）
  for(uint32_t i = 0; i < peekLen; i++)
  {
    data[i] = rb->buffer[tempTail];
    tempTail = (tempTail + 1) % rb->size;  // 临时指针前进
  }

  return peekLen;  // 返回实际读取的字节数
}

/**
 * @brief   获取可用数据长度
 *
 * @details 计算缓冲区中可读取的字节数：
 *          1. 如果满：可用数据 = size（整个缓冲区都是有效数据）
 *          2. 如果head >= tail：可用数据 = head - tail（线性区域）
 *          3. 如果head < tail：可用数据 = size - tail + head（环形回绕）
 *
 *          示例（size=8）：
 *          情况1：[A B C D E F G H]  head=0, tail=0, isFull=true  → available=8
 *          情况2：[A B C _ _ _ _ _]  head=3, tail=0              → available=3
 *          情况3：[E F _ _ _ A B C]  head=2, tail=5              → available=5
 *                 (计算：8-5+2=5，即tail到末尾3个+开头到head的2个)
 */
uint32_t RingBuffer_GetAvailable(const RingBuffer_t *rb)
{
  if(rb == NULL)
  {
    return 0;
  }

  // 情况1：缓冲区已满
  if(rb->isFull)
  {
    return rb->size;
  }

  // 情况2：head在tail前面（线性区域）
  if(rb->head >= rb->tail)
  {
    return rb->head - rb->tail;
  }
  // 情况3：head在tail后面（环形回绕）
  else
  {
    return rb->size - rb->tail + rb->head;
  }
}

/**
 * @brief   获取剩余空间
 *
 * @details 计算缓冲区中可写入的字节数：
 *          剩余空间 = 总大小 - 已用空间
 *
 *          注意：在覆盖模式下，即使返回0（满），仍可继续写入（会覆盖旧数据）
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
