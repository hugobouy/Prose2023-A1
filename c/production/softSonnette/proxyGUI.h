/**
 * @file 	proxyGUI.h
 * @brief	Header du fichier proxyGUI.c
 * @author 	CHIRON Paul
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

#ifndef PROXYGUI_H
#define PROXYGUI_H

/*
 * INCLUDES
 */

#include <stdio.h>
#include "common.h"

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
 * @brief Méthode simulant la méthode updateDoorState() de GUI.
 * 
 * @param state L'état de la porte à communiquer
 */
void proxyGUI_updateDoorState(bool state);

/**
 * @brief Méthode simulant la méthode setEmployeeList de GUI.
 * 
 * @param employeeID L'ID de l'employé courant.
 * @param name Le nom de l'employé courant.
 * @param surname Le prénom de l'employé courant.
 * @param role Le rôle de l'employé courant.
 * @param workingHours Les horaires d'accès de l'employé courant.
 */
void proxyGUI_setEmployeeList(uint8_t employeeID, char* name, char* surname, uint8_t role, Hour workingHours[7][2]);

#endif //PROXYGUI_H