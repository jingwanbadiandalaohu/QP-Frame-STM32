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
#include "cmsis_os2.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// 临时日志开关：仅记录到RAM，避免影响串口时序
#define MODBUS_TX_LOG_ENABLE  1

#if MODBUS_TX_LOG_ENABLE
/**
 * @brief 最近一次发送信息（用于现场调试）
 */
static volatile uint16_t s_last_tx_count = 0;
static volatile uint8_t s_last_tx_tail[4] = {0};
#endif

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
  uint32_t timeout = (byte_timeout_ms >= 0) ? (uint32_t)byte_timeout_ms : 0xFFFF;

#if MODBUS_TX_LOG_ENABLE
  // 记录发送长度和末尾字节，便于确认是否少发CRC
  s_last_tx_count = count;
  uint16_t tail_len = (count < 4U) ? count : 4U;
  for(uint16_t i = 0; i < tail_len; i++)
  {
    s_last_tx_tail[i] = buf[count - tail_len + i];
  }
#endif

  // NOTE: DMA发送可能无法访问栈区缓冲，先用阻塞发送验证链路
  if(uart_transmit(uart, (uint8_t*)buf, count, timeout) != 0)
  {
    return -1;
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
  memcpy(registers_out, &dev->regs[index], quantity * sizeof(uint16_t));

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

/**
 * @brief   更新Modbus寄存器数据
 *
 * @param[in]   regs  寄存器数组指针
 *
 * @return  None
 *
 * @note    该函数填充传感器数据到Modbus寄存器，
 *          地址映射：100-109模拟板，110-119数字板，141传感器类型
 */
void modbus_update_regs(uint16_t *regs)
{
  static uint16_t cnt;
  cnt++;
  
  if(regs == NULL)
  {
    return;
  }

  // 地址100-109: 模拟板
  regs[0] = 0;   // 100: CL AD Value
  regs[1] = 1;   // 101: CH AD Value
  regs[2] = 2;   // 102: PI AD Value (进气压力AD值)
  regs[3] = 3;   // 103: PD AD Value (排气压力AD值)
  regs[4] = 4;   // 104: TI AD Value (进气温度AD值)
  regs[5] = 5;   // 105: TM AD Value (电机温度AD值)
  regs[6] = 6;   // 106: VX AD Value
  regs[7] = 7;   // 107: VY AD Value
  regs[8] = 8;   // 108: CLA AD Value (电流泄漏AD值)
  regs[9] = 9;   // 109: VoltY AD Value

  // 地址110-119: 数字板
  regs[10] = 10000+cnt;  // 110: CL Value (mA*1000)
  regs[11] = 20000;  // 111: CH Value (mA*1000)
  regs[12] = 65000;  // 112: Intake Pressure (进气压力 Psi*10)
  regs[13] = 135;  // 113: Discharge Pressure (排气压力 Psi*10)
  regs[14] = 800;  // 114: Intake Temperature (进气温度 ℃*10)
  regs[15] = 800;  // 115: Motor Temperature (电机温度 ℃*10)
  regs[16] = 200;  // 116: X-Vibration (X振动 g*1000)
  regs[17] = 200;  // 117: Y-Vibration (Y振动 g*1000)
  regs[18] = 1;  // 118: Current Leakage (电流泄漏 mA*1000)
  regs[19] = 1000;  // 119: Y Point Voltage (Y点电压 V*10)

  // 地址130-139: 扩展数据
  // regs[20] = 0;  // 120: FW flag (错误状态标志)
  // regs[30] = 0;  // 130: Discharge Temperature (排气温度 ℃*10)
  // regs[31] = 0;  // 131: X-Vibration (X振动 g*10)
  // regs[32] = 0;  // 132: Z-Vibration (Z振动 g*10)
  // regs[33] = 0;  // 133: Y Point Voltage (Y点电压 V*10)
  // regs[34] = 0;  // 134: Star Point Voltage (星点电压 V*10)
  // regs[35] = 0;  // 135: Serial Number HSB (序列号高位)
  // regs[36] = 0;  // 136: Serial Number LSB (序列号低位)
  // regs[37] = 0;  // 137: SysSyncCount (系统同步计数)
  // regs[38] = 0;  // 138: SysDataCount (系统数据计数)
  // regs[39] = 0;  // 139: SysErrorCount (系统错误计数)
  // regs[40] = 0;  // 140: System Version (系统版本)

  // 地址141: 传感器类型
  // 0-NG, 1-XT1, 21-XT150 Type1, 22-XT175 Type1, 
  // 23-NGG(P.XT150) Type1, 24-CTS Type1, 25-XT150 Type0,
  // 26-XT175Type0, 27-NGG(P.XT150) Type0, 28-CTS Type0, 50-Zenith, 101-SFD01 ， 102-DGB
  regs[41] = 102;  // 141: Sensor Type (传感器类型)
}
