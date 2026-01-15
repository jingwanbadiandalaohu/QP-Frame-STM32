/**
 * @file    drv_uart_impl.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   STM32H7 UART驱动实现
 *
 * @details 实现UART1和UART2的HAL层封装，支持阻塞和中断收发模式。
 *          包含单字节接收缓冲和中断回调处理。
 */

#include "drv_uart.h"
#include "platform_config.h"

/* ==================== UART1硬件配置 ==================== */

#define UART1_INSTANCE              USART1                    /**< UART1外设实例 */
#define UART1_CLK_ENABLE()          __HAL_RCC_USART1_CLK_ENABLE()
#define UART1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART1_TX_PIN                GPIO_PIN_9                /**< UART1 TX: PA9 */
#define UART1_RX_PIN                GPIO_PIN_10               /**< UART1 RX: PA10 */
#define UART1_PORT                  GPIOA
#define UART1_AF                    GPIO_AF7_USART1
#define UART1_IRQn                  USART1_IRQn

/* ==================== UART2硬件配置 ==================== */

#define UART2_INSTANCE              USART2                    /**< UART2外设实例 */
#define UART2_CLK_ENABLE()          __HAL_RCC_USART2_CLK_ENABLE()
#define UART2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_TX_PIN                GPIO_PIN_2                /**< UART2 TX: PA2 */
#define UART2_RX_PIN                GPIO_PIN_3                /**< UART2 RX: PA3 */
#define UART2_PORT                  GPIOA
#define UART2_AF                    GPIO_AF7_USART2
#define UART2_IRQn                  USART2_IRQn

/* ==================== 私有数据结构 ==================== */

/**
 * @brief UART私有数据结构
 */
typedef struct
{
  UART_HandleTypeDef hal_handle;  /**< HAL句柄 */
  uint8_t rx_byte;                /**< 单字节接收缓冲 */
  volatile uint8_t rx_ready;      /**< 接收就绪标志 */
  uint8_t initialized;            /**< 初始化标志 */
} UART_Private_t;

static UART_Private_t s_uart1_private = {0};  /**< UART1私有数据 */
static UART_Private_t s_uart2_private = {0};  /**< UART2私有数据 */

/* ==================== 私有辅助函数 ==================== */

/**
 * @brief   从HAL句柄获取私有数据指针
 *
 * @param[in] huart  HAL UART句柄
 *
 * @return  私有数据指针，无效时返回NULL
 */
static UART_Private_t *drv_uart_private_from_hal(UART_HandleTypeDef *huart)
{
  if(huart == NULL)
  {
    return NULL;
  }

  if(huart->Instance == UART1_INSTANCE)
  {
    return &s_uart1_private;
  }
  if(huart->Instance == UART2_INSTANCE)
  {
    return &s_uart2_private;
  }

  return NULL;
}

/**
 * @brief   从设备指针获取私有数据指针
 *
 * @param[in] dev  UART设备指针
 *
 * @return  私有数据指针，无效时返回NULL
 */
static UART_Private_t *drv_uart_private_from_dev(UART_Device_t *dev)
{
  if(dev == NULL)
  {
    return NULL;
  }

  return (UART_Private_t *)dev->hw_handle;
}

/* ==================== HAL MSP回调函数 ==================== */

/**
 * @brief   UART MSP初始化回调
 *
 * @details 配置GPIO、使能时钟、设置中断优先级
 *
 * @param[in] huart  HAL UART句柄
 *
 * @return  None
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(huart->Instance == UART1_INSTANCE)
  {
    UART1_CLK_ENABLE();
    UART1_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin       = UART1_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART1_AF;
    HAL_GPIO_Init(UART1_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = UART1_RX_PIN;
    HAL_GPIO_Init(UART1_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(UART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART1_IRQn);
  }
  else if(huart->Instance == UART2_INSTANCE)
  {
    UART2_CLK_ENABLE();
    UART2_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin       = UART2_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART2_AF;
    HAL_GPIO_Init(UART2_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = UART2_RX_PIN;
    HAL_GPIO_Init(UART2_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(UART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART2_IRQn);
  }
}

/**
 * @brief   UART MSP反初始化回调
 *
 * @param[in] huart  HAL UART句柄
 *
 * @return  None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if(huart->Instance == UART1_INSTANCE)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(UART1_PORT, UART1_TX_PIN | UART1_RX_PIN);
    HAL_NVIC_DisableIRQ(UART1_IRQn);
  }
  else if(huart->Instance == UART2_INSTANCE)
  {
    __HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(UART2_PORT, UART2_TX_PIN | UART2_RX_PIN);
    HAL_NVIC_DisableIRQ(UART2_IRQn);
  }
}

/**
 * @brief   UART接收完成回调
 *
 * @details 设置接收就绪标志，重新启动单字节中断接收
 *
 * @param[in] huart  HAL UART句柄
 *
 * @return  None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UART_Private_t *priv = drv_uart_private_from_hal(huart);
  if(priv == NULL)
  {
    return;
  }

  priv->rx_ready = 1U;
  HAL_UART_Receive_IT(&priv->hal_handle, &priv->rx_byte, 1);
}

/* ==================== 驱动操作函数实现 ==================== */

