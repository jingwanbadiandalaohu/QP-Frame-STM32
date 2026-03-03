/**
 * @file    modbus.c
 * @author  Dylan
 * @date    2026-01-28
 * @brief   Modbus从机设备层实现
 *
 * @details 实现nanoMODBUS平台适配接口，对接DMA+IDLE+环形缓冲区串口驱动，
 *          当前仅实现功能码0x03（读保持寄存器）
 */

#include "modbus.h"
#include "board.h"
#include "cmsis_os2.h"

// 共享数据互斥锁由应用层创建并管理，这里仅做外部引用
extern osMutexId_t g_modbusDataMutex;

/**
 * @brief   nanoMODBUS平台读取接口
 *
 * @param[out]  buf             接收缓冲区
 * @param[in]   count           期望读取字节数
 * @param[in]   byte_timeout_ms 字节间超时时间（毫秒）
 * @param[in]   arg             用户参数（uart_desc_t）
 *
 * @return  实际读取的字节数，超时返回0-count之间的值，错误返回负数
 *
 * @note    采用字节超时机制：收到第一个字节后，如果后续字节间隔超过
 *          byte_timeout_ms，则认为帧结束，返回已读取的字节数
 */
static int32_t modbus_platform_read(uint8_t *buf, uint16_t count,
                                    int32_t byte_timeout_ms, void *arg)
{
  uart_desc_t uart = (uart_desc_t)arg;
  uint32_t read_len = 0;
  uint32_t last_byte_tick = osKernelGetTickCount();

  while(read_len < count)
  {
    // 检查环形缓冲区可用数据
    uint32_t available = uart_get_available(uart);
    
    if(available > 0)
    {
      // 批量读取可用数据（不超过剩余需要的字节数）
      uint32_t to_read = (count - read_len) < available ? 
                         (count - read_len) : available;
      uint32_t actual = uart_read_ringbuf(uart, buf + read_len, to_read);
      read_len += actual;
      last_byte_tick = osKernelGetTickCount();  // 更新最后读取时间
    }
    else
    {
      // 没有数据可读，检查超时
      if(byte_timeout_ms >= 0)
      {
        uint32_t elapsed = osKernelGetTickCount() - last_byte_tick;
        
        // 如果已经读到至少1字节，且字节间超时，则认为帧结束
        if(read_len > 0 && elapsed >= (uint32_t)byte_timeout_ms)
        {
          break;
        }
        
        // 如果一个字节都没读到，总超时设为10倍字节超时
        if(read_len == 0 && elapsed >= (uint32_t)(byte_timeout_ms * 10))
        {
          break;
        }
      }
      
      // 短暂延时，避免CPU空转
      osDelay(1);
    }
  }

  return (int32_t)read_len;
}

/**
 * @brief   nanoMODBUS平台发送接口
 *
 * @param[in]   buf             发送缓冲区
 * @param[in]   count           发送字节数
 * @param[in]   byte_timeout_ms 超时时间（毫秒）
 * @param[in]   arg             用户参数（uart_desc_t）
 *
 * @return  实际发送的字节数，超时返回0-count之间的值，错误返回负数
 */
static int32_t modbus_platform_write(const uint8_t *buf, uint16_t count,
                                     int32_t byte_timeout_ms, void *arg)
{
  uart_desc_t uart = (uart_desc_t)arg;
  uint8_t *tx_dma_buf = NULL;
  uint32_t wait_start_tick = 0;
  uint32_t timeout_ms = (byte_timeout_ms >= 0) ? (uint32_t)byte_timeout_ms : 0xFFFFFFFFU;

  // Step 1: 基础参数检查
  if(uart == NULL || buf == NULL)
  {
    return -1;
  }
  if(count == 0)
  {
    return 0;
  }
  if(count > sizeof(Uart1_dma_tx_buf))
  {
    return -1;
  }

  // Step 2: 根据固定硬件拓扑选择DMA发送缓冲区
  if(uart == uart1_rs232)
  {
    tx_dma_buf = Uart1_dma_tx_buf;
  }
  else if(uart == uart2_rs485)
  {
    tx_dma_buf = Uart2_dma_tx_buf;
  }
  else
  {
    tx_dma_buf = NULL;
  }

  if(tx_dma_buf == NULL)
  {
    return -1;
  }

  // Step 3: 等待上一次发送完成，避免DMA启动时UART仍处于BUSY_TX
  wait_start_tick = osKernelGetTickCount();
  while(!uart_is_tx_idle(uart))
  {
    if((osKernelGetTickCount() - wait_start_tick) >= timeout_ms)
    {
      return -1;
    }

    osDelay(1);
  }

  // Step 4: DMA不能访问DTCM/栈区，先复制到板级RAM_D1专用缓冲区
  memcpy(tx_dma_buf, buf, count);   //tx_dma_buf目标地址，buf源地址

  // Step 5: 启动DMA发送
  if(uart_transmit_dma(uart, tx_dma_buf, count) != 0)
  {
    return -1;
  }

  // Step 6: 保持write阻塞语义，等待本帧DMA发送完成
  wait_start_tick = osKernelGetTickCount();
  while(!uart_is_tx_idle(uart))
  {
    if((osKernelGetTickCount() - wait_start_tick) >= timeout_ms)
    {
      return -1;
    }

    osDelay(1);
  }

  return (int32_t)count;
}

