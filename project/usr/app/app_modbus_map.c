/**
 * @file    app_modbus_map.c
 * @author  Dylan
 * @date    2026-03-02
 * @brief   Modbus寄存器映射应用层实现
 */

#include "app_modbus_map.h"

/**
 * @brief   更新Modbus保持寄存器映射
 *
 * @param[in]   regs  保持寄存器数组指针
 * @param[in]   Data  采集数据结构体指针
 *
 * @return  None
 */
void app_modbus_update_regs(uint16_t *regs, Data_t *Data)
{
  static uint16_t s_cnt = 0U;
  s_cnt++;

  if(regs == NULL || Data == NULL)
  {
    return;
  }

  // 地址100-109: 模拟板
  regs[0] = 0;            // 100: CL AD Value
  regs[1] = 1;            // 101: CH AD Value
  regs[2] = 2;            // 102: PI AD Value (进气压力AD值)
  regs[3] = 3;            // 103: PD AD Value (排气压力AD值)
  regs[4] = 4;            // 104: TI AD Value (进气温度AD值)
  regs[5] = 5;            // 105: TM AD Value (电机温度AD值)
  regs[6] = 6;            // 106: VX AD Value
  regs[7] = 7;            // 107: VY AD Value
  regs[8] = 8;            // 108: CLA AD Value (电流泄漏AD值)
  regs[9] = 9;            // 109: VoltY AD Value

  // 地址110-119: 数字板
  regs[10] = 10000 + s_cnt;  // 110: CL Value (mA*1000)
  regs[11] = 20000;          // 111: CH Value (mA*1000)
  regs[12] = 65000;          // 112: Intake Pressure (进气压力 Psi*10)
  regs[13] = 135;            // 113: Discharge Pressure (排气压力 Psi*10)
  regs[14] = 800;            // 114: Intake Temperature (进气温度 ℃*10)
  regs[15] = 800;            // 115: Motor Temperature (电机温度 ℃*10)
  regs[16] = 200;            // 116: X-Vibration (X振动 g*1000)
  regs[17] = 200;            // 117: Y-Vibration (Y振动 g*1000)
  regs[18] = 1;              // 118: Current Leakage (电流泄漏 mA*1000)
  regs[19] = Data->VoltY;    // 119: Y Point Voltage (Y点电压 V*10)

  // 地址130-139: 扩展数据
  // regs[20] = 0;            // 120: FW flag (错误状态标志)
  // regs[30] = 0;            // 130: Discharge Temperature (排气温度 ℃*10)
  // regs[31] = 0;            // 131: X-Vibration (X振动 g*10)
  // regs[32] = 0;            // 132: Z-Vibration (Z振动 g*10)
  // regs[33] = 0;            // 133: Y Point Voltage (Y点电压 V*10)
  // regs[34] = 0;            // 134: Star Point Voltage (星点电压 V*10)
  // regs[35] = 0;            // 135: Serial Number HSB (序列号高位)
  // regs[36] = 0;            // 136: Serial Number LSB (序列号低位)
  // regs[37] = 0;            // 137: SysSyncCount (系统同步计数)
  // regs[38] = 0;            // 138: SysDataCount (系统数据计数)
  // regs[39] = 0;            // 139: SysErrorCount (系统错误计数)
  // regs[40] = 0;            // 140: System Version (系统版本)

  // 地址141: 传感器类型
  // 0-NG, 1-XT1, 21-XT150 Type1, 22-XT175 Type1,
  // 23-NGG(P.XT150) Type1, 24-CTS Type1, 25-XT150 Type0,
  // 26-XT175Type0, 27-NGG(P.XT150) Type0, 28-CTS Type0, 50-Zenith, 101-SFD01, 102-DGB
  regs[41] = 102;  // 141: Sensor Type (传感器类型)
}
