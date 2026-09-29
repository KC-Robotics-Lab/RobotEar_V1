/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PCMD3180.h"
#include "AcousticSL_Process.h"
#include "acoustic_sl.h"
#include "LED.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include "usbd_def.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define FRAME_SIZE_MS                (AUDIO_IN_SAMPLING_FREQUENCY / 1000U)    // 채널당 1ms 샘플 수
#define PCM_FRAME_SIZE               (AUDIO_IN_CHANNELS * FRAME_SIZE_MS)      // 1ms 전체(4ch)
#define SAI_RX_BUF_LEN               (PCM_FRAME_SIZE * 2U)   // half + full

#define CACHE_LINE 32

__attribute__((section(".RAM_D2"), aligned(CACHE_LINE)))
uint16_t SAI_L_Rx_Buffer[SAI_RX_BUF_LEN];

__attribute__((section(".RAM_D2"), aligned(CACHE_LINE)))
uint16_t SAI_R_Rx_Buffer[SAI_RX_BUF_LEN];

__attribute__((aligned(CACHE_LINE)))
uint16_t sai_safe_buf[PCM_FRAME_SIZE];

__attribute__((aligned(CACHE_LINE)))
uint16_t sai_safe_buf1[PCM_FRAME_SIZE];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t sai_rx_half_flag = 0;
volatile uint8_t sai_rx_full_flag = 0;

extern UART_HandleTypeDef huart1;   // 이미 usart.c에 있음

uint8_t rx_byte;                    // 1바이트 수신 버퍼
char cmd_buf[64];                   // 명령어 버퍼
uint8_t cmd_idx = 0;
volatile uint8_t cmd_ready = 0;     // 한 줄 완성 플래그


volatile uint8_t SAI_HALF_L = 0;
volatile uint8_t SAI_HALF_R = 0;
volatile uint8_t SAI_FULL_L = 0;
volatile uint8_t SAI_FULL_R = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void SAI1_DMA_Start(void);
void USART1_INT_Start(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int fd, char *ptr, int len)
{
  (void)fd;
  CDC_Transmit_HS((uint8_t*)ptr, len);
  HAL_Delay(1);
  return len;
}

uint8_t CDC_Transmit_HS_Blocking(uint8_t *Buf, uint16_t Len, uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();
    uint8_t  result;

    do
    {
        result = CDC_Transmit_HS(Buf, Len);
        if (result == USBD_OK)
        {
            return USBD_OK;   // 성공
        }

        // BUSY면 잠깐 쉬어줌 (IRQ가 도는 시간 확보)
        if (result == USBD_BUSY)
        {
            // 너무 오래 기다리는 건 방지
            if ((HAL_GetTick() - start) > timeout_ms)
            {
                return USBD_BUSY;  // 타임아웃
            }
            HAL_Delay(1);
        }
        else
        {
            // USBD_FAIL 등
            return result;
        }

    } while (1);
}

void SAI1_DMA_Start(void)
{
	if (HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t*)SAI_L_Rx_Buffer, SAI_RX_BUF_LEN) != HAL_OK)
		Error_Handler();
	if (HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t*)SAI_R_Rx_Buffer, SAI_RX_BUF_LEN) != HAL_OK)
		Error_Handler();
}

void USART1_INT_Start(void)
{
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

void uart_send_str(const char *s)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)s, strlen(s), 100);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_SAI1_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_I2C4_Init();
  /* USER CODE BEGIN 2 */
  LED_L_Init();
  LED_R_Init();
  USART1_INT_Start();
  pcmd3180_init();

  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  HAL_NVIC_DisableIRQ(EXTI1_IRQn);

  Audio_Libraries_L_Init();
  Audio_Libraries_R_Init();

  HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
  HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  SAI1_DMA_Start();

  printf("start\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(SAI_HALF_L)
	  {
		  SAI_HALF_L = 0;
		  AudioProcess_L();
	  }

	  if(SAI_FULL_L)
	  {
		  SAI_FULL_L = 0;
		  AudioProcess_L();
	  }

	  if(SAI_HALF_R)
	  {
		  SAI_HALF_R = 0;
		  AudioProcess_R();
	  }

	  if(SAI_FULL_R)
	  {
		  SAI_FULL_R = 0;
		  AudioProcess_R();
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#ifdef uart1_cli
	  if (cmd_ready)
	  {
		cmd_ready = 0;
		handle_command(cmd_buf);
		uart_send_str("> ");
	  }
#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLL3.PLL3M = 14;
  PeriphClkInitStruct.PLL3.PLL3N = 139;
  PeriphClkInitStruct.PLL3.PLL3P = 13;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 6357;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    // 개행 문자 들어오면 명령 한 줄 완성
    if (rx_byte == '\r' || rx_byte == '\n')
    {
      if (cmd_idx > 0)
      {
        cmd_buf[cmd_idx] = '\0';  // 문자열 종료
        cmd_ready = 1;
        cmd_idx = 0;
        uart_send_str("\r\n");
      }
      else
      {
        // 빈 줄일 때는 그냥 프롬프트만 내보냄
        uart_send_str("\r\n> ");
      }
    }
    else
    {
      // 일반 문자일 때 버퍼에 저장
      if (cmd_idx < sizeof(cmd_buf) - 1)
      {
        cmd_buf[cmd_idx++] = rx_byte;
        // 에코 (입력한 문자 다시 보내기)
        HAL_UART_Transmit(&huart1, &rx_byte, 1, 10);
      }
    }

    // 다음 1바이트 수신 재시작
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  }
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	if (hsai->Instance == SAI1_Block_A)
	{
		SCB_InvalidateDCache_by_Addr((uint32_t*)&SAI_L_Rx_Buffer[0], PCM_FRAME_SIZE * sizeof(uint16_t));
		Audio_FillFromI2S(&SAI_L_Rx_Buffer[0], 4, PCM_Buffer_L);
//		AudioProcess_L();
		SAI_HALF_L = 1;

	}

	if (hsai->Instance == SAI1_Block_B)
	{
		SCB_InvalidateDCache_by_Addr((uint32_t*)&SAI_R_Rx_Buffer[0], PCM_FRAME_SIZE * sizeof(uint16_t));
		Audio_FillFromI2S(&SAI_R_Rx_Buffer[0], 4, PCM_Buffer_R);
//		AudioProcess_R();
		SAI_HALF_R = 1;
	}
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	if (hsai->Instance == SAI1_Block_A)
	{
		SCB_InvalidateDCache_by_Addr((uint32_t*)&SAI_L_Rx_Buffer[PCM_FRAME_SIZE], PCM_FRAME_SIZE * sizeof(uint16_t));
		Audio_FillFromI2S(&SAI_L_Rx_Buffer[PCM_FRAME_SIZE], 4, PCM_Buffer_L);
//		AudioProcess_L();
		SAI_FULL_L = 1;
	}

	if (hsai->Instance == SAI1_Block_B)
	{
		SCB_InvalidateDCache_by_Addr((uint32_t*)&SAI_R_Rx_Buffer[PCM_FRAME_SIZE], PCM_FRAME_SIZE * sizeof(uint16_t));
		Audio_FillFromI2S(&SAI_R_Rx_Buffer[PCM_FRAME_SIZE], 4, PCM_Buffer_R);
//		AudioProcess_R();
		SAI_FULL_R = 1;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
