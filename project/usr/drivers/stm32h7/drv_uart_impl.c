/**
 * @file drv_uart_impl.c
 * @brief STM32H7 平台 UART 驱动实现
 * @note 包含 MspInit、时钟配置、HAL 调用
 */

#include "drv_uart.h"
#include "platform_config.h"

/* ==================== UART 硬件配置 ==================== */
/* UART1 配置 */
#define UART1_INSTANCE              USART1
#define UART1_CLK_ENABLE()          __HAL_RCC_USART1_CLK_ENABLE()
#define UART1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART1_TX_PIN                GPIO_PIN_9
#define UART1_RX_PIN                GPIO_PIN_10
#define UART1_PORT                  GPIOA
#define UART1_AF                    GPIO_AF7_USART1
#define UART1_IRQn                  USART1_IRQn

/* UART2 配置 */
#define UART2_INSTANCE              USART2
#define UART2_CLK_ENABLE()          __HAL_RCC_USART2_CLK_ENABLE()
#define UART2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_TX_PIN                GPIO_PIN_2
#define UART2_RX_PIN                GPIO_PIN_3
#define UART2_PORT                  GPIOA
#define UART2_AF                    GPIO_AF7_USART2
#define UART2_IRQn                  USART2_IRQn

/* ==================== 内部数据结构 ==================== */
typedef struct
{
  UART_HandleTypeDef hal_handle;
  uint8_t rx_byte;
  volatile uint8_t rx_ready;
  uint8_t initialized;
} UART_Instance_t;

static UART_Instance_t s_uart_instances[DRV_UART_MAX] = {0};

/* ==================== HAL MSP 回调 ==================== */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(huart->Instance == UART1_INSTANCE)
  {
    UART1_CLK_ENABLE();
    UART1_GPIO_CLK_ENABLE();

    /* TX */
    GPIO_InitStruct.Pin       = UART1_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART1_AF;
    HAL_GPIO_Init(UART1_PORT, &GPIO_InitStruct);

    /* RX */
    GPIO_InitStruct.Pin       = UART1_RX_PIN;
    HAL_GPIO_Init(UART1_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(UART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART1_IRQn);
  }
  else if(huart->Instance == UART2_INSTANCE)
  {
    UART2_CLK_ENABLE();
    UART2_GPIO_CLK_ENABLE();

    /* TX */
    GPIO_InitStruct.Pin       = UART2_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART2_AF;
    HAL_GPIO_Init(UART2_PORT, &GPIO_InitStruct);

    /* RX */
    GPIO_InitStruct.Pin       = UART2_RX_PIN;
    HAL_GPIO_Init(UART2_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(UART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART2_IRQn);
  }
}

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

/* ==================== UART 接收回调 ==================== */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == UART1_INSTANCE)
  {
    s_uart_instances[DRV_UART1].rx_ready = 1U;
    HAL_UART_Receive_IT(&s_uart_instances[DRV_UART1].hal_handle,
                        &s_uart_instances[DRV_UART1].rx_byte, 1);
  }
  else if(huart->Instance == UART2_INSTANCE)
  {
    s_uart_instances[DRV_UART2].rx_ready = 1U;
    HAL_UART_Receive_IT(&s_uart_instances[DRV_UART2].hal_handle,
                        &s_uart_instances[DRV_UART2].rx_byte, 1);
  }
}


/* ==================== 驱动接口实现 ==================== */

/**
 * @brief 初始化 UART
 */
int DRV_UART_Init(UART_Config_t *config)
{
  if(config == NULL || config->instance >= DRV_UART_MAX)
  {
    return DRV_ERROR;
  }

  UART_Instance_t *inst = &s_uart_instances[config->instance];
  UART_HandleTypeDef *huart = &inst->hal_handle;

  /* 选择 UART 实例 */
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

  /* 配置参数 */
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

  /* 启动中断接收 */
  HAL_UART_Receive_IT(huart, &inst->rx_byte, 1);

  inst->initialized = 1;

  return DRV_OK;
}

/**
 * @brief 反初始化 UART
 */
int DRV_UART_DeInit(DRV_UART_Instance_t instance)
{
  if(instance >= DRV_UART_MAX)
  {
    return DRV_ERROR;
  }

  UART_Instance_t *inst = &s_uart_instances[instance];

  if(HAL_UART_DeInit(&inst->hal_handle) != HAL_OK)
  {
    return DRV_ERROR;
  }

  inst->initialized = 0;

  return DRV_OK;
}

/**
 * @brief 获取 UART 句柄
 */
UART_Handle_t DRV_UART_GetHandle(DRV_UART_Instance_t instance)
{
  if(instance >= DRV_UART_MAX)
  {
    return NULL;
  }

  return (UART_Handle_t)&s_uart_instances[instance].hal_handle;
}

/**
 * @brief 读取接收到的字节
 */
int DRV_UART_ReadByte(DRV_UART_Instance_t instance, uint8_t *out)
{
  if(instance >= DRV_UART_MAX || out == NULL)
  {
    return DRV_ERROR;
  }

  UART_Instance_t *inst = &s_uart_instances[instance];

  if(inst->rx_ready == 0U)
  {
    return 0;
  }

  *out = inst->rx_byte;
  inst->rx_ready = 0U;

  return 1;
}

/**
 * @brief 阻塞方式发送数据
 */
int DRV_UART_Transmit(UART_Handle_t handle, uint8_t *data, uint16_t len,
                      uint32_t timeout)
{
  if(handle == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  HAL_StatusTypeDef status = HAL_UART_Transmit((UART_HandleTypeDef *)handle,
                                                data, len, timeout);

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
 * @brief 阻塞方式接收数据
 */
int DRV_UART_Receive(UART_Handle_t handle, uint8_t *data, uint16_t len,
                     uint32_t timeout)
{
  if(handle == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  HAL_StatusTypeDef status = HAL_UART_Receive((UART_HandleTypeDef *)handle,
                                               data, len, timeout);

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
 * @brief 中断方式发送数据
 */
int DRV_UART_TransmitIT(UART_Handle_t handle, uint8_t *data, uint16_t len)
{
  if(handle == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  HAL_StatusTypeDef status = HAL_UART_Transmit_IT((UART_HandleTypeDef *)handle,
                                                   data, len);

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
 * @brief 中断方式接收数据
 */
int DRV_UART_ReceiveIT(UART_Handle_t handle, uint8_t *data, uint16_t len)
{
  if(handle == NULL || data == NULL)
  {
    return DRV_ERROR;
  }

  HAL_StatusTypeDef status = HAL_UART_Receive_IT((UART_HandleTypeDef *)handle,
                                                  data, len);

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

/* ==================== printf 输出重定向 ==================== */

void _putchar(char character)
{
  DRV_UART_Transmit(DRV_UART_GetHandle(DRV_UART2),
                    (uint8_t *)&character, 1, 0xFFFF);
}
