/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ws2812b.h"
#include "ws28xx_fx.h"
#include "ds18b20.h"
#include "usbd_cdc_if.h"
#include "stdio.h"
#include "ring_buffer.h"
#include "string.h"
#include "utils.h"
#include "parser_complex.h"
#include "led_blink.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CASE_OPEN_STATE HAL_GPIO_ReadPin(FRONT_SWITCH_GPIO_Port, FRONT_SWITCH_Pin)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const char Version[] = "1.0";

volatile uint8_t CommandToJump = 0;

int32_t Temp;
float Temperature;
uint8_t ds1[DS18B20_ROM_CODE_SIZE];
uint16_t FanPwm;
uint16_t FanSpeed;

blink_t CommPcUsb, ErrorBlink;

RingBuffer_t ReceiveBuffer;
RingBuffer_t TransmitBuffer;

uint8_t LineCounter;
uint8_t ReceivedData[RING_BUFFER_SIZE];
uint8_t TransmitData[RING_BUFFER_SIZE];

uint8_t DataToTransmit;

extern USBD_HandleTypeDef hUsbDeviceFS;

uint32_t OldTick500ms, OldTick100ms, OldTick50ms, OldTick10000ms;

volatile struct ErrorCode ErrorCode;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SetLedH(uint8_t Val);
void SetLedS(uint8_t Val);
void SetLedV(uint8_t Val);
void SetLedMode(uint8_t Mode);
void SetLedDelay(uint16_t Fx, uint16_t Blink);
void SetLedRevDir(uint8_t Rev);
void SetLedGamma(uint8_t Switch);

void BootloaderJump(uint16_t Code);

void IntervalFunc10000ms(void);
void IntervalFunc500ms(void);
void IntervalFunc100ms(void);
void IntervalFunc50ms(void);

