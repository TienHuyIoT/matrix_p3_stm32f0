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
#include "matrix_p3.h"
#include "my_printf.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
uint8_t REV;
struct str_uart
{
	char data[200];
	char flag_finish;
};
#define NUM_BUFF_UART_TEMP 2
struct str_uart UART_RECIVE[NUM_BUFF_UART_TEMP];

struct
{
	uint8_t line;
	uint8_t pause_run;
	uint8_t data[200];
	uint16_t packit; //chỉ lấy 3byte 000-999
} Frame_01;			 /*nhận chuỗi dữ liệu hiển thị*/
struct
{
	uint8_t lenght;
	uint8_t cmd;
	uint8_t data[200];
	uint8_t crc;
} Frame_All;

void get_string_handle(void);
void get_byte_uart(uint8_t c);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, &REV, 1);
	get_byte_uart(REV);
}

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
	MX_USART1_UART_Init();

	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_UART_Receive_IT(&huart1, &REV, 1);
	HAL_IWDG_Start(&hiwdg);

	debug_msg("start kaka...........\r");
	matrix_refesh(0);
	matrix_refesh(1);
	
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
		get_string_handle();
		matrix_process();
		HAL_Delay(30);
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

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
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
	char buff[100] = {0};
	uint8_t i, crc = 0;
	sprintf(buff, "%c%c%d%d%d%03d", 8, 1, line, pause_run, status, packit);
	for (i = 0; i < strlen(buff); i++)
		crc ^= buff[i];
	buff[strlen(buff)] = crc;
	printf_cmd("\x7E%s\x7F", buff);
}
//
void get_string_handle(void)
{
	static uint8_t buff_num = 0;
	static uint16_t packit_old = 0;
	uint8_t i;
	uint8_t crc;

	if (UART_RECIVE[buff_num].flag_finish == 1)
	{
		UART_RECIVE[buff_num].flag_finish = 0;

		/*sao chep dữ liệu qua frame trung tâm*/
		debug_msg("%s\r", UART_RECIVE[buff_num].data);
		memcpy((uint8_t *)&Frame_All, UART_RECIVE[buff_num].data, strlen(UART_RECIVE[buff_num].data));

		/*kiểm tra độ dài của dữ liệu*/
		debug_msg("lenght: %d-%d\r", Frame_All.lenght, strlen((char *)Frame_All.data) + 1); //1 là CMD, crc đã tính trong strlen
		if (Frame_All.lenght != strlen((char *)Frame_All.data) + 1)							//1 là CMD, crc đã tính trong strlen
		{
			report_to_app(0, 0, 1, 0);
			return;
		}
		/*kiểm tra CMD*/
		debug_msg("cmd: %d\r", Frame_All.cmd);
		if (Frame_All.cmd == 0)
		{
			report_to_app(0, 0, 2, 0);
			return;
		}
		/*kiểm tra CRC*/
		crc = 0;
		crc ^= Frame_All.lenght;
		crc ^= Frame_All.cmd;
		for (i = 0; i < (Frame_All.lenght - 3); i++) //3 là CMD, lenght, crc
			crc ^= Frame_All.data[i];
		Frame_All.crc = Frame_All.data[strlen((char *)Frame_All.data) - 1]; //1 là crc
		Frame_All.data[strlen((char *)Frame_All.data) - 1] = 0;				//1 là crc
		debug_msg("crc: %02X-%02X\r", Frame_All.crc, crc);
		if (Frame_All.crc != crc)
		{
			report_to_app(0, 0, 4, 0);
			return;
		}

		/*sao chép dữ liệu qua frame điều khiển matrix*/
		debug_msg("%s\r", (char *)Frame_All.data);
		memcpy((uint8_t *)&Frame_01, Frame_All.data, strlen((char *)Frame_All.data));
		/*lấy dữ liệu cmd dòng*/
		Frame_01.line -= 48;
		/*lấy dữ liệu cmd pause run*/
		Frame_01.pause_run -= 48;
		/*lấy dữ liệu cmd packit*/
		Frame_01.packit = 0;
		for (i = strlen((char *)Frame_01.data) - 3; i < strlen((char *)Frame_01.data); i++)
		{
			Frame_01.packit *= 10;
			Frame_01.packit += Frame_01.data[i] - 48;
			debug_msg("packit %d: %d-%c\r", i, Frame_01.packit, Frame_01.data[i]);
		}

		/*xóa dữ liệu packit ra khỏi data*/
		Frame_01.data[strlen((char *)Frame_01.data) - 3] = 0;

		/*kiểm tra lệnh điều khiển dòng*/
		debug_msg("line: %d\r", Frame_01.line);
		if (Frame_01.line != 1 && Frame_01.line != 2)
		{
			report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
			return;
		}
		/*kiểm tra hiệu ứng chạy dừng*/
		debug_msg("pause_run: %d\r", Frame_01.pause_run);
		if (Frame_01.pause_run != 0 && Frame_01.pause_run != 1)
		{
			report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
			return;
		}
		/*kiểm tra packit*/
		debug_msg("packit: %d\r", Frame_01.packit);
		if (Frame_01.packit == packit_old)
		{
			report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
			return;
		}
		/*kiểm tra dữ liệu quá dài so với màng hình*/
		if (font_get_lenght_pixel((char*)Frame_01.data) == 0 && Frame_01.pause_run == 0)
		{
			report_to_app(Frame_01.line, Frame_01.pause_run, 3, Frame_01.packit);
			return;
		}
		/*dữ liệu được đưa ra màng hình*/
		packit_old = Frame_01.packit;
		debug_msg("data: %s\r", Frame_01.data);
		matrix_send(Frame_01.line - 1, (char *)Frame_01.data, Frame_01.pause_run);
		report_to_app(Frame_01.line, Frame_01.pause_run, 0, Frame_01.packit);
	}

	if (++buff_num >= NUM_BUFF_UART_TEMP)
		buff_num = 0;
}
//

void get_byte_uart(uint8_t ch)
{
	static uint8_t buff_num = 0, index_buff = 0;
	static uint8_t flag_ready = 0;

	/*kiểm tra ký tự start*/
	if (ch == 0x7E)
	{
		index_buff = 0;
		flag_ready = 1;
		return;
	}
	/*kiểm tra ký tự stop*/
	else if (ch == 0x7F)
	{
		flag_ready = 0;
		UART_RECIVE[buff_num].flag_finish = 1;
		if (++buff_num == NUM_BUFF_UART_TEMP)
			buff_num = 0;

		return;
	}
	/*lấy data*/
	if (flag_ready)
	{
		UART_RECIVE[buff_num].data[index_buff] = ch;
		UART_RECIVE[buff_num].data[++index_buff] = 0;
	}
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
