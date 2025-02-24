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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

/* USER CODE BEGIN PV */
uint16_t LightThresholdVal = 2000;//光阈值，大于该值认为是夜间
uint16_t VoiceThresholdVal = 800;//声音阈值，小于该值，认为有大声音
uint8_t MovingObjectFg = 0;    //检测到移动物体标志
uint8_t LoudVoiceFg = 0;//出现大声音标志
uint8_t DayOrNightFg = 0;//白天晚上标志志，0为白天，1为晚上
uint16_t PhotoSensorAdVal = 0;//光敏传感器AD值
uint16_t VoiceSensorAdVal = 0;//声音传感器AD值
uint8_t Timer1msFg = 0;    //定时器1ms标志
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*采集光敏传感器和声音传感器的AD值*/
void AnaSampleCalc(void)
{
    uint8_t i = 0;
    int16_t adc_buf[3] = {0};
    char str[40] = {0};
    for(i=0;i<2;i++)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,0xffff);

        if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
        {
            adc_buf[i] = HAL_ADC_GetValue(&hadc1);
        }
    }
    HAL_ADC_Stop(&hadc1);
    PhotoSensorAdVal = adc_buf[0];
    VoiceSensorAdVal = adc_buf[1];
//    sprintf(str,"%d %d\n",adc_buf[0],adc_buf[1]);
//    HAL_UART_Transmit(&huart1,str,strlen(str),0xffff);//打印AD值
}
/*根据光敏传感器的AD值与阈值比较，判断白天与夜间*/
void DayOrNightJudge(void)
{
    static uint16_t cnt1 = 0;
    static uint16_t cnt2 = 0;
    if(PhotoSensorAdVal>LightThresholdVal)    //夜间
    {
        if(++cnt1>=5)
        {
            cnt1=5;
            cnt2=0;
            DayOrNightFg = 1;
        }
    }
    else
    {
        if(++cnt2>=5)
        {
            cnt2=5;
            cnt1 = 0;
            DayOrNightFg = 0;
        }
    }
}
/*根据声音传感器的AD值与阈值比较，判断是否有大声音*/
void LoudVoiceDetection(void)
{

    if(VoiceSensorAdVal<VoiceThresholdVal)    //有大声音
    {

            LoudVoiceFg = 1;

    }
    else
    {

            LoudVoiceFg = 0;

    }
}
/*灯的控制流程*/
void LampControl(void)
{
    static uint8_t step = 1;
    static uint16_t time_cnt = 0;
    switch(step)
    {
        case 1:
            if(DayOrNightFg==1)//当前为夜间
            {
                if(LoudVoiceFg)//大声音标志
                {
                    step++;
                }
                else if(MovingObjectFg)//移动物体标志
                {
                    MovingObjectFg = 0;
                    step++;
                }
            }
            break;
        case 2:
            HAL_UART_Transmit(&huart1,"lamp On!\n",strlen("lamp On!\n"),0xffff);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);//打开灯
            time_cnt = 0;    //计时清零
            step++;
            break;
        case 3:
            if(++time_cnt>=5000)//计时5秒
            {
                HAL_UART_Transmit(&huart1,"lamp off!\n",strlen("lamp off!\n"),0xffff);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);//关闭灯
                MovingObjectFg = 0;
                step = 1;
            }
            break;
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
  MX_ADC1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);//先关闭灯
  HAL_TIM_Base_Start_IT(&htim4);//开启定时器4中断
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(Timer1msFg)
    {
      Timer1msFg = 0;
      AnaSampleCalc();
      DayOrNightJudge();
      LoudVoiceDetection();
      LampControl();
    }  
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*雷达信号外部中断回调函数*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    if(GPIO_Pin==GPIO_PIN_13)
    {
        MovingObjectFg = 1;//置检测到移动物体标志
    }
}
/*定时器4中断回调函数*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == htim4.Instance)
  {
      Timer1msFg = 1;
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
