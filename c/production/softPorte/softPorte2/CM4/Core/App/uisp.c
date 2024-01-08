/**
 * @file 	uisp.c
 * @brief 	Main logic for the board UI
 * @author 	BOUY Hugo
 * @date 	09/05/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

/*
 * INCLUDES
 */

#include "stm32mp1xx_hal.h"
#include "stm32mp15xx_disco.h"
#include "cmsis_os.h"

//#include "main.h"
#include "uisp.h"
#include "doorManager.h"
#include "postmanSS.h"
#include "proxyUISS.h"
//#include "util.h"

/*
 * DEFINES
 */

#define TAL		5

/*
 * TYPEDEF
 */

/*
 * LOCAL FUNCTIONS
 */

static void uisp_ledTimerCallback(void *argument);
static void uisp_signalAppState(void);

/*
 * LOCAL VARIABLES
 */

// Definitions for the led timer (Timer to turn off Red LED after TAL)
static osTimerId_t ledTimerHandler;
static const osTimerAttr_t ledTimer_attributes = {
  .name = "doorTimer"
};

/*******************************************************************************************/

void uisp_init(void)
{
	// Initialize LEDs mounted on STM32MP157C-DK2 board
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
	// Initial state is Off
	BSP_LED_Off(LED5);
	BSP_LED_Off(LED6);
}

void uisp_launchSP(void)
{
	// Led timer creation
	ledTimerHandler = osTimerNew(uisp_ledTimerCallback, osTimerOnce, NULL, &ledTimer_attributes);
	STOP_ON_ERROR(ledTimerHandler == NULL);

	// Start active objects
	doorManager_start();
	postmanSS_start();

	// Init com with SoftSonnette
	proxyUISS_askStartCom();
}

void uisp_quitSP(void)
{
	// Relay event to the Starter
	starter_stopEvent();
}


void uisp_free(void)
{
	osStatus_t returnStatus;

	// Free the led timer
	returnStatus = osTimerDelete(ledTimerHandler);
	STOP_ON_ERROR(returnStatus != osOK);
}

void uisp_ackStartCom(void)
{
	// TODO
	uisp_signalAppState();
}

void uisp_signalNotAllowed(void)
{
	osStatus_t returnStatus;

	// Turn on the Red LED
	BSP_LED_On(LED6);

	// Start TAL timer
	// pdMS_TO_TICKS allow to convert ms to system ticks
	returnStatus = osTimerStart(ledTimerHandler, (uint32_t) pdMS_TO_TICKS(TAL*1000));
	STOP_ON_ERROR(returnStatus != osOK);
}

/**
 * @brief Callback function for the LED Timer
 */
static void uisp_ledTimerCallback(void *argument)
{
	// Turn off red LED after TAL
	BSP_LED_Off(LED6);
}

/**
 * @brief Turn on the LED5 (green LED) to signal SP launch
 */
static void uisp_signalAppState(void)
{
	BSP_LED_On(LED5);
}
