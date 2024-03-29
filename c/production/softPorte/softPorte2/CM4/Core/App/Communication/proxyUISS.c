/**
 * @file 	proxyUISS.c
 * @brief 	Encode messages for UISS and send it to the postmanSS
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

#include "proxyUISS.h"
#include "postmanSS.h"

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

void proxyUISS_askStartCom(void)
{
	decodedMessage_t decodedMsg;
	encodedMessage_t encodedMsg;

	// Fill informations
	decodedMsg.CMD = ASK_START_COM;
	decodedMsg.TARGET = SOFTS_ID;

	// Encode message
	encodedMsg.buffer[0] = decodedMsg.CMD;
	encodedMsg.buffer[1] = decodedMsg.TARGET;

	postmanSS_send(&encodedMsg);
}

void proxyUISS_updateDoorState(bool state)
{
	decodedMessage_t decodedMsg;
	encodedMessage_t encodedMsg;

	// Fill informations
	decodedMsg.CMD = (state?UPDATE_DOOR_STATE_TRUE:UPDATE_DOOR_STATE_FALSE);
	decodedMsg.TARGET = SOFTS_ID;

	// Encode message
	encodedMsg.buffer[0] = decodedMsg.CMD;
	encodedMsg.buffer[1] = decodedMsg.TARGET;

	postmanSS_send(&encodedMsg);
}
