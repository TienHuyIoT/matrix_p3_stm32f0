/**
  ******************************************************************************
  * @file    uart.c
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    11-march-2019
  * @brief   device mounted on STM32H743I-NEO boards.
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "uart.h"

/** @defgroup _UART_Private_Variables UART Private Variables
  * @{
  */
UART_HandleTypeDef usart1Handle;
volatile UART_Printf_t lld_uart_printf;

/* Tx handle buff UART1 */
static uint8_t usart1_Tx_Fifo_buffer[USART1_TX_FIFO_BUFF_SIZE];
static uint8_t usart1_Tx_Irq_buffer[USART1_TX_IRQ_BUFF_SIZE];
static FIFO_Handle_t FifoUsart1_Tx = {
	0,
	0,
	USART1_TX_FIFO_BUFF_SIZE,
	usart1_Tx_Fifo_buffer,
	FIFO_IRQ_STATE_READY,
	FIFO_UNLOCKED,
	USART1_TX_IRQ_BUFF_SIZE,
	usart1_Tx_Irq_buffer
};

/* Rx handle buff USART1 */
static uint8_t usart1_Rx_Fifo_buffer[USART1_RX_FIFO_BUFF_SIZE];
static uint8_t usart1_Rx_Irq_buffer[USART1_RX_IRQ_BUFF_SIZE];
static FIFO_Handle_t FifoUart1_Rx = {
	0,
	0,
	USART1_RX_FIFO_BUFF_SIZE,
	usart1_Rx_Fifo_buffer,
	FIFO_IRQ_STATE_READY,
	FIFO_UNLOCKED,
	USART1_RX_IRQ_BUFF_SIZE,
	usart1_Rx_Irq_buffer
};

/**
  * @}
  */

/** @defgroup _UART_Private_Function_Prototypes UART Private Function Prototypes
  * @{
  */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void GiveBuff(FIFO_Handle_t *hFifo, uint8_t *Buf, uint16_t Len);
static uint16_t TakeBuff(FIFO_Handle_t *hFifo, uint8_t *Buf, uint16_t Len);
static int16_t Buff_is_available(FIFO_Handle_t *hFifo);
static UART_Status_t putchar_uartCom(char c, UART_HandleTypeDef *huart);

/**
  * @}
  */

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
UART_Status_t LLD_USART1_Init(uint32_t Baud)
{
	UART_Status_t Result = UART_OK;
	usart1Handle.Instance = USART1;
	usart1Handle.Init.BaudRate = Baud;
	usart1Handle.Init.WordLength = UART_WORDLENGTH_8B;
	usart1Handle.Init.StopBits = UART_STOPBITS_1;
	usart1Handle.Init.Parity = UART_PARITY_NONE;
	usart1Handle.Init.Mode = UART_MODE_TX_RX;
	usart1Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	usart1Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	usart1Handle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	usart1Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&usart1Handle) != HAL_OK) {
		/* Initialization Error */
		Result = UART_ERROR;
	}

	/*##- Put UART peripheral in reception process ###########################*/
	if(HAL_UART_Receive_IT(&usart1Handle, FifoUart1_Rx.IrqBuff, FifoUart1_Rx.IrqSize) != HAL_OK)
	{
		Result = UART_ERROR;
	}

	return Result;
}

/**
  * @brief  DeInitializes the UART device.
  * @retval UART status
  */
UART_Status_t LLD_USART1_DeInit(void)
{
  UART_Status_t Result = UART_ERROR;
  /* UART device de-initialization */
  usart1Handle.Instance = USART1;

  if(HAL_UART_DeInit(&usart1Handle) != HAL_OK)
  {
    Result = UART_ERROR;
  }
  else
  {
    Result = UART_OK;
  }

  return Result;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	if (lld_uart_printf & PRINTF_USART1) {
		putchar_uartCom(ch, &usart1Handle);
	}
	return ch;
}

/**
  * @brief give one character to Tx buff usart1
  * @param  none
  * @retval UART_OK / UART_ERROR
  */
UART_Status_t putchar_usart1(char c)
{
	return putchar_uartCom(c, &usart1Handle);
}

/**
  * @brief give one character to Tx buff
  * @param  c	charactre Tx
  * @param  huart: UART handle.
  * @retval UART_OK / UART_ERROR
  */
