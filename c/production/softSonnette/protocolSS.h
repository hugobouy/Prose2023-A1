/**
 * @file 	protocolSS.h
 * @brief	Header du fichier protocolSS.c
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

#ifndef PROTOCOLSS_H
#define PROTOCOLSS_H

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/*
 * DEFINES
 */

/*
 * TYPEDEF
 */

/**
 * @brief Structure de la trame pré-encodée. Comprend l'identifiant de commande, le nombre d'arguments, la chaîne de données et la longueur de celle-ci.
 */
typedef struct
{
    uint8_t cmdId;
    uint8_t nbArgs;
    char *data;
    uint8_t dataLength;
}Frame;

/**
 * @brief Structure de la trame décodée. Comprend l'identifiant de commande, le nombre d'arguments, les tailles et données liées à chaque argument.
 */
typedef struct {
    uint8_t nbArgs;
    uint16_t* lenArgs;
    char** args;
}Decoded_Frame;

typedef struct {
    uint16_t size;
    uint8_t cmdId;
}Decoded_Header;

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
char* protocolSS_encode(int cmd_id, int nbargs, char *data);

/**
 * @brief Fonction de décodage de la trame. Reconstruit la trame via une instance de Decoded_Frame.
 * 
 * @param [in] frame La trame à décoder
 * @return Decoded_Frame La structure de trame décodée
 */
Decoded_Frame protocolSS_decode(char* frame, uint16_t frameSize);

Decoded_Header protocolSS_decodeHeader(char* header);

/**
 * @brief Fonction de création d'une trame comprenant au moins 1 argument.
 * 
 * @param [in] frame La structure de la trame pré-encodée comprenant les informations définies lors de l'encodage
 * @param [in, out] bytes La chaîne d'octets à utiliser
 * @param [in,out] frameSize La taille de la trame complète
 */
void protocolSS_buildFrame(Frame frame, char **bytes, int *frameSize);

/**
 * @brief Fonction de création d'une trame brute ne comprenant pas d'arguments (ou donnée).
 * 
 * @param [in] frame La structure de la trame pré-encodée comprenant les informations définies lors de l'encodage
 * @param [in,out] bytes La chaîne d'octets à utiliser
 * @param [in,out] frameSize La taille de la trame complète
 */
void protocolSS_buildRawFrame(Frame frame, char **bytes, int *frameSize);

/**
 * @brief Fonction de destruction de la trame. Libère la mémoire allouée dynamiquement.
 * 
 * @param [in] frame La trame TCP à détruire
 * @param [in] allArgs La chaîne de caractère comprenant les arguments convertis
 */
void protocolSS_destroyFrame(char* frame, char* allArgs);

/**
 * @brief Fonction de création de la trame intermédiaire de données.
 * 
 * @param [in] frame La structure de la trame pré-encodée comprenant les informations définies lors de l'encodage
 * @param [in,out] dataBytes La chaîne d'octets à remplir
 * @param [in,out] dataSize La taille de la chaîne d'octets
 * @param [in] args La chaîne de caractères contenant les arguments convertis, utilisée pour remplir dataBytes
 */
void protocolSS_buildData(Frame frame, char **dataBytes, int *dataSize, char args[]);

/**
 * @brief Fonction d'affichage de la trame complète.
 * 
 * @param data La chaîne d'octets correspondant à la trame 
 * @param size La taille de la trame 
 */
void protocolSS_hexdump(const void* data, size_t size);

/**
 * @brief Fonction retournant l'entier correspondant à l'addition de deux octets
 * 
 * @param highByte L'octet de poids fort
 * @param lowByte L'octet de poids faible 
 * @return uint16_t Le résultat entier de l'opération
 */
uint16_t protocolSS_getIntLength(unsigned char highByte, unsigned char lowByte);

/**
 * @brief Fonction retournant l'entier correspondant à un chaîne de caractère 
 * 
 * @param byteString La chaîne des caractères devant être convertis
 * @return uint16_t Le résultat entier de l'opération
 */
uint16_t protocolSS_getIntFromString(char* byteString);

#endif //PROTOCOLSS_H