//
//Bootloader jump
//
void jump_to_application(uint32_t const app_address);
void deinit_peripherals(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(TIM1 == htim->Instance)
	{
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	__asm__ volatile("ldr r10, =0x0000");
__enable_irq(); // turn on irq
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
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  OldTick500ms = HAL_GetTick();
  OldTick100ms = HAL_GetTick();
  OldTick50ms = HAL_GetTick();
  OldTick10000ms = HAL_GetTick();

  led_gamma_correction_enebled = 1;
  ws28LedFxInit(30, 100, 150, 200, 255, 2, 1);

  if (ds18b20_read_address(ds1) != HAL_OK)
  {
    ERROR_DS;
  }

  LedBlinkInit(&CommPcUsb, COMM_PC_LED_GPIO_Port, COMM_PC_LED_Pin, 20);
  LedBlinkInit(&ErrorBlink, ERROR_LED_GPIO_Port, ERROR_LED_Pin, 50);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  HAL_IWDG_Refresh(&hiwdg);





	  if(CommandToJump == 1)
	  {
		  jump_to_application(0x8000000);
	  }

	  if(LineCounter)
	  {
		  Parser_TakeLine(&ReceiveBuffer, ReceivedData);

		  LineCounter--;

		  Parser_parse(ReceivedData);
	  }
	  if(DataToTransmit > 0 )
	  {
		  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
		  if (hcdc->TxState == 0)
		  {
			  LedBlinkOne(&CommPcUsb);
			  UsbTransmitTask();

			  DataToTransmit--;
		  }

	  }


	  ws28Task();

	  LedBlinkTask(&CommPcUsb);
	  LedBlinkTask(&ErrorBlink);

	  IntervalFunc100ms();
	  IntervalFunc500ms();
	  IntervalFunc50ms();
	  IntervalFunc10000ms();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void SetLedH(uint8_t Val)
{
	ws28xxFx.h = Val;
}
void SetLedS(uint8_t Val)
{
	ws28xxFx.s = Val;
}
void SetLedV(uint8_t Val)
{
	ws28xxFx.v = Val;
}
void SetLedMode(uint8_t Mode)
{
	ws28xxFx.Mode = Mode;
}
void SetLedDelay(uint16_t Fx, uint16_t Blink)
{
	ws28xxFx.FxDelay = Fx;
	ws28xxFx.BlinkDelay = Blink;
}
void SetLedRevDir(uint8_t Rev)
{
	ws28xxFx.ReverseDirection = Rev;
}
void SetLedGamma(uint8_t Switch)
{
	led_gamma_correction_enebled = Switch;
}

void BootloaderJump(uint16_t Code)
{
	if(Code == 1234)
	CommandToJump = 1;
}

void IntervalFunc10000ms(void)
{
	if(HAL_GetTick() - OldTick10000ms >10000)
	{
		char MsgToSend[255];

		sprintf(MsgToSend, "10/0x%lx%lx%lx/%s",
				HAL_GetUIDw2(),
				HAL_GetUIDw1(),
				HAL_GetUIDw0(),
				Version);
		UsbBuffWrite(MsgToSend);
		OldTick10000ms = HAL_GetTick();
	}
}

void IntervalFunc100ms(void)
{
	if(HAL_GetTick() - OldTick100ms >100)
	{
		char MsgToSend[255];

		sprintf(MsgToSend, "0/%u/%u/%u/%u/%u/%u/%u/%u",
				ws28xxFx.h,
				ws28xxFx.s,
				ws28xxFx.v,
				ws28xxFx.Mode,
				ws28xxFx.FxDelay,
				ws28xxFx.BlinkDelay,
				ws28xxFx.ReverseDirection,
				led_gamma_correction_enebled);
		UsbBuffWrite(MsgToSend);

//		char TemperatureBuff[8];
//		Temperature100ToString(Temp, TemperatureBuff);

		sprintf(MsgToSend, "1/%.2f/%u/%u/%u/%u",
				Temperature,
				FanPwm,
				FanSpeed,
				CASE_OPEN_STATE,
				ErrorCode.Error);
		UsbBuffWrite(MsgToSend);

		OldTick100ms = HAL_GetTick();
	}
}

void IntervalFunc500ms(void)
{
	  if(HAL_GetTick() - OldTick500ms >500)
	  {
		  OldTick500ms = HAL_GetTick();

		  static uint8_t TempMeasureFlag = 0;
		  if(!TempMeasureFlag)
		  {
			  if(ds18b20_start_measure(NULL) != HAL_OK) ERROR_DS;
			  TempMeasureFlag = 1;
		  }
		  else
		  {
			  Temp = ds18b20_get_temp_wo_fp(NULL);
			  Temperature = Temp;
			  Temperature = Temperature/100;
			  TempMeasureFlag = 0;
		  }
	  }
}

void IntervalFunc50ms(void)
{
	if(HAL_GetTick() - OldTick50ms >50)
	{

		OldTick50ms = HAL_GetTick();
	}
}

void CDC_ReveiveCallback(uint8_t *Buffer, uint8_t Length)
{
	if(Length > 0)
	{
		uint8_t i = 0;
		LedBlinkOne(&CommPcUsb); //Control Led
		while(i < Length)
		{
		if (RB_OK == Ring_Buffer_Write(&ReceiveBuffer, Buffer[i]))
		{
			if(Buffer[i] == ENDLINE)
			{
				LineCounter++;
			}
		}
		else
		{
			Ring_Buffer_Flush(&ReceiveBuffer);
			UsbBuffWrite("ERROR");
		}
		i++;
		}
	}
}

void UsbBuffWrite(char * Message)
{

	DataToTransmit++;
	for(uint8_t y = 0 ; y < strlen(Message) ; y++)
	{
		if(RB_OK == Ring_Buffer_Write(&TransmitBuffer, Message[y]))
		{

		}
		else
		{
			Ring_Buffer_Flush(&TransmitBuffer);
			DataToTransmit--;
		}
	}

	if(RB_OK == Ring_Buffer_Write(&TransmitBuffer, '^'))
	{

	}
	else
	{
		Ring_Buffer_Flush(&TransmitBuffer);
		DataToTransmit--;
	}

}

void UsbTransmitTask(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
		do
		{
			if(Ring_Buffer_Read(&TransmitBuffer, &tmp) == RB_OK)
			{
			TransmitData[i] = tmp;
			i++;
			}
			else
			{
				break;
			}

		} while(tmp != '^');

		CDC_Transmit_FS(TransmitData, i);
}

void deinit_peripherals(void)
{

	__disable_irq();
	NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	USBD_Stop(&hUsbDeviceFS);
	USBD_DeInit(&hUsbDeviceFS);
	__HAL_RCC_USB_CLK_DISABLE();
	memset(&hUsbDeviceFS, 0, sizeof(USBD_HandleTypeDef));

	  HAL_GPIO_DeInit(COMM_PC_LED_GPIO_Port, COMM_PC_LED_Pin);
	  HAL_GPIO_DeInit(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
	  HAL_GPIO_DeInit(FRONT_SWITCH_GPIO_Port, FRONT_SWITCH_Pin);
	  //DMA Deinit
	  __HAL_RCC_DMA1_CLK_DISABLE();
	  HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
	  //TIM Deinit
	  __HAL_RCC_TIM1_CLK_DISABLE();
	  __HAL_RCC_TIM2_CLK_DISABLE();

	  HAL_UART_DeInit(&huart3);


	  HAL_DeInit();

	  IWDG->KR = 0xAAAA; //Unlock key Register
	  IWDG->KR = 0x0000; //Deactive IWDG


	  SysTick->CTRL = 0;
	  SysTick->LOAD = 0;
	  SysTick->VAL = 0;

}

void jump_to_application(uint32_t const app_address) {
	for (uint8_t i = 0; i < 8; i++) {
	        NVIC->ICER[i] = 0xFFFFFFFF;
	    }
  typedef void (*jumpFunction)(); // helper-typedef
  uint32_t const jumpAddress = *(__IO uint32_t*) (app_address + 4); // Address of application's Reset Handler
  jumpFunction runApplication =  jumpAddress; // Function we'll use to jump to application


  deinit_peripherals(); // Deinitialization of peripherals and systick

  __set_MSP(*((__IO uint32_t*) app_address)); // Stack pointer setup
  __asm__ volatile("ldr r10, =0x1234");
  runApplication(); // Jump to application
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

#ifdef  USE_FULL_ASSERT
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
