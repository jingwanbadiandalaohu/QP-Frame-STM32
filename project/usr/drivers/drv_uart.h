/**
 * @file drv_uart.h
 * @brief UART driver abstraction layer (platform-independent interface)
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UART_Device UART_Device_t;

typedef enum
{
  DRV_UART1,
  DRV_UART2,
  DRV_UART_MAX
} DRV_UART_Instance_t;

typedef enum
{
  DRV_UART_PARITY_NONE,
  DRV_UART_PARITY_EVEN,
  DRV_UART_PARITY_ODD
} DRV_UART_Parity_t;

typedef enum
{
  DRV_UART_STOPBITS_1,
  DRV_UART_STOPBITS_2
} DRV_UART_StopBits_t;

typedef struct
{
  DRV_UART_Instance_t instance;
  uint32_t baudrate;
  uint8_t data_bits;
  DRV_UART_StopBits_t stop_bits;
  DRV_UART_Parity_t parity;
} DRV_UART_Config_t;

/* Return codes */
#ifndef DRV_OK
#define DRV_OK          0
#define DRV_ERROR      -1
#define DRV_BUSY       -2
#define DRV_TIMEOUT    -3
#endif

typedef struct UART_Operations
{
  int (*init)(UART_Device_t *dev, DRV_UART_Config_t *config);
  int (*deinit)(UART_Device_t *dev);
  int (*transmit)(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout);
  int (*receive)(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout);
  int (*transmit_it)(UART_Device_t *dev, uint8_t *data, uint16_t len);
  int (*receive_it)(UART_Device_t *dev, uint8_t *data, uint16_t len);
} UART_Ops_t;

struct UART_Device
{
  const char *name;
  DRV_UART_Instance_t instance;
  void *hw_handle;
  UART_Ops_t *ops;
};

static inline int uart_init(UART_Device_t *dev, DRV_UART_Config_t *config)
{
  if(dev && dev->ops && dev->ops->init)
  {
    return dev->ops->init(dev, config);
  }
  return DRV_ERROR;
}

static inline int uart_deinit(UART_Device_t *dev)
{
  if(dev && dev->ops && dev->ops->deinit)
  {
    return dev->ops->deinit(dev);
  }
  return DRV_ERROR;
}

static inline int uart_transmit(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(dev && dev->ops && dev->ops->transmit)
  {
    return dev->ops->transmit(dev, data, len, timeout);
  }
  return DRV_ERROR;
}

static inline int uart_receive(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(dev && dev->ops && dev->ops->receive)
  {
    return dev->ops->receive(dev, data, len, timeout);
  }
  return DRV_ERROR;
}

static inline int uart_transmit_it(UART_Device_t *dev, uint8_t *data, uint16_t len)
{
  if(dev && dev->ops && dev->ops->transmit_it)
  {
    return dev->ops->transmit_it(dev, data, len);
  }
  return DRV_ERROR;
}

static inline int uart_receive_it(UART_Device_t *dev, uint8_t *data, uint16_t len)
{
  if(dev && dev->ops && dev->ops->receive_it)
  {
    return dev->ops->receive_it(dev, data, len);
  }
  return DRV_ERROR;
}

int uart_read_byte(UART_Device_t *dev, uint8_t *out);
void drv_uart_irq_handler(UART_Device_t *dev);

/* Platform device instances */
extern UART_Device_t *drv_uart1;
extern UART_Device_t *drv_uart2;

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
