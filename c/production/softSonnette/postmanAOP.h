/**
 * @file 	postmanAOP.h
 * @brief	Header du fichier postmanAOP.c
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

#ifndef POSTMAN_AOP_H
#define POSTMAN_AOP_H

/*
 * INCLUDES
 */

#include <stdint.h>
#include <stdarg.h>

/*
 * DEFINES
 */

#define SERVER_PORT 1253
#define MAX_PENDING_CONNECTIONS 10
#define MAX_TCP_LENGTH 1500

/*
 * TYPEDEF
 */

/*
 * PUBLIC FUNCTIONS
 */

/**
 * @brief Fonction d'allocation de la mémoire liée au facteur
 *  
 * @return uint8_t Potentiel code d'erreur
 */
uint8_t postmanAOP_new();

/**
 * @brief Fonction d'initialisation et de démarrage du facteur
 * 
 * @return uint8_t Potentiel code d'erreur
 */
uint8_t postmanAOP_start();

/**
 * @brief Fonction d'arrêt du serveur C.
 */
void postmanAOP_stop(void);

/**
 * @brief Fonction de libération de la mémoire liée au facteur
 *  
 * @return uint8_t Potentiel code d'erreur
 */
uint8_t postmanAOP_free();

/**
 * @brief Fonction de lecture de la socket.
 * @param [in] size La taille du message à lire
 * @return char* Le message lu par le facteur
 */
char* postmanAOP_readMessage(int size);

/**
 * @brief Fonction d'écriture sur la socket.
 * @param [in] frame La trame de donnée à envoyer.
 */
void postmanAOP_askSendMessage(char* frame);

#endif //POSTMAN_AOP_H