static UART_Status_t putchar_uartCom(char c, UART_HandleTypeDef *huart)
{
	int16_t Leng;

	FIFO_Handle_t *Fifo_Handle;

	if(huart->Instance==USART1)
	{
		Fifo_Handle = &FifoUsart1_Tx;
	}
	else
	{
		return UART_ERROR;
	}

	/* Process Locked */
	__FIFO_LOCK(Fifo_Handle);

	/* Add character on buff */
	GiveBuff(Fifo_Handle, (uint8_t*)&c, 1);

	/* can send */
	if(Fifo_Handle->IrqState == FIFO_IRQ_STATE_READY)
	{
		Leng = TakeBuff(Fifo_Handle, Fifo_Handle->IrqBuff, Fifo_Handle->IrqSize);
		/*##-2- Start the transmission process #####################################*/
		if(HAL_UART_Transmit_IT(huart, Fifo_Handle->IrqBuff, Leng) != HAL_OK)
		{
			/* Transfer error in transmission process */
			return UART_ERROR;
		}
		Fifo_Handle->IrqState = FIFO_IRQ_STATE_BUSY;
	}

	/* Process Unlocked */
	__FIFO_UNLOCK(Fifo_Handle);

	return UART_OK;
}

/**
  * @brief get one character from Rx buff usart1
  * @param  none
  * @retval character
  */
UART_Status_t getchar_usart1(char *c)
{
    int16_t Leng;
    Leng = TakeBuff(&FifoUart1_Rx, (uint8_t*)c, 1);
    return (Leng)? UART_OK : UART_ERROR;
}

/**
  * @brief check rx buff usart1
  * @param  none
  * @retval size real buff available
  */
int16_t Usart1_is_available(void){
	return Buff_is_available(&FifoUart1_Rx);
}

/**
  * @brief Give Buff Fifo
  * @param  hFifo 	FIFO_Handle_t Handle
  * @param  Buf 	Pointer Buf data
  * @param  Buf 	Size Buf data
  * @retval None
  */
static void GiveBuff(FIFO_Handle_t *hFifo, uint8_t *Buf, uint16_t Len)
{
	for (uint16_t i = 0; i < Len; i++)
	{
		hFifo->FifoBuff[hFifo->Wr_Index++] = Buf[i];
		if (hFifo->Wr_Index >= hFifo->FifoSize)
		{
			hFifo->Wr_Index = 0;
		}
	}
}

/**
  * @brief Take buff Fifo
  * @param  hFifo 	FIFO_Handle_t Handle
  * @param  Buf 	Pointer Buf data
  * @param  Buf 	Size Buf data
  * @retval size real buff
  */
static uint16_t TakeBuff(FIFO_Handle_t *hFifo, uint8_t *Buf, uint16_t Len)
{
	uint16_t i;
	for (i = 0; i < Len; i++)
	{
		if (hFifo->Wr_Index == hFifo->Rd_Index)
			break;
		Buf[i] = hFifo->FifoBuff[hFifo->Rd_Index++];
		if (hFifo->Rd_Index >= hFifo->FifoSize)
		{
			hFifo->Rd_Index = 0;
		}
	}
	return i;
}

/**
  * @brief check buff counter
  * @param  hFifo 	FIFO_Handle_t Handle
  * @retval size real buff available
  */
static int16_t Buff_is_available(FIFO_Handle_t *hFifo)
{
	int16_t Num = hFifo->Wr_Index - hFifo->Rd_Index;
	Num += (Num < 0) ? hFifo->FifoSize : 0;
	return Num;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	int16_t Leng;
	FIFO_Handle_t *Fifo_Handle;

	if(huart->Instance==USART1)
	{
		Fifo_Handle = &FifoUsart1_Tx;
	}
	else
	{
		return;
	}

	/* can send continue */
	if(Fifo_Handle->Lock == FIFO_UNLOCKED)
	{
		Leng = TakeBuff(Fifo_Handle, Fifo_Handle->IrqBuff, Fifo_Handle->IrqSize);
		if(Leng > 0)
		{
			/*## - Start the transmission process #####################################*/
			HAL_UART_Transmit_IT(huart, Fifo_Handle->IrqBuff, Leng);
		}
		else
		{
			/* allow transmission from putchar() */
			Fifo_Handle->IrqState = FIFO_IRQ_STATE_READY;
		}
	}
	else
	{
		/* allow transmission from putchar_uartCom() */
		Fifo_Handle->IrqState = FIFO_IRQ_STATE_READY;
	}
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	FIFO_Handle_t *Fifo_Handle;

	if(huart->Instance==USART1)
	{
		Fifo_Handle = &FifoUart1_Rx;
	}
	else
	{
		return;
	}

	/* Enable RX_IRQ continue */
	HAL_UART_Receive_IT(huart, Fifo_Handle->IrqBuff, Fifo_Handle->IrqSize);
	/* GiveBuff Rxbuff */
	GiveBuff(Fifo_Handle, Fifo_Handle->IrqBuff, Fifo_Handle->IrqSize);
}

/**
  * @brief  UART error callbacks
  * @param  huart: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{

	}
	else
	{

	}
}
/**
  * @}
  */

/**
  * @}
  */
