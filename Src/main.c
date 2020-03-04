/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */

//#include "stm32f0xx_hal_gpio.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "matrix_p3.h"
#include "my_printf.h"
#include "uart.h"
#include "command_uart.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM3_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

struct
{
	uint8_t line;
	uint8_t pause_run;
	uint8_t data[201];
	uint16_t packit; //chỉ lấy 3byte 000-999
} Frame_01;			 /*nhận chuỗi dữ liệu hiển thị*/

void get_string_handle(char *data, command_error_typdef error);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t cnt_blink;
	if (htim->Instance == htim3.Instance)
	{
		if (cnt_blink++ > 300)
		{
			cnt_blink = 0;
			HAL_GPIO_TogglePin(GPIOA, LED1_Pin);
		}

		matrix_scan_show();
	}
}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{
	/* USER CODE BEGIN 1 */
	//   int i;
	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_IWDG_Init();
	MX_TIM3_Init();
	//MX_USART1_UART_Init();
	LLD_USART1_Init(9600);

	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim3);
	
	HAL_IWDG_Start(&hiwdg);

	USART1_PRINTF("\r\nHello matrix Fans!\r\n");
	matrix_refesh(0);
	matrix_refesh(1);
	
	command_register_callback(get_string_handle);
	
	//matrix_send(0, "Cao van teo em", 1);
	//matrix_send(1, "Cao Van Tuan", 0);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_IWDG_Refresh(&hiwdg);
		get_command_uart();
		matrix_process();
	}
	/* USER CODE END 3 */
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks 
    */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/**Initializes the CPU, AHB and APB busses clocks 
    */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	/**Configure the Systick interrupt time 
    */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick 
    */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* IWDG init function */
static void MX_IWDG_Init(void)
{

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
	hiwdg.Init.Window = 4095;
	hiwdg.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		Error_Handler();
	}
}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 48;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 550;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LED1_Pin | LATCH_Pin | CLK_Pin | E_Pin | R0_Pin | A_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, OE_Pin | R1_Pin | G1_Pin | B_Pin | C_Pin | D_Pin | R2_Pin | G2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : LED1_Pin LATCH_Pin CLK_Pin E_Pin 
                           R0_Pin A_Pin */
	GPIO_InitStruct.Pin = LED1_Pin | LATCH_Pin | CLK_Pin | E_Pin | R0_Pin | A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : OE_Pin R1_Pin G1_Pin B_Pin C_Pin D_Pin
                           R2_Pin G2_Pin */
	GPIO_InitStruct.Pin = OE_Pin | R1_Pin | G1_Pin | B_Pin | C_Pin | D_Pin | R2_Pin | G2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void report_to_app(uint8_t line, uint8_t pause_run, uint8_t status, uint16_t packit)
{
	char buff[250] = {0};
	uint8_t i, crc = 0;
	sprintf(buff, "%c%c%u%u%u%03u", 8, 1, line, pause_run, status, packit);
	for (i = 0; i < strlen(buff); i++)
	{
		crc ^= buff[i];
	}		
	buff[strlen(buff)] = crc;
	
	putchar_usart1(0x7E);
	for(i = 0; i < (strlen(buff) + 1); i++)
	{
		putchar_usart1(buff[i]);
	}	
	putchar_usart1(0x7F);
	
	debug_msg("Response\r\n");
}

/*
$7E$0C$0110Hello005$7B$7F
$7E$0C$0111Hello005$7A$7F

$7E$21$0111Welcome to MT LED Test App005$57$7F
$7E$21$0110Welcome to MT LED Test App005$56$7F

$7E$16$012129-2-2010 15:30005$06$7F
$7E$16$012029-2-2010 15:30005$07$7F

$7E$65$0111 !"##$$%&'()*+,-./123456789:;<<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~005$1E$7F
$7E$65$0110 !"##$$%&'()*+,-./123456789:;<<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~005$1F$7F

$7E$65$0121 !"##$$%&'()*+,-./123456789:;<<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~005$1D$7F
$7E$65$0120 !"##$$%&'()*+,-./123456789:;<<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~005$1C$7F

 !"#$%&'()*+,-./123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~

*/
void get_string_handle(char *data, command_error_typdef error)
{
//	uint8_t i;
	
	if(error != COMMAND_SUCCESS)
	{
		  debug_msg("Fail command\r\n");
		  report_to_app(0, 0, error, 0);
		  return;
	}

	/*sao chép dữ liệu qua frame điều khiển matrix*/
	//debug_msg("%s\r", (char *)data);
	memcpy((uint8_t *)&Frame_01, data, strlen((char *)data));
	/*lấy dữ liệu cmd dòng*/
	Frame_01.line = (data[0] - '0');
	/*lấy dữ liệu cmd pause run*/
	Frame_01.pause_run = (data[1] - '0');
	memset(Frame_01.data, 0, 201);
	memcpy(Frame_01.data, &data[2], strlen(data) - 5);
	/*lấy dữ liệu cmd packit*/
	Frame_01.packit = atoi(&data[strlen(data) - 3]);
	
	uint8_t leng = strlen((char *)Frame_01.data);
	debug_msg("Data %u: %s\r\n", leng, (char *)Frame_01.data);
	
//	for (i = leng - 3; i < leng; i++)
//	{
//		Frame_01.packit *= 10;
//		Frame_01.packit += (Frame_01.data[i] - 48);		
//	}

	debug_msg("Line %u, run %u, p_id %u",Frame_01.line, Frame_01.pause_run, Frame_01.packit);

	/*xóa dữ liệu packit ra khỏi data*/
	//Frame_01.data[strlen((char *)Frame_01.data) - 3] = 0;

	/*kiểm tra lệnh điều khiển dòng*/
	//debug_msg("line: %d\r", Frame_01.line);
	if (Frame_01.line != 1 && Frame_01.line != 2)
	{
		debug_msg("Fail line\r\n");
		report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
		return;
	}
	/*kiểm tra hiệu ứng chạy dừng*/
	//debug_msg("pause_run: %d\r", Frame_01.pause_run);
	if (Frame_01.pause_run != 0 && Frame_01.pause_run != 1)
	{
		debug_msg("Fail run\r\n");
		report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
		return;
	}
	/*kiểm tra packit*/
	//debug_msg("packit: %d\r", Frame_01.packit);

	/*kiểm tra dữ liệu quá dài so với màng hình*/
//	if (font_get_lenght_pixel((char*)Frame_01.data) == 0 && Frame_01.pause_run == 0)
//	{
//		debug_msg("Fail lenght pixel\r\n");
//		report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
//		return;
//	}
	/*dữ liệu được đưa ra màng hình*/
	debug_msg("OK\r\n");
	matrix_send(Frame_01.line - 1, (char *)Frame_01.data, Frame_01.pause_run);
	report_to_app(Frame_01.line, Frame_01.pause_run, COMMAND_SUCCESS, Frame_01.packit);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
	{
	}
	/* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