/**
 * @brief   UART初始化
 *
 * @param[in] dev     设备指针
 * @param[in] config  配置参数
 *
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int stm32h7_uart_init(UART_Device_t *dev, DRV_UART_Config_t *config)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);
  UART_HandleTypeDef *huart = NULL;

  if(dev == NULL || config == NULL || priv == NULL)
  {
    return DRV_ERROR;
  }

  huart = &priv->hal_handle;

  if(config->instance == DRV_UART1)
  {
    huart->Instance = UART1_INSTANCE;
  }
  else if(config->instance == DRV_UART2)
  {
    huart->Instance = UART2_INSTANCE;
  }
  else
  {
    return DRV_ERROR;
  }

  huart->Init.BaudRate = config->baudrate;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = (config->stop_bits == DRV_UART_STOPBITS_2) ?
                          UART_STOPBITS_2 : UART_STOPBITS_1;

  switch(config->parity)
  {
    case DRV_UART_PARITY_EVEN:
      huart->Init.Parity = UART_PARITY_EVEN;
      break;
    case DRV_UART_PARITY_ODD:
      huart->Init.Parity = UART_PARITY_ODD;
      break;
    default:
      huart->Init.Parity = UART_PARITY_NONE;
      break;
  }

  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;

  if(HAL_UART_Init(huart) != HAL_OK)
  {
    return DRV_ERROR;
  }

  HAL_UART_Receive_IT(huart, &priv->rx_byte, 1);
  priv->initialized = 1U;

  return DRV_OK;
}

/**
 * @brief   UART反初始化
 *
 * @param[in] dev  设备指针
 *
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int stm32h7_uart_deinit(UART_Device_t *dev)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);

  if(priv == NULL)
  {
    return DRV_ERROR;
  }

  if(HAL_UART_DeInit(&priv->hal_handle) != HAL_OK)
  {
    return DRV_ERROR;
  }

  priv->initialized = 0U;

  return DRV_OK;
}

/**
 * @brief   UART阻塞发送
 *
 * @param[in] dev      设备指针
 * @param[in] data     发送数据
 * @param[in] len      数据长度
 * @param[in] timeout  超时时间
 *
 * @return  DRV_OK/DRV_TIMEOUT/DRV_BUSY/DRV_ERROR
 */
static int stm32h7_uart_transmit(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);
  HAL_StatusTypeDef status;

  if(priv == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  status = HAL_UART_Transmit(&priv->hal_handle, data, len, timeout);

  if(status == HAL_OK)
  {
    return DRV_OK;
  }
  else if(status == HAL_TIMEOUT)
  {
    return DRV_TIMEOUT;
  }
  else if(status == HAL_BUSY)
  {
    return DRV_BUSY;
  }

  return DRV_ERROR;
}

/**
 * @brief   UART阻塞接收
 *
 * @param[in]  dev      设备指针
 * @param[out] data     接收缓冲区
 * @param[in]  len      期望长度
 * @param[in]  timeout  超时时间
 *
 * @return  DRV_OK/DRV_TIMEOUT/DRV_BUSY/DRV_ERROR
 */
