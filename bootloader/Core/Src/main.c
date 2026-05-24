/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include <stdarg.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t usb_tx_buf[USB_BUF_LEN];
uint8_t usb_rx_buffer[BL_RX_LEN];
extern USBD_HandleTypeDef hUsbDeviceFS;
extern CDC_RX_STATE cdc_rx_state;
extern uint8_t data_len;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_USB_DEVICE_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(2000);
  print_msg("BL_DEBUG_MSG: Hello from Bootloader!!!\r\n");

  if ( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET )
  {
    print_msg("BL_DEBUG_MSG: Button is pressed .. going to BL mode.\r\n");
    bootloader_usb_read_data();
  } else
  {
    print_msg("BL_DEBUG_MSG: Button is not pressed .. executing user app.\r\n");
    bootloader_jump_to_user_app();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : I2S3_WS_Pin */
  GPIO_InitStruct.Pin = I2S3_WS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(I2S3_WS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_SCK_Pin SPI1_MISO_Pin SPI1_MOSI_Pin */
  GPIO_InitStruct.Pin = SPI1_SCK_Pin|SPI1_MISO_Pin|SPI1_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : I2S3_MCK_Pin I2S3_SCK_Pin I2S3_SD_Pin */
  GPIO_InitStruct.Pin = I2S3_MCK_Pin|I2S3_SCK_Pin|I2S3_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Audio_SCL_Pin Audio_SDA_Pin */
  GPIO_InitStruct.Pin = Audio_SCL_Pin|Audio_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void bootloader_usb_read_data(void)
{
  // Start receiving data from USB
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &usb_rx_buffer[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  while (1)
  {
    if (cdc_rx_state == CDC_RX_STATE_IDLE) continue;

    uint8_t len = usb_rx_buffer[0];
    uint32_t host_crc = *((uint32_t *) (usb_rx_buffer + len - 4) );

    VERIFY_CRC crc_valid = bootloader_verify_crc(&usb_rx_buffer[0], len - 4, host_crc);

    if (crc_valid == VERIFY_CRC_ERROR)
    {
      bootloader_send_nack();
      cdc_rx_state = CDC_RX_STATE_IDLE;
      continue;
    };

    // If CRC matches we send ACK byte
    print_msg("BL_DEBUG_MSG: checksum success !!\r\n");
    bootloader_send_ack();

    

    const BL_RX_CMD cmd = (BL_RX_CMD) usb_rx_buffer[1];

    switch (cmd)
    {
    case BL_GET_VER:
      bootloader_handle_getver_cmd(usb_rx_buffer);
      break;
    case BL_GET_HELP:
      bootloader_handle_gethelp_cmd(usb_rx_buffer);
      break;
    case BL_GET_CID:
      bootloader_handle_getcid_cmd(usb_rx_buffer);
      break;
    case BL_GET_RDP_STATUS:
      bootloader_handle_getrdp_cmd(usb_rx_buffer);
      break;
    case BL_GO_TO_ADDR:
      bootloader_handle_go_cmd(usb_rx_buffer);
      break;
    case BL_FLASH_ERASE:
      bootloader_handle_flash_erase_cmd(usb_rx_buffer);
      break;
    case BL_MEM_WRITE:
      bootloader_handle_mem_write_cmd(usb_rx_buffer);
      break;
    case BL_EN_RW_PROTECT:
      bootloader_handle_en_rw_protect(usb_rx_buffer);
      break;
    case BL_MEM_READ:
      bootloader_handle_mem_read(usb_rx_buffer);
      break;
    case BL_READ_SECTOR_P_STATUS:
      bootloader_handle_read_sector_protection_status(usb_rx_buffer);
      break;
    case BL_OTP_READ:
      bootloader_handle_read_otp(usb_rx_buffer);
      break;
    case BL_DIS_R_W_PROTECT:
      bootloader_handle_dis_rw_protect(usb_rx_buffer);
      break;
    default:
      print_msg("BL_DEBUG_MSG:Invalid command code received from host \r\n");
      break;
    }

    cdc_rx_state = CDC_RX_STATE_IDLE;
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &usb_rx_buffer[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  }

}
void bootloader_jump_to_user_app(void)
{

  // Function pointer to hold the address of the reset handler
  void (*app_reset_handler)(void) __attribute__((noreturn));

  print_msg("BL_DEBUG_MSG: bootloader_jump_to_user_app\r\n");

  // Configure MSP to be the base address of application stack
  // Which is the first value in the start of the application flash sector
  uint32_t msp_value = *(volatile uint32_t *) FLASH_SECTOR_3_BASE_ADDRESS;
  print_msg("BL_DEBUG_MSG: MSP value: %#lx\r\n", msp_value);

  // Fetch reset handler address of application
  // Which is the second value in the stert of the application flash sector
  uint32_t reset_handler_address = *(volatile uint32_t *) ( FLASH_SECTOR_3_BASE_ADDRESS + 4 );

  app_reset_handler = (void *) reset_handler_address;

  print_msg("BL_DEBUG_MSG: app reset handler address: %#lx\r\n", reset_handler_address);
  HAL_Delay(100);              // let print drain

  USBD_DeInit(&hUsbDeviceFS);
  HAL_DeInit();
  HAL_RCC_DeInit();   // switch back to HSI, disable PLL/HSE

  SysTick->CTRL = 0;

  // Clearing NVIC
  for (int i = 0; i < 8; i++) {
    NVIC->ICER[i] = 0xFFFFFFFFU;   // disable all
    NVIC->ICPR[i] = 0xFFFFFFFFU;   // clear pending
  }

  SCB->VTOR = FLASH_SECTOR_3_BASE_ADDRESS;
  __DSB(); __ISB();

  __set_MSP(msp_value);        // set MSP LAST, just before jumping
  // Jump to reset handler of the application
  app_reset_handler();
}

void print_msg(const char *fmt, ...)
{
  char str[80];

  va_list args;
  va_start(args, fmt);
  vsprintf(str, fmt, args);
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
  va_end(args);
}

void bootloader_handle_getver_cmd(uint8_t *bl_rx_buffer)
{

}
void bootloader_handle_gethelp_cmd(uint8_t *pBuffer)
{

}
void bootloader_handle_getcid_cmd(uint8_t *pBuffer)
{

}
void bootloader_handle_getrdp_cmd(uint8_t *pBuffer)
{

}
void bootloader_handle_go_cmd(uint8_t *pBuffer)
{

}
void bootloader_handle_flash_erase_cmd(uint8_t *pBuffer)
{

}
void bootloader_handle_mem_write_cmd(uint8_t *pBuffer)
{

}
void bootloader_handle_en_rw_protect(uint8_t *pBuffer)
{

}
void bootloader_handle_mem_read (uint8_t *pBuffer)
{

}
void bootloader_handle_read_sector_protection_status(uint8_t *pBuffer)
{

}
void bootloader_handle_read_otp(uint8_t *pBuffer)
{

}
void bootloader_handle_dis_rw_protect(uint8_t *pBuffer)
{

}

void bootloader_send_ack(void)
{
  uint8_t ack = BL_ACK;
  CDC_Transmit_FS(&ack, 1);
}

void bootloader_send_nack(void)
{
  uint8_t nack = BL_NACK;
  CDC_Transmit_FS(&nack, 1);
}

VERIFY_CRC bootloader_verify_crc(uint8_t *pBuffer, uint32_t len, uint32_t crc_host)
{
  __HAL_CRC_DR_RESET(&hcrc);
  uint32_t uw_crc_value = 0xffffffff;

  for (uint32_t i = 0; i < len; i ++)
  {
    uint32_t i_data = pBuffer[i];
    uw_crc_value = HAL_CRC_Accumulate(&hcrc, &i_data, 1);
  }

  if (uw_crc_value != crc_host) return VERIFY_CRC_ERROR;

  return VERIFY_CRC_SUCCESS;
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
