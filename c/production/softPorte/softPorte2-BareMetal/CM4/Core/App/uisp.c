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

#include "stm32mp15xx_disco.h"

#include "main.h"
#include "uisp.h"
#include "doorManager.h"
#include "proxyUISS.h"
#include "postmanSS.h"
#include "util.h"

/*
 * DEFINES
 */

/*
 * TYPEDEF
 */

/*
 * LOCAL FUNCTIONS
 */

static void uisp_init(void);
static void uisp_signalAppState(void);

/*
 * LOCAL VARIABLES
 */

/*******************************************************************************************/

void uisp_launchSP(void)
{
	// Init UI
	uisp_init();
	// Init VUART
	postmanSS_init();
}

void uisp_quitSP(void)
{
	// We're out!
	g_softPorte_state = S_DEAD;

	// Trun off LEDs before going out
	BSP_LED_Off(LED5);
	BSP_LED_Off(LED6);
}

void uisp_ackStartCom(void)
{
	HAL_TIM_Base_Stop_IT(&htim13); // Stop sending pings
	uisp_signalAppState();
	proxyUISS_updateDoorState(false); // Door is in closed position at start
}

void uisp_signalNotAllowed(void)
{
	// Turn on the Red LED
	BSP_LED_On(LED6);

#if 1
	// Block execution while signaling face is not allowed
	HAL_Delay(5000);
	BSP_LED_Off(LED6);
#else
	// Start TAL timer
	//HAL_TIM_Base_Start_IT(&htim6);
#endif

}

void uisp_TALCallback(void)
{
	// Turn off red LED after TAL
	BSP_LED_Off(LED6);
}

/**
 * @brief Init the uisp object (LEDs + Timer)
 */
static void uisp_init(void)
{
	// Initialize LEDs mounted on STM32MP157C-DK2 board
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
	// Initial state is Off
	BSP_LED_Off(LED5);
	BSP_LED_Off(LED6);
}


/**
 * @brief Turn on the LED5 (green LED) to signal SP launch
 */
static void uisp_signalAppState(void)
{
	BSP_LED_On(LED5);
}
