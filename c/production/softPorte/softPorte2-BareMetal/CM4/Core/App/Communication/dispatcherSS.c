/**
 * @file 	dispatcherSS.c
 * @brief 	Read the VUART uart and dispatch incoming messages to the app modules
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
#include "dispatcherSS.h"
#include "doorManager.h"
#include "uisp.h"
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

/*
 * LOCAL VARIABLES
 */

/*******************************************************************************************/


void dispatcherSS_dispatch(encodedMessage_t *encodedMsg)
{
	decodedMessage_t decodedMsg;

	decodedMsg.CMD = encodedMsg->buffer[0];
	decodedMsg.TARGET = encodedMsg->buffer[1];

	switch(decodedMsg.CMD)
	{
		case ERR:
			log_info("ERR messaged decoded\r\n");
			STOP_ON_ERROR(true); // SoftSonnette is throwing an error, stop the program
			break;

		case ASK_OPEN_DOOR:
			log_info("ASK_OPEN_DOOR messaged decoded\r\n");
			doorManager_askOpenDoor();
			break;

		case ASK_DOOR_STATE:
			log_info("ASK_DOOR_STATE messaged decoded\r\n");
			doorManager_askDoorState();
			break;

		case SIGNAL_NOT_ALLOWED:
			log_info("SIGNAL_NOT_ALLOWED messaged decoded\r\n");
			uisp_signalNotAllowed();
			break;

		case QUIT_SP:
			log_info("QUIT_SP messaged decoded\r\n");
			uisp_quitSP();
			break;

		case ACK_START_COM:
			log_info("ACK_START_COM messaged decoded\r\n");
			uisp_ackStartCom();
			break;

		// All other messages should be considered as sus
		default:
			log_info("Non recognized messaged decoded: CMD=%d, TARGET=%d\r\n", decodedMsg.CMD, decodedMsg.TARGET);
			STOP_ON_ERROR(true);
			break;
	}
}
