/**
 * @file 	proxyConnectionManager.c
 * @brief 	Code du proxy de la classe Connection Manager
 * @author 	LETENNEUR Laurent
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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "proxyConnectionManager.h"
#include "postmanAOP.h"
#include "protocolSS.h"
#include "common.h"


void proxyConnectionManager_validatePass(bool passValidated)
{
  char* allArgs = malloc(sizeof(passValidated)+1);
  allArgs[0] = '\0';

  sprintf(allArgs, "%d", passValidated); 

  char* frame = protocolSS_encode(AOP_CMD_VALIDATE_PASS,1,allArgs);
  postmanAOP_askSendMessage(frame);
  protocolSS_destroyFrame(frame, allArgs);
}




