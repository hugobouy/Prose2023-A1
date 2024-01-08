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

//#include <stdbool.h>
#include "cmsis_os.h"

#include "doorManager.h"
#include "proxyUISS.h"
#include "proxyGUI.h"
#include "util.h"

/*
 * DEFINES
 */

// Time for which the door is open
#define TOP 	10

#define QUEUE_SIZE	4U

/*
 * TYPEDEF
 */

typedef enum {S_FORGET=0, S_IDLE, S_DOOR_OPEN, S_DEATH, STATE_NB} doorManager_State_t;
typedef enum {E_ASK_OPEN_DOOR, E_ASK_DOOR_STATE, E_TIMEOUT_TOP, E_STOP, EVENT_NB} doorManager_Event_t;
typedef enum {A_NOP=0, A_OPEN_DOOR, A_SEND_DOOR_STATE, A_CLOSE_DOOR} doorManager_Action_t;

typedef struct
{
	doorManager_State_t destinationState;
	doorManager_Action_t action;
}doorManager_Transition_t;

typedef union
{
	doorManager_Event_t event;
	uint8_t buffer[sizeof(doorManager_Event_t)];
}doorManager_Message_t;

/*
 * LOCAL FUNCTIONS
 */

static void doorManager_run(void *argument);
static void doorManager_queueReceive(doorManager_Message_t *msg);
static void doorManager_queueSend(doorManager_Event_t event);
static void doorManager_doorTimerCallback(void *argument);
static void doorManager_performAction(doorManager_Action_t action);
static void doorManager_unlockDoor(void);

/*
 * LOCAL VARIABLES
 */

static bool doorState;

// All other event combination are ignored
static doorManager_Transition_t transTable [STATE_NB-1][EVENT_NB] =
{
		[S_IDLE][E_ASK_OPEN_DOOR] = {S_DOOR_OPEN, A_OPEN_DOOR},
		[S_IDLE][E_ASK_DOOR_STATE] = {S_IDLE, A_SEND_DOOR_STATE},
		[S_IDLE][E_STOP] = {S_DEATH, A_NOP},
		[S_DOOR_OPEN][E_TIMEOUT_TOP] = {S_IDLE, A_CLOSE_DOOR},
		[S_DOOR_OPEN][E_ASK_DOOR_STATE] = {S_DOOR_OPEN, A_SEND_DOOR_STATE},
		[S_DOOR_OPEN][E_STOP] = {S_DEATH, A_NOP}
};

// Definitions for the doorManager task
static osThreadId_t doorManagerTaskHandle;
static const osThreadAttr_t doorManagerTask_attributes = {
  .name = "doorManager",
  .stack_size = 128,
  .priority = (osPriority_t) osPriorityNormal,
};

// Definitions for the doorManager queue
static osMessageQueueId_t doorManagerQueueHandle;
static const osMessageQueueAttr_t doorManagerQueue_attributes = {
  .name = "doorManagerQueue"
};

// Definitions for the door timer
static osTimerId_t doorTimerHandle;
static const osTimerAttr_t doorTimer_attributes = {
  .name = "doorTimer"
};

// Definition for the quit semaphore
static osSemaphoreId_t doorManagerQuitSemHandle;
static const osSemaphoreAttr_t doorManagerQuitSem_attributes = {
  .name = "doorManagerQuitSem"
};

/*******************************************************************************************/

void doorManager_new(void)
{
	osStatus_t returnStatus;

	// Semaphore creation
	doorManagerQuitSemHandle = osSemaphoreNew(1U, 0U, &doorManagerQuitSem_attributes);
	STOP_ON_ERROR(doorManagerQuitSemHandle == NULL);

	// Queue creation
	doorManagerQueueHandle = osMessageQueueNew (QUEUE_SIZE, sizeof(doorManager_Message_t), &doorManagerQueue_attributes);
	STOP_ON_ERROR(doorManagerQueueHandle == NULL);

	// Timer creation
	doorTimerHandle = osTimerNew(doorManager_doorTimerCallback, osTimerOnce, NULL, &doorTimer_attributes);
	STOP_ON_ERROR(doorTimerHandle == NULL);

	// Thread creation
	doorManagerTaskHandle = osThreadNew(doorManager_run, NULL, &doorManagerTask_attributes);
	STOP_ON_ERROR(doorManagerTaskHandle == NULL);

	returnStatus = osThreadSuspend(doorManagerTaskHandle); // Don't start it yet
	STOP_ON_ERROR(returnStatus != osOK);
}

