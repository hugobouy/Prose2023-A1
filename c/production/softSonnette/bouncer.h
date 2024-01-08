/**
 * @file         bouncer.h
 * @brief        Header du fichier bouncer.c
 * @author         JURET Paul
 * @date         11/05/2023
 *
 * \b License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */


#ifndef BOUNCER_H
#define BOUNCER_H

#include "common.h"

/**
 * @brief Permet de convertir le nom de l'employé reconnu par l'IA en ID pour la suite de la procédure
 * 
 * @param [in] employeeID Employé retourné par l'IA
 */
void bouncer_setRecognizeFace(EmployeeID employeeID);



/**
 * @brief Lance la procédure de reconnaissance faciale
 * 
 */
void bouncer_askFaceRecognition();



/**
 * @brief Méthode permettant de créer la message queue de bouncer
 * 
 */
void bouncer_new();


/**
 * @brief Méthode permettant de lancer le thread de bouncer
 * 
 */
void bouncer_start();


/**
 * @brief Méthode permettant d'arrêter le thread de bouncer
 * 
 */
void bouncer_stop();


/**
 * @brief Méthode permettant libérer la message queue
 * 
 */
void bouncer_free();

#endif
