/**
 * @file 	uisp.h
 * @brief	util file defining global macros
 * @author 	BOUY Hugo
 * @date 	06/06/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

#ifndef APP_UTIL_H_
#define APP_UTIL_H_

/*
 * INCLUDES
 */

#include "main.h"

/*
 * DEFINES
 */

#define ERR 						(uint8_t) 0x00
#define ASK_OPEN_DOOR				(uint8_t) 0x01
#define UPDATE_DOOR_STATE_TRUE  	(uint8_t) 0x02
#define UPDATE_DOOR_STATE_FALSE		(uint8_t) 0x0A
#define ASK_DOOR_STATE				(uint8_t) 0x04
#define SIGNAL_NOT_ALLOWED			(uint8_t) 0x05
#define LAUNCH_SP					(uint8_t) 0x06
#define QUIT_SP						(uint8_t) 0x07
#define ASK_START_COM				(uint8_t) 0x08
#define ACK_START_COM				(uint8_t) 0x09

#define SOFTS_ID	0
#define AOP_ID		1
#define SOFTP_ID	2

#define ENCODED_MESSAGE_SIZE	2

/*
 * TYPEDEF
 */

typedef struct
{
	uint8_t CMD;
	uint8_t TARGET;
}decodedMessage_t;

typedef union
{
	uint8_t buffer[ENCODED_MESSAGE_SIZE];
}encodedMessage_t;

/*
 * MACROS
 */

extern volatile bool debug;
#define STOP_ON_ERROR(error_condition) if(error_condition) {while(debug);}


#endif /* APP_UTIL_H_ */
