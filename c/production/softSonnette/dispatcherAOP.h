/**
 * @file 	dispatcherAOP.h
 * @brief	Header du fichier dispatcherAOP.c
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

#ifndef DISPATCHER_SS_H
#define DISPATCHER_SS_H

/*
 * INCLUDES
 */
#include <stdbool.h>

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
 * @brief Fonction d'allocation de la mémoire liée au dispatcheur AOP
 *
 * @return uint8_t Potentiel code d'erreur
 */
uint8_t dispatcherAOP_new();

/**
 * @brief Fonction d'initialisation et de démarrage du dispatcheur AOP
 * 
 * @return uint8_t Potentiel code d'erreur
 */
uint8_t dispatcherAOP_start();

/**
 * @brief Fonction d'arrêt du dispatcheur AOP
 */
void dispatcherAOP_stop();

/**
 * @brief Fonction de libération de la mémoire liée au dispatcheur AOP
 *
 * @return uint8_t Potentiel code d'erreur
 */
uint8_t dispatcherAOP_free();

/**
 * @brief Fonction de notification de l'état de la connexion avec AOP
 * 
 * @param state état de la connexion
 */
void dispatcherAOP_setConnected(bool state);

#endif //DISPATCHER_SS_H
