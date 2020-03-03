#ifndef __MY_PRINTF_H__
#define __MY_PRINTF_H__

#include "uart.h"

/* Private macro -------------------------------------------------------------*/
#define printf_cmd(f_, ...) 					USART1_PRINTF(f_, ##__VA_ARGS__)

#define TAG				"MSG"
#define debug_msg(f_, ...)       			BOARD_NEO_LOG(TAG, f_, ##__VA_ARGS__)


#endif //__MY_PRINTF_H__
/*THE END*/