static int stm32h7_uart_receive(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);
  HAL_StatusTypeDef status;

  if(priv == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  status = HAL_UART_Receive(&priv->hal_handle, data, len, timeout);

  if(status == HAL_OK)
  {
    return DRV_OK;
  }
  else if(status == HAL_TIMEOUT)
  {
    return DRV_TIMEOUT;
  }
  else if(status == HAL_BUSY)
  {
    return DRV_BUSY;
  }

  return DRV_ERROR;
}

/**
 * @brief   UART中断发送
 *
 * @param[in] dev   设备指针
 * @param[in] data  发送数据
 * @param[in] len   数据长度
 *
 * @return  DRV_OK/DRV_BUSY/DRV_ERROR
 */
static int stm32h7_uart_transmit_it(UART_Device_t *dev, uint8_t *data, uint16_t len)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);
  HAL_StatusTypeDef status;

  if(priv == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  status = HAL_UART_Transmit_IT(&priv->hal_handle, data, len);

  if(status == HAL_OK)
  {
    return DRV_OK;
  }
  else if(status == HAL_BUSY)
  {
    return DRV_BUSY;
  }

  return DRV_ERROR;
}

/**
 * @brief   UART中断接收
 *
 * @param[in]  dev   设备指针
 * @param[out] data  接收缓冲区
 * @param[in]  len   期望长度
 *
 * @return  DRV_OK/DRV_BUSY/DRV_ERROR
 */
static int stm32h7_uart_receive_it(UART_Device_t *dev, uint8_t *data, uint16_t len)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);
  HAL_StatusTypeDef status;

  if(priv == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  status = HAL_UART_Receive_IT(&priv->hal_handle, data, len);

  if(status == HAL_OK)
  {
    return DRV_OK;
  }
  else if(status == HAL_BUSY)
  {
    return DRV_BUSY;
  }

  return DRV_ERROR;
}

/* ==================== 操作函数集与设备实例 ==================== */

/**
 * @brief STM32H7 UART操作函数集
 */
static UART_Ops_t stm32h7_uart_ops =
{
  .init = stm32h7_uart_init,
  .deinit = stm32h7_uart_deinit,
  .transmit = stm32h7_uart_transmit,
  .receive = stm32h7_uart_receive,
  .transmit_it = stm32h7_uart_transmit_it,
  .receive_it = stm32h7_uart_receive_it
};

/**
 * @brief UART1设备实例
 */
static UART_Device_t uart1_device =
{
  .name = "UART1",
  .instance = DRV_UART1,
  .hw_handle = &s_uart1_private,
  .ops = &stm32h7_uart_ops
};

/**
 * @brief UART2设备实例
 */
static UART_Device_t uart2_device =
{
  .name = "UART2",
  .instance = DRV_UART2,
  .hw_handle = &s_uart2_private,
  .ops = &stm32h7_uart_ops
};

UART_Device_t *drv_uart1 = &uart1_device;  /**< UART1全局设备指针 */
UART_Device_t *drv_uart2 = &uart2_device;  /**< UART2全局设备指针 */

/* ==================== 扩展接口函数 ==================== */

/**
 * @brief   从接收缓冲区读取单字节
 *
 * @param[in]  dev  设备指针
 * @param[out] out  读取字节存储指针
 *
 * @return  1有数据，0无数据，DRV_ERROR参数错误
 */
int uart_read_byte(UART_Device_t *dev, uint8_t *out)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);

  if(priv == NULL || out == NULL)
  {
    return DRV_ERROR;
  }

  if(priv->rx_ready == 0U)
  {
    return 0;
  }

  *out = priv->rx_byte;
  priv->rx_ready = 0U;

  return 1;
}

/**
 * @brief   UART中断处理函数
 *
 * @param[in] dev  设备指针
 *
 * @return  None
 *
 * @note    需在USARTx_IRQHandler中调用
 */
void drv_uart_irq_handler(UART_Device_t *dev)
{
  UART_Private_t *priv = drv_uart_private_from_dev(dev);

  if(priv == NULL)
  {
    return;
  }

  HAL_UART_IRQHandler(&priv->hal_handle);
}

/**
 * @brief   printf底层字符输出函数
 *
 * @details 将字符通过UART2发送，供printf库使用
 *
 * @param[in] character  待发送字符
 *
 * @return  None
 */
void _putchar(char character)
{
  uart_transmit(drv_uart2, (uint8_t *)&character, 1, 0xFFFF);
}
