/**
  ******************************************************************************
  * @file    uart.h
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    1-March-2019
  * @brief   This file contains the common defines and functions prototypes for
  *          the uart.c driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

/**
  * @brief  UART status structure definition
  */

typedef enum {
	UART_OK = 0,
	UART_ERROR
} UART_Status_t;

typedef enum {
	PRINTF_USART1 = 0x01
}UART_Printf_t;

typedef enum {
	FIFO_UNLOCKED = 0x00,
	FIFO_LOCKED = 0x01
} FIFO_LockTypeDef;

typedef enum {
	FIFO_IRQ_STATE_READY = 0x00,
	FIFO_IRQ_STATE_BUSY = 0x01
} FIFO_IrqStateTypeDef;

#define __FIFO_LOCK(__HANDLE__)                                          \
                                 do{                                       \
                                     (__HANDLE__)->Lock = FIFO_LOCKED;    \
                                   }while (0)

#define __FIFO_UNLOCK(__HANDLE__)                                          \
                                 do{                                       \
                                     (__HANDLE__)->Lock = FIFO_UNLOCKED;    \
                                   }while (0)

typedef struct {
	int16_t Wr_Index;
	int16_t Rd_Index;
	int16_t FifoSize;
	uint8_t *FifoBuff;
	uint8_t IrqState;
	uint8_t Lock;
	uint8_t IrqSize;
	uint8_t *IrqBuff;
} FIFO_Handle_t;

/** @defgroup UART_Exported_Macro UART Exported Macro
  * @{
  */

#define USART1_PRINTF(f_, ...)    do { UART_Printf_t ubk = lld_uart_printf;\
                                        lld_uart_printf = PRINTF_USART1;\
                                        printf((f_), ##__VA_ARGS__);\
                                        lld_uart_printf = ubk;\
                                      } while(0) 

#define UARTn_SELECT(x)         (lld_uart_printf = x)                                       

#define DEBUG_PRINTF            PRINTF_USART1
                                                  
#define BOARD_NEO_LOG(Tag, f_, ...)    do { UART_Printf_t ubk = lld_uart_printf;\
                                      lld_uart_printf = DEBUG_PRINTF;\
                                      printf("\r\n" Tag " " f_, ##__VA_ARGS__);\
                                      lld_uart_printf = ubk;\
                                    } while(0)

/**
 * @defgroup UART_BUFF_SIZE
 * */
/* UART1 */
#define USART1_TX_FIFO_BUFF_SIZE			512
#define USART1_TX_IRQ_BUFF_SIZE			128

#define USART1_RX_FIFO_BUFF_SIZE			512
#define USART1_RX_IRQ_BUFF_SIZE			1

/**
  * @}
  */

/** @defgroup _UART_Exported_Functions UART Exported Functions
  * @{
  */
UART_Status_t LLD_USART1_Init(uint32_t Baud);
UART_Status_t LLD_USART1_DeInit(void);

UART_Status_t putchar_usart1(char c);

UART_Status_t getchar_usart1(char *c);

int16_t Usart1_is_available(void);

extern volatile UART_Printf_t lld_uart_printf;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
