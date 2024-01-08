/**
 * @file 	postmanSS.c
 * @brief 	Send messages from the proxy on the VUART
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

//#include "main.h"
#include "cmsis_os.h"
#include "openamp.h"

#include "postmanSS.h"
#include "dispatcherSS.h"
//#include "util.h"

/*
 * DEFINES
 */

#define MAX_BUFFER_SIZE 	128
#define QUEUE_SIZE 			4U

#define RUNNING		1
#define DEAD		0

/*
 * TYPEDEF
 */

/*
 * LOCAL FUNCTIONS
 */

static void postmanSS_run(void *argument);
static void postmanSS_read(void *argument);
static void postmanSS_queueReceive(encodedMessage_t *msg);
static void postmanSS_queueSend(encodedMessage_t *msg);
static void postmanSS_virt_UART0_cb(VIRT_UART_HandleTypeDef *vuartHandle);

/*
 * LOCAL VARIABLES
 */

// State of the threads (used to exit threads)
static bool postmasSSState;

// Definition for the idle task
static osThreadId_t postmanSSRunTaskHandle;
static const osThreadAttr_t postmanSSRun_attributes =
{
	.name = "postmanSSRun",
	.stack_size = 256,
	.priority = (osPriority_t) osPriorityNormal
};

// Definition for the t0 task
static osThreadId_t postmanSSReadTaskHandle;
static const osThreadAttr_t postmanSSRead_attributes =
{
	.name = "postmanSSRead",
	.stack_size = 256,
	.priority = (osPriority_t) osPriorityHigh
};

// Definitions for the postmanSS_send queue
static osMessageQueueId_t postmanSS_sendQueueHandle;
static const osMessageQueueAttr_t postmanSS_sendQueue_attributes = {
  .name = "postmanSS_sendQueue"
};

// Definition for the quit semaphore
static osSemaphoreId_t postmanSSQuitSemHandle;
static const osSemaphoreAttr_t postmanSSQuitSem_attributes = {
  .name = "postmanSSQuitSem"
};

// Definitions for Virtual Uart
VIRT_UART_HandleTypeDef virtUART0;

__IO FlagStatus VirtUart0RxMsg = RESET;
uint8_t VirtUart0ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart0ChannelRxSize = 0;

/*******************************************************************************************/

void postmanSS_initVUART(void)
{
	VIRT_UART_StatusTypeDef returnStatus_VIRT_UART;

	// Init Virtual Uart
	returnStatus_VIRT_UART = VIRT_UART_Init(&virtUART0);
	STOP_ON_ERROR(returnStatus_VIRT_UART != VIRT_UART_OK);

	// Register callback
	returnStatus_VIRT_UART = VIRT_UART_RegisterCallback(&virtUART0, VIRT_UART_RXCPLT_CB_ID, postmanSS_virt_UART0_cb);
	STOP_ON_ERROR(returnStatus_VIRT_UART != VIRT_UART_OK);
}

void postmanSS_new(void)
{
	osStatus_t returnStatus;

	// Semaphore creation
	postmanSSQuitSemHandle = osSemaphoreNew(2U, 0U, &postmanSSQuitSem_attributes);
	STOP_ON_ERROR(postmanSSQuitSemHandle == NULL);

	// Queue creation
	postmanSS_sendQueueHandle = osMessageQueueNew (QUEUE_SIZE, ENCODED_MESSAGE_SIZE, &postmanSS_sendQueue_attributes);
	STOP_ON_ERROR(postmanSS_sendQueueHandle == NULL);

	// Threads creations
	postmanSSRunTaskHandle = osThreadNew(postmanSS_run, NULL, &postmanSSRun_attributes);
	STOP_ON_ERROR(postmanSSRunTaskHandle == NULL);

	returnStatus = osThreadSuspend(postmanSSRunTaskHandle); // Don't start the thread yet
	STOP_ON_ERROR(returnStatus != osOK);

	postmanSSReadTaskHandle = osThreadNew(postmanSS_read, NULL, &postmanSSRead_attributes);
	STOP_ON_ERROR(postmanSSReadTaskHandle == NULL);

	returnStatus = osThreadSuspend(postmanSSReadTaskHandle); // Don't start the thread yet
	STOP_ON_ERROR(returnStatus != osOK);
}

void postmanSS_free(void)
{
	osStatus_t returnStatus;

	// Free the message Queue
	returnStatus = osMessageQueueDelete(postmanSS_sendQueueHandle);
	STOP_ON_ERROR(returnStatus != osOK);
}

