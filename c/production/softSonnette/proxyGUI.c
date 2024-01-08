/**
 * @file 	proxyGUI.c
 * @brief 	Module de communication simulant le comportement de l'objet GUI d'AOP.
 * @author 	CHIRON Paul
 * @date 	05/06/2023
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "protocolSS.h"
#include "proxyGUI.h"
#include "postmanAOP.h"
#include "common.h"

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

void proxyGUI_updateDoorState(bool state)
{
    char* allArgs = malloc(sizeof(state) + 1);
    allArgs[0] = '\0';
    sprintf(allArgs, "%d", state);
    TRACE("converted data : %s\n", allArgs);

    char* frame = protocolSS_encode(AOP_CMD_UPDATE_DOOR_STATE,1,allArgs); //encodage de la trame

    postmanAOP_askSendMessage(frame); //demande d'envoi du message au facteur

    protocolSS_destroyFrame(frame, allArgs);
}

void proxyGUI_setEmployeeList(uint8_t employeeID, char* name, char* surname, uint8_t role, Hour workingHours[7][2])
{
    if(employeeID != 0) //test si fin de liste 
    {
        char myWorkingHours[14]; //traduction des wokingHours en chaîne de caractère pour l'encodage
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                myWorkingHours[2*i + j] = workingHours[i][j].wh + 1;
            }
        }

        char* allArgs = malloc(sizeof(employeeID) + strlen(name) + strlen(surname) + sizeof(role) + sizeof(myWorkingHours) + 4);
        allArgs[0] = '\0';
        sprintf(allArgs, "%d|%s|%s|%d|%.*s", employeeID, name, surname, role, (int)sizeof(myWorkingHours), myWorkingHours);

        char* frame = protocolSS_encode(AOP_CMD_SET_EMPLOYEE_LIST,5,allArgs); //encodage de la trame

        TRACE("setEmployeeList\n");
        postmanAOP_askSendMessage(frame); //demande d'envoi du message au facteur

        protocolSS_destroyFrame(frame, allArgs);
    }
    else //envoi du message de fin de liste à AOP
    {
        char* allArgs = malloc(sizeof(employeeID) + 1);
        TRACE("size malloc : %d\n", sizeof(employeeID));
        allArgs[0] = '\0';
        sprintf(allArgs, "%d", employeeID);

        char* frame = protocolSS_encode(AOP_CMD_SET_EMPLOYEE_LIST,1,allArgs); //encodage de la trame

        postmanAOP_askSendMessage(frame); //demande d'envoi du message au facteur

        protocolSS_destroyFrame(frame, allArgs);
    }

}