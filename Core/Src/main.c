/**
  ******************************************************************************
  * @file    BSP/Src/main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32H750B_DISCOVERY BSP Drivers
  *          This is the main program.   
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "temp_conversion.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"
#include "touchscreen.h"
#include "alarm.h"
/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFSIZE 256
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char sendBuffer[BUFFSIZE];
int counter;

int analogValue;
double VOut;
int VOut_int;
int VOut_frac;

double res;
int res_int;
int res_frac;

TempPair temps;
float temp;
int temp_int;
int temp_frac;

int16_t alarm_temp = 35;
Button_Pressed_Typedef current_state = BUTTON_PRESSED_NONE;
Button_Pressed_Typedef new_state;

volatile uint32_t flag_1s = 1;
volatile uint32_t flag_20ms = 1;
int hold_counter = 0;;
int hold_speed = 10;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
  /* System Init, System clock, voltage scaling and L1-Cache configuration are done by CPU1 (Cortex-M7)
     in the meantime Domain D2 is put in STOP mode(Cortex-M4 in deep-sleep)
  */

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 400 MHz */
  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();

  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);

  // Initializes the static features of the LCD GUI
  Touchscreen_template_init();
  // Draws the default alarm temperature (35)
  Draw_threshold(alarm_temp);

  /* Wait For User inputs */
  while (1)
  {
	  // Is true every 20ms
	  if (flag_20ms) {
		  new_state = Button_check();
		  if (current_state == new_state) {
			  hold_counter++;
		  } else {
			  hold_counter = 0;
			  hold_speed = 10;
		  }

		  if (new_state == BUTTON_PRESSED_UP && alarm_temp < 130 && (!hold_counter || hold_speed == hold_counter)) {
			  hold_counter = 0;
			  if (hold_speed > 4) {
				  hold_speed--;
			  }
			  Draw_threshold(++alarm_temp);
		  } else if (new_state == BUTTON_PRESSED_DOWN && alarm_temp > -30 && (!hold_counter || hold_speed == hold_counter)) {
			  hold_counter = 0;
			  if (hold_speed > 4) {
				  hold_speed--;
			  }
			  Draw_threshold(--alarm_temp);
		  }

		  if (new_state != current_state) {
			  Draw_buttons(new_state, current_state);
			  current_state = new_state;
		  }
		  flag_20ms = 0;
	  }

	  // Is true every second
	  if (flag_1s) {
		  // Gets value from ADC
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		  analogValue = HAL_ADC_GetValue(&hadc1);

		  // Calculates ADC input voltage
		  VOut = (double)analogValue / 65535.0f * 3.3;

		  VOut_int = (int)VOut;
		  VOut_frac = (int)((VOut - VOut_int) * 10000);

		  // Calculates thermistor resistance
		  res = 100000 * ((65535.0 / (double)analogValue) - 1);

		  res_int = (int)res;
		  res_frac = (int)((res - res_int) * 100);

		  // Converts resistance to Celsius using LUT
		  temps = Binary_search(res);

		  // Interpolates temperature for more accurate result
		  temp = Linear_interpolation(temps, res);

		  temp_int = (int)temp;
		  temp_frac = (int)((temp - temp_int) * 100);

		  // Draws current temperature on LCD
		  Draw_temperature(temp_int, temp_frac);
		  // Handles alarm activation, deactivation and the changing of the note being played
		  Alarm((int)alarm_temp, temp);

		  snprintf(sendBuffer, BUFFSIZE, "[%d] analog: %d | VOut: %d.%04d | res: %d.%02d | temp: %d.%02d\n\r",
				  counter++, analogValue, VOut_int, VOut_frac, res_int, res_frac, temp_int, temp_frac);
		  HAL_UART_Transmit(&huart3, (uint8_t *)sendBuffer, strlen(sendBuffer), 100);

//		  HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);

		  flag_1s = 0;
	  }

  }
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */

// Handles the 20ms and 1s flags
void HAL_IncTick(void)
{
    static uint32_t counter = 0;

    counter++;
    if (counter >= 1000) {
        flag_1s = 1;
        counter = 0;
    }
    if (!(counter % 20)) {
    	flag_20ms = 1;
    }

	uwTick += (uint32_t)uwTickFreq;
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

 /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
 */

  /*activate CSI clock mondatory for I/O Compensation Cell*/
  __HAL_RCC_CSI_ENABLE() ;

  /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;

  /* Enables the I/O Compensation Cell */
  HAL_EnableCompensationCell();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Turn LED REDon */
  BSP_LED_On(LED_RED);
  while(1)
  {
  }
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @}
  */

/**
  * @}
  */