void postmanSS_start(void)
{
	osStatus_t returnStatus;

	postmasSSState = RUNNING;

	// Resume threads
	returnStatus = osThreadResume(postmanSSRunTaskHandle);
	STOP_ON_ERROR(returnStatus != osOK);

	returnStatus = osThreadResume(postmanSSReadTaskHandle);
	STOP_ON_ERROR(returnStatus != osOK);
}

void postmanSS_stop(void)
{
	osStatus_t returnStatus;
	postmasSSState = DEAD;

	// Wait for the 2 threads to finish their execution
	returnStatus = osSemaphoreAcquire(postmanSSQuitSemHandle, 0);
	STOP_ON_ERROR(returnStatus != osOK);

	returnStatus = osSemaphoreAcquire(postmanSSQuitSemHandle, 0);
	STOP_ON_ERROR(returnStatus != osOK);
}

void postmanSS_send(encodedMessage_t* msg)
{
	postmanSS_queueSend(msg);
}

/**
 * @brief Thread function that handles the writings on the Virtual UART
 */
static void postmanSS_run(void *argument)
{
	osStatus_t returnStatus;
	encodedMessage_t msg;

	while (postmasSSState == RUNNING)
	{
		// Wait for a new message to come (Blocking call)
		postmanSS_queueReceive(&msg);
		// Send it on the Virtual UART
		VIRT_UART_Transmit(&virtUART0, &(msg.buffer), ENCODED_MESSAGE_SIZE);
	}
	// Signal to the main thread we're out
	returnStatus = osSemaphoreRelease(postmanSSQuitSemHandle);
	STOP_ON_ERROR(returnStatus != osOK);

	osThreadExit();
}

/**
 * @brief Thread function that handles the readings on the Virtual UART
 */
static void postmanSS_read(void *argument)
{
	osStatus_t returnStatus;
	encodedMessage_t msg;

	while (postmasSSState == RUNNING)
	{
		// Check if new message has come
		OPENAMP_check_for_message();

		if (VirtUart0RxMsg == SET)
		{
			VirtUart0RxMsg = RESET;
			uint16_t transactonSize = VirtUart0ChannelRxSize;

			// Handle the case with several messages in one transaction
			while(transactonSize != 0)
			{
				// Error if message is smaller that expected
				STOP_ON_ERROR(transactonSize < ENCODED_MESSAGE_SIZE);

				// Copy message in the buffer
				for(uint8_t i=0; i<ENCODED_MESSAGE_SIZE; i++)
				{
					msg.buffer[i] = VirtUart0ChannelBuffRx[i];
				}

				// Dispatch it
				dispatcherSS_dispatch(&msg);
				transactonSize -= ENCODED_MESSAGE_SIZE;
			}
		}
		osDelay(1); // Short delay before reading again the Virtual UART
	}
	// Signal to the main thread we're out
	returnStatus = osSemaphoreRelease(postmanSSQuitSemHandle);
	STOP_ON_ERROR(returnStatus != osOK);

	osThreadExit();
}

/**
 * @brief Receive message from the postmanSS queue. Blocking function with no timeout.
 *
 * @param *encodedMessage_t: pointer to the message structure that will receive the message
 */
static void postmanSS_queueReceive(encodedMessage_t *msg)
{
	osStatus_t returnStatus;
	returnStatus = osMessageQueueGet(postmanSS_sendQueueHandle, msg->buffer, NULL, 0);
	STOP_ON_ERROR(returnStatus != osOK);
}

/**
 * @brief Send passed event to the doorManager queue. Blocking function with no timeout.
 *
 * @param *encodedMessage_t: pointer to the message structure that contains the message
 */
static void postmanSS_queueSend(encodedMessage_t *msg)
{
	osStatus_t returnStatus;
	returnStatus = osMessageQueuePut(postmanSS_sendQueueHandle, msg->buffer, 0, 0);
	STOP_ON_ERROR(returnStatus != osOK);
}


/**
 * @brief [Open AMP callback]
 * This function is called in the context of postmanSS_read task inside the function OPENAMP_check_for_message.
 * OpenAMP is not thread safe, so we can't do directly the read in the task function
 * FreeRTOS is not able to manage context switching in this situation.
 *
 * @param huart
 */
static void postmanSS_virt_UART0_cb(VIRT_UART_HandleTypeDef *vuartHandle)
{
  VirtUart0ChannelRxSize = vuartHandle->RxXferSize < MAX_BUFFER_SIZE ? vuartHandle->RxXferSize : MAX_BUFFER_SIZE - 1;
  memcpy(VirtUart0ChannelBuffRx, vuartHandle->pRxBuffPtr, VirtUart0ChannelRxSize);

  VirtUart0RxMsg = SET;
}
