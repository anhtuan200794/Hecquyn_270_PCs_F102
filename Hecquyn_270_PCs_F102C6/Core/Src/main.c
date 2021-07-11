/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdbool.h>
#include <stdio.h>

struct __FILE
{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};
/*send text over SWV*/
int fputc(int ch, FILE *f) {
  ITM_SendChar(ch);//send method for SWV
  return (ch);
}

#define MP3_TRAKING_NUM       0x03 // 0..2999
#define MP3_VOLUME          0x06 // 0..30
#define MP3_RESET         0x0C
#define MP3_PLAYBACK        0x0D
#define MP3_PAUSE         0x0E
#define MP3_STOP        0x16

#define COUNT_TO_ON_OFF  10
#define TIME_30_SECOND  30000
#define TIME_1_MINUTE   60000
#define TIME_2_MINUTES  120000
#define TIME_5_MINUTES  300000
#define TIME_15_MINUTES 900000
#define TIME_20_MINUTES 1200000
#define TIME_25_MINUTES 1500000
#define TIME_30_MINUTES 1800000
#define TIME_ADJUST_SPEED_ALARM 5000
#define TIME_GOTO_SLEEP_MODE 600000
//#define TIME_TO_CONGRATULATE 300000 //5min
#define PROGRAM_TIMEOUT 30000
#define TIME_TO_CONGRATULATE_STOP 30000 //30s

UART_HandleTypeDef huart3;

int key = 100; // default
uint32_t keyPadData;
int mode = 1;
int nSetup = 1;
bool isStart = false;
bool isSetup = false;
bool isMode = false;
bool isStartWithMode = false;
bool isPressStop = false;
bool isOn = true;
bool isSpeedChange = false;
bool isSleep = false;

