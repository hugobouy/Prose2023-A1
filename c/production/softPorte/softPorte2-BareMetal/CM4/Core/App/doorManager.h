/**
 * @file 	doorManager.h
 * @brief	Header file for doorManager.c
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

#ifndef APP_DOORMANAGER_H_
#define APP_DOORMANAGER_H_

/*
 * INCLUDES
 */

/*
 * DEFINES
 */

/*
 * TYPEDEF
 */

/*
 * PUBLIC FUNCTIONS
 */

/**
 * @brief Incoming event asking to open the Door.
 * 		  Unlocks the door and starts the TOP timer.
 */
void doorManager_askOpenDoor(void);

/**
 * @brief Incoming event asking the current Door state
 * 		  Send back the door state
 */
void doorManager_askDoorState(void);

/*
 * @brief Callback for the TOP timer.
 */
void doorManager_TOPCallback(void);

#endif /* APP_DOORMANAGER_H_ */
