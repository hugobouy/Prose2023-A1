/**
 * @file 	protocolSP.h
 * @brief	Header du fichier protocolSP.c
 * @author 	CHIRON Paul
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

#ifndef PROTOCOLSP_H
#define PROTOCOLSP_H

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdint.h>

/*
 * DEFINES
 */

/*
 * TYPEDEF
 */


/**
 * @brief Structure de la trame décodée. Comprend l'identifiant de commande et la cible/expéditeur.
 */
typedef struct {
    uint8_t cmdId;
    uint8_t target;
}Decoded_Frame_SP;

/*
 * PUBLIC FUNCTIONS
 */

/**
 * @brief Fonction haut niveau d'encodage de la trame. Construit la trame en utilisant les fonctions intermédiaires d'encodage.
 * 
 * @param [in] cmd_id L'identifiant de commande 
 * @param [in] nbargs Le nombre d'arguments compris dans le message à envoyer
 * @param [in] data La chaîne de caractères contenant les arguments convertis 
 * @return char* La trame encodée 
 */
char* protocolSP_encode(uint8_t cmd_id, uint8_t target);

/**
 * @brief Fonction de décodage de la trame. Reconstruit la trame via une instance de Decoded_Frame.
 * 
 * @param [in] frame La trame à décoder
 * @return Decoded_Frame_SP La structure de trame décodée
 */
Decoded_Frame_SP protocolSP_decode(char* frame);


/**
 * @brief Fonction pour le debug, permettant l'affichage de la trame
 *
 * @param [in] data Trame envoyée
 */
void protocolSP_hexdump(const void* data);

#endif //PROTOCOLSP_H