int nStopPress = 0;
int nVol = 15;
GPIO_PinState safeKey = GPIO_PIN_RESET;
GPIO_PinState lastSafeKeyStatus = GPIO_PIN_RESET;
uint32_t sleepModeTick = 0;
uint32_t remindTick = 0;
uint32_t currentTick = 0;
uint32_t tickForPlusMinus = 0;
uint32_t tickForCline = 0;
uint32_t tickForCongratulate = 0;
uint32_t tickForProgramTimeout = 0;
uint32_t startick = 0;
uint8_t mp3_cmd_buf[10] = {0x7E, 0xFF, 0x06, 0x00, 0x01, 0x0, 0x0, 0x00, 0x00, 0xEF};

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void playVoice(char key);
void MP3_send_cmd (uint8_t cmd, uint16_t high_arg, uint16_t low_arg);
void MP3_stop(void);
void MP3_play(uint8_t index);
void MP3_setVol(uint8_t vol);
void MP3_reset(void);
void SetDefaulData(void);

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

  printf("Application start\n");
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  MP3_reset();
  HAL_Delay(5000);
  MP3_setVol(nVol);
  HAL_Delay(200);
  MP3_play(1);
  sleepModeTick = HAL_GetTick();

  while (1)
  {
    /* USER CODE END WHILE */
    keyPadData = GPIOA->IDR;
    safeKey = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0); // check safe key
    if (lastSafeKeyStatus == GPIO_PIN_SET && safeKey == GPIO_PIN_RESET)
    {
      // Let continue
      MP3_play(30);
      HAL_Delay(50);

    }
    if (lastSafeKeyStatus == GPIO_PIN_RESET && safeKey == GPIO_PIN_SET)
    {
      MP3_play(29); // hay cai khoa an toan truoc khi luyen tap
      HAL_Delay(50);
      SetDefaulData();
    }
    lastSafeKeyStatus = safeKey;
    if (isOn && !isSleep && (safeKey == GPIO_PIN_RESET)) {

      currentTick = HAL_GetTick();
      if (!isStartWithMode) { // if start with mode, don't remind.
        //==============================================
        if (isStart && (currentTick - startick < TIME_5_MINUTES - 60000) && (currentTick - remindTick >= TIME_1_MINUTE)) // 0-4m
        {
          remindTick = currentTick;
          printf("1m remind speed up/down\n");
          MP3_play(36);
          HAL_Delay(50);
        }
        else if (isStart && (currentTick - startick > TIME_5_MINUTES + 60000) && (currentTick - startick < TIME_15_MINUTES - 60000) && (currentTick - remindTick >= TIME_1_MINUTE)) // 6-14m
        {
          remindTick = currentTick;
          printf("1m remind speed up/down\n");
          MP3_play(36);
          HAL_Delay(50);
        }
        else if (isStart && (currentTick - startick > TIME_15_MINUTES + 60000) && (currentTick - startick < TIME_20_MINUTES - 60000) && (currentTick - remindTick >= TIME_1_MINUTE)) // 16-19m
        {
          remindTick = currentTick;
          printf("1m remind speed up/down\n");
          MP3_play(36);
          HAL_Delay(50);
        }
        //==================================================================== Speed down
        else if (isStart && (currentTick - startick > TIME_20_MINUTES + 60000) && (currentTick - startick < TIME_25_MINUTES - 60000) && (currentTick - remindTick >= TIME_1_MINUTE)) //21-24m
        {
          remindTick = currentTick;
          printf("1m remind speed down\n");
          MP3_play(19);
          HAL_Delay(50);
        }
        else if (isStart && (currentTick - startick > TIME_25_MINUTES + 60000) && (currentTick - startick < TIME_30_MINUTES - 60000) && (currentTick - remindTick >= TIME_1_MINUTE)) //26-29m
        {
          remindTick = currentTick;
          printf("1m remind speed down\n");
          MP3_play(19);
          HAL_Delay(50);
        }
        else if (isStart && (currentTick - startick > TIME_30_MINUTES + 60000) && (currentTick - remindTick >= TIME_1_MINUTE)) //26-29m
        {
          remindTick = currentTick;
          printf("1m remind speed down\n");
          MP3_play(19);
          HAL_Delay(50);
        }
        //===============================================
        else if ((currentTick - startick >= TIME_5_MINUTES) && isStart && (currentTick - startick < TIME_5_MINUTES + 100 )) //5 minutes remind adjust speed
        {
          printf("Buoc 4\n");
          MP3_play(32); // Buoc 4 voi khoang 10p tiep theo
          HAL_Delay(50);
        }
        else if ((currentTick - startick >= TIME_15_MINUTES) && isStart && (currentTick - startick < TIME_15_MINUTES + 100)) //15 minutes
        {
          printf("Buoc 5\n");
          MP3_play(33); // buoc 5 voi khoang 5p tiep theo
          HAL_Delay(50);
        }
        else if ((currentTick - startick >= TIME_20_MINUTES) && isStart && (currentTick - startick < TIME_20_MINUTES + 100)) // sau 20p
        {
          printf("Buoc 6\n");
          MP3_play(35); // buoc 6 voi khoang 5p den 10p
          HAL_Delay(50);
        }
        else if ((currentTick - startick >= TIME_25_MINUTES) && isStart && (currentTick - startick < TIME_25_MINUTES + 150)) // sau 25p
        {
          printf("Buoc 7\n");
          MP3_play(34); // buoc 7
          HAL_Delay(50);
        }
        else if ((currentTick - startick >= TIME_30_MINUTES) && isStart && (currentTick - startick < TIME_30_MINUTES + 150)) // sau 30p
        {
          printf("Buoc 7\n");
          MP3_play(34); // buoc 7
          HAL_Delay(50);
        }
      }

      if (currentTick - tickForProgramTimeout >= PROGRAM_TIMEOUT && isMode)
      {
        SetDefaulData();
        printf("program timeout\n");
      }

      if (((HAL_GetTick() - sleepModeTick) >= TIME_GOTO_SLEEP_MODE) && !isStart) // sleep mode check
      {
        isSleep = true;
        printf("Go to sleep mode\n");
        SetDefaulData();// reset all data
        sleepModeTick = HAL_GetTick();
      }
      if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_0)))) // Key A incl :3
      {
        key = 10;
        if (isStart)
          MP3_play(13);
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_0)))) // Key B incl: 6
      {
        key = 11;
        if (isStart)
          MP3_play(12);
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_0)))) // Key C incl: 9
      {
        key = 12;
        if (isStart)
          MP3_play(11);
        HAL_Delay(200);
      }
      /*  Check Key keyPadData from the second pin */
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_1)))) // Incline +
      {
        //remindTick = HAL_GetTick();
        key = 0;
        if (isStart)
        {
          if ((HAL_GetTick() - tickForCline) >= TIME_ADJUST_SPEED_ALARM) /// after 30s if user adjust incline it will remind keep safe
          {
            MP3_play(24);
          }
          tickForCline = HAL_GetTick();
        }
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_5 | GPIO_PIN_1)))) //          key = Program
      {
        key = 6;
        if (isStart == false && isSetup == false)
        {
          isMode = true;
          tickForProgramTimeout = HAL_GetTick();
          switch (mode) {
            case 1:
              MP3_play(3);
              break;
            case 2:
              MP3_play(4);
              break;
            case 12:
              MP3_play(5);
              break;
            case 13:
              MP3_play(6);
              break;
            case 14:
              mode = 0; // mode will be set to 1 in the end of switch
              isMode = false;
              break;
            default:
              break;
          }
          mode++;
        }
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_4 | GPIO_PIN_1)))) //key Start;
      {
        startick = tickForCline = tickForPlusMinus = tickForCongratulate = remindTick = HAL_GetTick();
        key = 7;
        //          HAL_Delay(150);
        if (isStart == false)
        { // start
          if (mode > 1) {
            isStartWithMode = true;
            printf("Start with mode, don't remind\n");
            MP3_play(37);
          } else {
            MP3_play(7);
          }
          isStart = true;
          mode = 1 ; // reset mode
          nSetup = 1; // reset nSetup
          isSetup = false;
          isMode = false;
        }
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_5 | GPIO_PIN_2)))) //key Stop;
      {
        key = 8;
        isPressStop = true;
        HAL_Delay(200);
        if (isPressStop)
        {
          nStopPress++;
        }

        if (isStart == true) {  // stop
          if ((HAL_GetTick() - startick) > TIME_TO_CONGRATULATE_STOP)
          {
            MP3_play(2);
          } else
          {
            MP3_stop();
          }
        } else if (nStopPress == COUNT_TO_ON_OFF) { // /// turn on/off audio manual
          if (isOn)
          {
            isOn = false;
            MP3_play(18);
          }
          else
          {
            isOn = true;
            MP3_play(17);
          }
          nStopPress = 0;
					isPressStop = false;
        }
        //SetDefaulData();
        isStart = false;
        mode = 1 ; // reset mode
        nSetup = 1; // reset nSetup
        isSetup = false;
        isMode = false;
        //HAL_Delay(100);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_2)))) // Key Mode
      {
        key = 9;
        if (isStart == false && isMode == false) // setup
        {
          isSetup = true;
          switch (nSetup) {
            case 1:
              MP3_play(8);
              break;
            case 2:
              MP3_play(9);
              break;
            case 3:
              MP3_play(10);
              break;
            case 4:
              MP3_stop();
              nSetup = 0;
              isSetup = false;
              break;
            default:
              break;
          }
          nSetup++;
        }
        HAL_Delay(200);
      }
      /*  Check Key keyPadData from the third pin */
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_1)))) // Incline -
      {
        //remindTick = HAL_GetTick();
        key = 1;
        HAL_Delay(150);
        if (isStart)
        {
          if ((HAL_GetTick() - tickForCline) >= TIME_ADJUST_SPEED_ALARM) /// after 30s if user adjust incline it will remind keep safe
          {
            MP3_play(25);
          }
          tickForCline = HAL_GetTick();
        }
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_2)))) // Key +
      {
        //remindTick  = HAL_GetTick();
        key = 4;
        nVol += 2;
        if (nVol >= 20)
          nVol = 20;
        if (!(isMode || isSetup || isStart))
        {
          MP3_setVol(nVol);
          HAL_Delay(100);
          //MP3_play(20);
          printf("Vol: %d\n", nVol);
        }
        if (isStart)
        {
          if ((HAL_GetTick() - tickForPlusMinus) >= TIME_ADJUST_SPEED_ALARM) /// after 30s if user adjust speed it will remind keep safe
          {
            MP3_play(21);
          }
          tickForPlusMinus = HAL_GetTick();
        }
        HAL_Delay(50);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_2))) ) // key -
      {
        //remindTick  = HAL_GetTick();
        key = 5;
        nVol -= 2;
        if (nVol <= 0)
          nVol = 0;
        if (!(isMode || isSetup || isStart))
        {
          MP3_setVol(nVol);
          HAL_Delay(100);
          //MP3_play(23);
          printf("Vol: %d\n", nVol);
        }
        if (isStart)
        {
          if ((HAL_GetTick() - tickForPlusMinus) >= TIME_ADJUST_SPEED_ALARM) // after 30s if user adjust speed it will remind keep safe
          {
            MP3_play(22);
          }
          tickForPlusMinus = HAL_GetTick();
        }
        HAL_Delay(50);
      }
      /* Check Key keyPadData from the fourt pin */
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_3)))) // Key D speed 3
      {
        key = 13;
        if (isStart)
          MP3_play(16);
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_3)))) // Key E speed 6
      {
        key = 14;
        if (isStart)
          MP3_play(15);
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_3)))) // Key F speed 9
      {
        key = 15;
        if (isStart)
          MP3_play(14);
        HAL_Delay(200);
      }
      //-----------------------------------------------------------------------------------------------------
    } else
    { /// turn off audio manual
      if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_0)))) // Key 10 incl :4
      {
        key = 10;
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_0)))) // Key 11 incl: 4
      {

        key = 11;
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_0)))) // Key 12 incl: 12
      {
        key = 12;
        HAL_Delay(200);
      }
      /*  Check Key keyPadData from the second pin */
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_1))))
      {
        key = 0;
        HAL_Delay(150);
      }

      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_5 | GPIO_PIN_1)))) //          key 6 Program
      {
        key = 6;
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_4 | GPIO_PIN_1)))) //key 7 Start;
      {
        key = 7;
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_5 | GPIO_PIN_2)))) //key 8 Stop;
      {
        key = 8;
        isPressStop = true;
        MP3_stop();
        HAL_Delay(200);
        if (isPressStop)
        {
          nStopPress++;
        }
        if (isStart == true) {  // stop
          MP3_play(2);
        } else if (nStopPress == COUNT_TO_ON_OFF) { // /// turn on/off audio manual
          if (isOn)
          {
            isOn = false;
            MP3_play(18);
          }
          else
          {
            isOn = true;
            MP3_play(17);
          }
          nStopPress = 0;
        }
        isStart = false;
        mode = 1 ; // reset mode
        nSetup = 1; // reset nSetup
        isSetup = false;
        isMode = false;
        //HAL_Delay(100);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_2)))) // Key 9  Mode
      {
        key = 9;
        HAL_Delay(200);
      }
      /*  Check Key keyPadData from the third pin */
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_1)))) //key 1
      {
        key = 1;
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_5 | GPIO_PIN_2))))
      {
        key = 2;
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_2))))
      {
        key = 3;
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_2)))) // +
      {
        key = 4;
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_4 | GPIO_PIN_3)))) // + 2
      {
        key = 16;
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_2)))) // -
      {
        key = 5;
        HAL_Delay(150);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_5 | GPIO_PIN_3)))) // - 2
      {
        key = 17;
        HAL_Delay(150);
      }

      /* Check Key keyPadData from the fourt pin */
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_6 | GPIO_PIN_3)))) // Key 13 speed 9
      {
        key = 13;
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_7 | GPIO_PIN_3)))) // Key 14 speed 6
      {
        key = 14;
        HAL_Delay(200);
      }
      else if (!(keyPadData & ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_3)))) // Key 15 speed 3
      {
        key = 15;
        HAL_Delay(200);
      }
    }
    // Check key change
    if (key != 100)
    {
      if (key != 8)
      {
        isPressStop = false;
      }
      if (safeKey == GPIO_PIN_SET)
      {
        MP3_play(26);
        SetDefaulData();
      }
      sleepModeTick = HAL_GetTick();
      isSleep = false;
      printf("Key: %d -- IsStart: %d -- IsMode: %d -- isOn: %d -- isSetup: %d -- isSleep: %d -- SafeKey: %d\n", key, isStart, isMode, isOn, isSetup, isSleep, safeKey);
      key = 100;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void SetDefaulData(void)
{
  printf("Set Defaul Data\n");
  key = 100; // default
  mode = 1;
  nSetup = 1;
  isStart = false;
  isSetup = false;
  isMode = false;
  //isPressStop = false;
  //isOn = true;
  isSpeedChange = false;
  //nStopPress = 0;
  //sleepModeTick = 0;
  remindTick = 0;
  currentTick = 0;
  tickForPlusMinus = 0;
  isStartWithMode = false;
}
/* Calculate checksum
*/
uint16_t MP3_checksum (void) {
  uint16_t sum = 0;
  uint8_t i;
  for (i = 1; i < 7; i++) {
    sum += mp3_cmd_buf[i];
  }
  return -sum;
}

/* Send command to DFPlayer
*/
void MP3_send_cmd (uint8_t cmd, uint16_t high_arg, uint16_t low_arg) {

  uint16_t checksum;

  mp3_cmd_buf[3] = cmd;

  mp3_cmd_buf[5] = high_arg;
  mp3_cmd_buf[6] = low_arg;

  checksum = MP3_checksum();
  mp3_cmd_buf[7] = (uint8_t) ((checksum >> 8) & 0x00FF);
  mp3_cmd_buf[8] = (uint8_t) (checksum & 0x00FF);

  // Send command to UART3
  /*
    for (i=0; i<10; i++) {
    char* data = &mp3_cmd_buf[0];
    int size = sizeof(mp3_cmd_buf);
    HAL_UART_Transmit(&huart1,data,size,2000);
    //        USART_SendData(USART1, mp3_cmd_buf[i]);
    //        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
  */
  int size = sizeof(mp3_cmd_buf);

  //    while((status != HAL_OK) || i <= 10)
  //      {
  HAL_UART_Transmit(&huart1, mp3_cmd_buf, size, 300);
  //        i++;
  //    }
  //memset(&mp3_cmd_buf[0], 0, sizeof mp3_cmd_buf);
}
void MP3_stop(void)
{
  printf("MP3 Stop !\n");
  MP3_send_cmd(MP3_STOP, 0, 0);
  //MP3_send_cmd(MP3_RESET,0,0);
}

void MP3_play(uint8_t index)
{
  printf("Play: %d\n", index);
  MP3_send_cmd(MP3_STOP, 0, 0);
  HAL_Delay(500);
  MP3_send_cmd(MP3_TRAKING_NUM, 0, index);
}
void MP3_setVol(uint8_t vol)
{
  MP3_send_cmd(MP3_VOLUME, 0, vol);
}

void MP3_reset()
{
  MP3_send_cmd(MP3_RESET, 0, 0);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
