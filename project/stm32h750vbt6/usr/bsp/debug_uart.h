#ifndef DEBUG_UART_H 
#define DEBUG_UART_H 

/* ==================== [Includes] ========================================== */
#include "stm32h7xx_hal.h"


#ifdef __cplusplus
extern "C"{
#endif

/* ==================== [Defines] ========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================== */
void elab_debug_uart_init(uint32_t baudrate);
void elab_debug_uart2_init(uint32_t baudrate);
int debug_uart1_read(uint8_t *out);
int debug_uart2_read(uint8_t *out);
/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
}
#endif

#endif	/* DEBUG_UART_H */