void doorManager_free(void)
{
	osStatus_t returnStatus;

	// Free the message Queue
	returnStatus = osMessageQueueDelete(doorManagerQueueHandle);
	STOP_ON_ERROR(returnStatus != osOK);

	// Free the timer
	returnStatus = osTimerDelete(doorTimerHandle);
	STOP_ON_ERROR(returnStatus != osOK);

	// Free the semaphore
	returnStatus = osSemaphoreDelete(doorManagerQuitSemHandle);
	STOP_ON_ERROR(returnStatus != osOK);
}

void doorManager_start(void)
{
	// Resume thread
	osStatus_t returnStatus;
	returnStatus = osThreadResume(doorManagerTaskHandle);
	STOP_ON_ERROR(returnStatus != osOK);
}

void doorManager_stop(void)
{
	osStatus_t returnStatus;
	doorManager_queueSend(E_STOP);
	// Wait for the thread execution to finish
	returnStatus = osSemaphoreAcquire(doorManagerQuitSemHandle, 0);
	STOP_ON_ERROR(returnStatus != osOK);
}

void doorManager_askOpenDoor(void)
{
	doorManager_queueSend(E_ASK_OPEN_DOOR);
}

void doorManager_askDoorState(void)
{
	doorManager_queueSend(E_ASK_DOOR_STATE);
}

/**
 * @brief Thread function for doorManager
 */
static void doorManager_run(void *argument)
{
	osStatus_t returnStatus;
	doorManager_Message_t msg;
	doorManager_State_t currentState = S_IDLE;
	doorManager_Transition_t transition;

	// Thread loop
	while(currentState != S_DEATH)
	{
		doorManager_queueReceive(&msg);
		transition = transTable[currentState][msg.event];
		if(transition.destinationState != S_FORGET)
		{
			doorManager_performAction(transition.action);
			currentState = transition.destinationState;
		}
		// Transitions that are not registered are ignored
	}
	// Signal to the main thread we're out
	returnStatus = osSemaphoreRelease(doorManagerQuitSemHandle);
	STOP_ON_ERROR(returnStatus != osOK);

	osThreadExit();
}

/**
 * @brief Receive message from the doorManager queue. Blocking function with no timeout.
 *
 * @param *doorManager_Message_t: pointer to the message structure that will receive the message
 */
static void doorManager_queueReceive(doorManager_Message_t *msg)
{
	osStatus_t returnStatus;
	returnStatus = osMessageQueueGet(doorManagerQueueHandle, msg->buffer, NULL, 0);
	STOP_ON_ERROR(returnStatus != osOK);
}

/**
 * @brief Send passed event to the doorManager queue. Blocking function with no timeout.
 *
 * @param doorManager_Event_t: event to send
 */
static void doorManager_queueSend(doorManager_Event_t event)
{
	osStatus_t returnStatus;
	doorManager_Message_t msg;

	msg.event = event;
	returnStatus = osMessageQueuePut(doorManagerQueueHandle, &(msg.buffer), 0, 0);
	STOP_ON_ERROR(returnStatus != osOK);
}

/**
 * @brief Perform action passed with the function
 *
 * @param doorManager_Action_t: action to be done
 */
static void doorManager_performAction(doorManager_Action_t action)
{
	switch(action)
	{
		case A_OPEN_DOOR:
			// Update door state on GUI and UISS
			proxyGUI_updateDoorState(true);
			proxyUISS_updateDoorState(true);

			// Unlock door
			doorManager_unlockDoor();

			break;

		case A_CLOSE_DOOR:
			proxyGUI_updateDoorState(false);
			proxyUISS_updateDoorState(false);

			doorState = false; // Simulate door look
			break;

		case A_SEND_DOOR_STATE:
			proxyGUI_updateDoorState(doorState);
			proxyUISS_updateDoorState(doorState);
			break;

		// Any other action receive are considered as errors
		default:
			STOP_ON_ERROR(true);
	}
}

/**
 *  @brief Callback for the doorTimer (door is locked)
 */
static void doorManager_doorTimerCallback(void *argument)
{
	doorManager_queueSend(E_TIMEOUT_TOP);
}

/**
 * @brief Simulate the unlocking of the Door for TOP (10s)
 */
static void doorManager_unlockDoor(void)
{
	// Start TOP timer
	osStatus_t returnStatus;
	// pdMS_TO_TICKS allow to convert ms to system ticks
	returnStatus = osTimerStart(doorTimerHandle, (uint32_t) pdMS_TO_TICKS(TOP*1000));
	STOP_ON_ERROR(returnStatus != osOK);
	doorState = true;
}
