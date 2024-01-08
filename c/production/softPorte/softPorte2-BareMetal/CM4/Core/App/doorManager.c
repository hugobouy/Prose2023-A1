/**
 * @file 	doorManager.c
 * @brief 	Handle the logic for to simulate the door
 * @author 	BOUY Hugo
 * @date 	04/06/2023
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

#include <stdbool.h>

#include "main.h"
#include "doorManager.h"
#include "proxyUISS.h"
#include "proxyGUI.h"
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

static inline void doorManager_sendDoorState(void);
static void doorManager_lockDoor(void);
static void doorManager_unlockDoor(void);

/*
 * LOCAL VARIABLES
 */

static bool doorState;

/*******************************************************************************************/

void doorManager_askOpenDoor(void)
{
	// Unlock door
	doorManager_unlockDoor();

#if 1
	// Block execution during door opening
	HAL_Delay(10000);
	// Lock door
	doorManager_lockDoor();
#else
	// Start TOP timer
	HAL_TIM_Base_Start_IT(&htim5);
#endif
}

void doorManager_askDoorState(void)
{
	// Send back door state to AOP and SS
	doorManager_sendDoorState();
}

void doorManager_TOPCallback(void)
{
	// Lock door
	doorManager_lockDoor();
}

/**
 * @brief Send door state to the proxys
 * 		  Inline function to save stack
 */
static inline void doorManager_sendDoorState(void)
{
	proxyGUI_updateDoorState(doorState);
	HAL_Delay(200); // Small delay to prevent overloading the VUART
	proxyUISS_updateDoorState(doorState);
}

/**
 * @brief Simulate the door lock after TOP
 */
static void doorManager_lockDoor(void)
{
	// Door is closed
	doorState = false;

	log_info("Door closed\r\n");

	// Inform AOP and SS
	doorManager_sendDoorState();
}

/**
 * @brief Simulate the unlocking of the door for TOP (10s)
 */
static void doorManager_unlockDoor(void)
{
	// Door is open
	doorState = true;

	log_info("Door opened\r\n");

	// Inform AOP and SS
	doorManager_sendDoorState();
}