/**
 * @brief   读保持寄存器回调函数（功能码0x03）
 *
 * @param[in]   address      起始地址
 * @param[in]   quantity     寄存器数量
 * @param[out]  registers_out 输出寄存器数组
 * @param[in]   unit_id      单元ID（RTU地址）
 * @param[in]   arg          用户参数（modbus_dev_t指针）
 *
 * @return  NMBS_ERROR_NONE 成功，其他值为Modbus异常码
 */
static nmbs_error modbus_read_holding_regs_callback(uint16_t address, uint16_t quantity,
                                                    uint16_t *registers_out,
                                                    uint8_t unit_id, void *arg)
{
  (void) unit_id;
  modbus_dev_t *dev = (modbus_dev_t *)arg;

  // 检查地址范围：必须在 [base_addr, base_addr + regs_count) 内
  if(address < dev->base_addr || 
     address + quantity > dev->base_addr + dev->regs_count)
  {
    return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }

  // 转换为数组索引：Modbus地址 → 数组索引
  uint16_t index = address - dev->base_addr;
  // 读取保持寄存器前加锁，保证不会读到“半更新”数据
  if(osMutexAcquire(g_modbusDataMutex, osWaitForever) != osOK)
  {
    // 获取锁失败返回设备故障异常，让主站感知本次读失败
    return NMBS_EXCEPTION_SERVER_DEVICE_FAILURE;
  }

  memcpy(registers_out, &dev->regs[index], quantity * sizeof(uint16_t));
  // 读取完成立即释放锁，缩短临界区
  osMutexRelease(g_modbusDataMutex);

  return NMBS_ERROR_NONE;
}

/**
 * @brief   初始化Modbus从机
 *
 * @param[in]   dev         Modbus设备描述符指针
 * @param[in]   uart        串口描述符
 * @param[in]   slave_addr  从机地址（1-247）
 * @param[in]   regs        保持寄存器数组指针
 * @param[in]   regs_count  保持寄存器数量
 * @param[in]   base_addr   寄存器起始地址（如100表示地址100-199）
 *
 * @return  None
 */
void modbus_init(modbus_dev_t *dev, uart_desc_t uart, uint8_t slave_addr,
                 uint16_t *regs, uint16_t regs_count, uint16_t base_addr)
{
  if(dev == NULL || uart == NULL || regs == NULL)
  {
    return;
  }

  memset(dev, 0, sizeof(modbus_dev_t));
  dev->uart = uart;                                 
  dev->slave_addr = slave_addr;
  dev->regs = regs;
  dev->regs_count = regs_count;
  dev->base_addr = base_addr;

  // 配置平台接口
  nmbs_platform_conf platform_conf;
  nmbs_platform_conf_create(&platform_conf);
  platform_conf.transport = NMBS_TRANSPORT_RTU;     // 使用RTU模式
  platform_conf.read = modbus_platform_read;        // 注册NanoModbus读函数
  platform_conf.write = modbus_platform_write;      // 注册NanoModbus写函数
  platform_conf.arg = uart;

  // 配置回调函数
  nmbs_callbacks callbacks;
  nmbs_callbacks_create(&callbacks);
  callbacks.read_holding_registers = modbus_read_holding_regs_callback;     //注册保持寄存器回调函数
  callbacks.arg = dev;

  // 创建Modbus从机
  nmbs_error err = nmbs_server_create(&dev->nmbs, slave_addr, &platform_conf, &callbacks);
  if(err != NMBS_ERROR_NONE)
  {
    // 初始化失败处理
    return;
  }

  // 设置默认超时（9600波特率下，1字节约1ms）
  // Modbus RTU标准：字节间超时 = 1.5个字符时间 ≈ 2ms (9600bps)
  // 帧间隔 = 3.5个字符时间 ≈ 4ms
  nmbs_set_read_timeout(&dev->nmbs, 100);    // 100ms总超时
  nmbs_set_byte_timeout(&dev->nmbs, 10);     // 10ms字节间超时
}

/**
 * @brief   Modbus从机轮询处理函数
 *
 * @details 需要在任务中循环调用，处理接收到的Modbus请求
 *
 * @param[in]   dev  Modbus设备描述符指针
 *
 * @return  NMBS_ERROR_NONE 成功，其他值为错误码
 */
nmbs_error modbus_poll(modbus_dev_t *dev)
{
  if(dev == NULL)
  {
    return NMBS_ERROR_INVALID_ARGUMENT;
  }

  return nmbs_server_poll(&dev->nmbs);
}

/**
 * @brief   设置读取超时时间
 *
 * @param[in]   dev         Modbus设备描述符指针
 * @param[in]   timeout_ms  超时时间（毫秒），<0表示无限等待
 *
 * @return  None
 */
void modbus_set_read_timeout(modbus_dev_t *dev, int32_t timeout_ms)
{
  if(dev == NULL)
  {
    return;
  }

  nmbs_set_read_timeout(&dev->nmbs, timeout_ms);
}

/**
 * @brief   设置字节间超时时间
 *
 * @param[in]   dev         Modbus设备描述符指针
 * @param[in]   timeout_ms  超时时间（毫秒），<0表示无限等待
 *
 * @return  None
 */
void modbus_set_byte_timeout(modbus_dev_t *dev, int32_t timeout_ms)
{
  if(dev == NULL)
  {
    return;
  }

  nmbs_set_byte_timeout(&dev->nmbs, timeout_ms);
}
