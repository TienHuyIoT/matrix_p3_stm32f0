#ifndef __MY_PRINTF_H__
#define __MY_PRINTF_H__

#include "stm32f0xx_hal.h"

extern UART_HandleTypeDef huart1;

#define debug_msg(fmt, ...) //printf_uart("\r\n" fmt, ##__VA_ARGS__);
#define printf_cmd(fmt, ...) printf_uart(fmt, ##__VA_ARGS__);

void printf_uart(const char *fmt, ...);

#endif //__MY_PRINTF_H__
/*THE END*/
