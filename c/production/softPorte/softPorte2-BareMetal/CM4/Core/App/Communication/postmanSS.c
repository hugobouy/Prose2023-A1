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

#include "main.h"
#include "openamp.h"

#include "postmanSS.h"
#include "dispatcherSS.h"
#include "util.h"

/*
 * DEFINES
 */

#define MAX_BUFFER_SIZE RPMSG_BUFFER_SIZE


/*
 * TYPEDEF
 */

/*
 * LOCAL FUNCTIONS
 */

static void postmanSS_virt_UART0_cb(VIRT_UART_HandleTypeDef *vuartHandle);

/*
 * LOCAL VARIABLES
 */

// Definitions for Virtual Uart
VIRT_UART_HandleTypeDef virtUART0;

__IO FlagStatus VirtUart0RxMsg = RESET;
uint8_t VirtUart0ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart0ChannelRxSize = 0;

/*******************************************************************************************/

void postmanSS_init(void)
{
	VIRT_UART_StatusTypeDef returnStatus_VIRT_UART;
	// Init Virtual Uart
	log_info("Virtual UART0 OpenAMP-rpmsg channel creation\r\n");
	returnStatus_VIRT_UART = VIRT_UART_Init(&virtUART0);
	STOP_ON_ERROR(returnStatus_VIRT_UART != VIRT_UART_OK);

	// Send addr is the same as receive one
	virtUART0.ept.dest_addr = virtUART0.ept.addr;

	// Register callback
	returnStatus_VIRT_UART = VIRT_UART_RegisterCallback(&virtUART0, VIRT_UART_RXCPLT_CB_ID, postmanSS_virt_UART0_cb);
	STOP_ON_ERROR(returnStatus_VIRT_UART != VIRT_UART_OK);

	log_info("Virtual UART0 successfully initialized\r\n");
}


void postmanSS_send(encodedMessage_t* msg)
{
	VIRT_UART_StatusTypeDef returnStatus_VIRT_UART;
	log_info("Sending: 0x0%d,0x0%d\r\n", msg->buffer[0], msg->buffer[1]);
	returnStatus_VIRT_UART = VIRT_UART_Transmit(&virtUART0, msg->buffer, ENCODED_MESSAGE_SIZE);
	log_info("Virtual UART0 - Sent %d bytes, return status: %d\r\n", ENCODED_MESSAGE_SIZE, (uint8_t) returnStatus_VIRT_UART);
	STOP_ON_ERROR(returnStatus_VIRT_UART != VIRT_UART_OK);
}


void postmanSS_read(void)
{
	encodedMessage_t msg;

	// Check if new message has come
	OPENAMP_check_for_message();

	if (VirtUart0RxMsg == SET)
	{
		VirtUart0RxMsg = RESET;
		uint16_t transactonSize = VirtUart0ChannelRxSize;

		log_info("Received %d bytes on the VirtualUART \r\n", transactonSize);

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
}


/*
 * @brief Callback function for the OPENAMP_check_for_message() call
 * 		   This function should not be called directly by the app
 *
 * @param VIRT_UART_HandleTypeDef: handle of the Virtual UART instance
 */
static void postmanSS_virt_UART0_cb(VIRT_UART_HandleTypeDef *vuartHandle)
{
  VirtUart0ChannelRxSize = vuartHandle->RxXferSize < MAX_BUFFER_SIZE ? vuartHandle->RxXferSize : MAX_BUFFER_SIZE - 1;
  memcpy(VirtUart0ChannelBuffRx, vuartHandle->pRxBuffPtr, VirtUart0ChannelRxSize);

  VirtUart0RxMsg = SET;
}
