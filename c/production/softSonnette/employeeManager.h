/**
 * @file 	employeeManager.h
 * @brief	Header pour le fichier employeeManager.c
 * @author 	Paul CHIRON
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

#ifndef EMPLOYEE_MANAGER_H
#define EMPLOYEE_MANAGER_H

//#include <stdint.h>
#include "common.h"

/*
 * DEFINES
 */

#define MAX_EMPLOYEE 10
#define PHOTO_WIDTH 100
#define PHOTO_HEIGHT 100

/*
 * TYPEDEF
 */


/**
 * @brief Fonction permettant d'ajouter un employé à la liste des employés
 * 
 * @param [in] name Le nom de famille de l'employé
 * @param [in] firstName Le prénom de l'employé
 * @param [in] picture Le chemin local vers la photo de l'employé
 * @param [in] role Le rôle de l'employé
 * @param [in] workingHours Les heures de travail de l'employé
 */
void employeeManager_addEmployee(char name[], char firstName[], Picture picture, Role_e role, Hour workingHours[7][2]);

/**
 * @brief Fonction permettant de supprimer un employé depuis la liste des employés
 * 
 * @param [in] employeeID L'identifiant de l'employé
 */
void employeeManager_deleteEmployee(EmployeeID employeeID);

/**
 * @brief Fonction permettant de retourner la liste des employés
 * 
 * @return Employee_t * La liste des employés
 */
Employee_t* employeeManager_getEmployeeList(void);

/**
 * @brief Fonction permettant de retourner un employé à partir de son ID
 * 
 * @param [in] employeeID L'identifiant de l'employé
 * @return Employee_t L'employé correspondant
 */
Employee_t employeeManager_getEmployee(EmployeeID employeeID);

/**
 * @brief Fonction permettant à AOP de signaler la demande d'envoi de la liste des employés.
 * 
 */
void employeeManager_askEmployeeList();

/**
 * @brief Fonction permettant de sauvegarder la liste des employés dans le fichier CSV
 */
void employeeManager_save();

/**
 * @brief Fonction permettant de charger la liste des employés à partir du fichier CSV
 */
void employeeManager_load();

void employeeManager_storeImage(char* imageBytes, char* filePath);

#endif //EMPLOYEE_MANAGER_H
