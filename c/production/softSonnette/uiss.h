/**
 * @file 	uiss.h
 * @brief 	Header du fichier uiss.c
 * @author 	CHIRON Paul
 * @date 	11/05/2023
 *
 * \b License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

#ifndef UISS_H
#define UISS_H

/*
 * INCLUDES
 */

#include <stdint.h>
#include <stdbool.h>

#define TASS 5

/**
 * @brief Fonction d'allocation de la mémoire liée à UISS
 * 
 */
uint8_t uiss_new(void);

/**
 * @brief Fonction d'initialisation et de démarrage d'UISS
 * 
 * @return uint8_t Gestion de potentielles erreurs
 */
uint8_t uiss_start(void);

/**
 * @brief Fonction d'arrêt d'UISS
 * 
 */
void uiss_stop(void);

/**
 * @brief Fonction de libération de la mémoire liée à UISS
 * 
 */
uint8_t uiss_free(void);

/**
 * @brief Fonction de démarrage d'UISS, l'interface utilisateur de SoftSonnette. 
 * 
 */
void uiss_launchSS(void);

/**
 * @brief Fonction d'arrêt d'UISS, l'interface utilisateur de SoftSonnette. 
 * 
 */
void uiss_quitSS(void);

/**
 * @brief Fonction définissant l'action utilisateur d'appui sur le bouton Ring.
 * 
 */
void uiss_ring(void);

/**
 * @brief Fonction demandant la connexion entre SoftPorte et SoftSonnette.
 * 
 */
void uiss_askStartCom();

/**
 * @brief Fonction d'actualisation de l'état de la caméra
 * 
 * @param [in] state Booléen représentant l'état courant de la caméra
 */
void uiss_updateCamState(bool state);

/**
 * @brief Fonction d'actualisation de l'état de la porte
 * 
 * @param [in] state Booléen représentant l'état courant de la porte
 */
void uiss_updateDoorState(bool state);

#endif //UISS_H
