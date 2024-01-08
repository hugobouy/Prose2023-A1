/**
 * @file 	protocolSS.c
 * @brief 	Module définissant le protocole de communication entre SoftSonnette et AOP, les fonctions liées à l'encodage et au décodage des trames.
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

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "protocolSS.h"
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


char* protocolSS_encode(int cmd_id, int nbargs, char *data)
{
    //Initialisation de la trame
    Frame frame;
    frame.cmdId = cmd_id;
    frame.nbArgs = nbargs;

    //trame "vide" ou trame de données/arguments
    if(data == NULL)
    {
        frame.dataLength = 0;
        frame.data = NULL;

        char *bytes; //création de la chaîne d'octets et de la taille de trame
        int frameSize;

        //création et affichage de la trame
        protocolSS_buildRawFrame(frame, &bytes, &frameSize);

        return(bytes);
    }
    else
    {
        frame.dataLength = strlen(data);
        frame.data = data;

        char *dataBytes; //création de la chaîne d'octets et de la taille de trame
        int dataSize;

        //création de la trame de donnée intermédiaire
        protocolSS_buildData(frame, &dataBytes, &dataSize, data);
        frame.dataLength = dataSize; //réaffectation de la taille à cause des bits de contrôle/séparation
        frame.data = dataBytes; //réaffectation de la data pour la même raison

        char *bytes; //création de la chaîne d'octets et de la taille de trame
        int frameSize;

        //création et affichage de la trame
        protocolSS_buildFrame(frame, &bytes, &frameSize);

        return(bytes);
    }
}

void protocolSS_buildFrame(Frame frame, char **bytes, int *frameSize)
{

    //calcul de la taille de la trame selon les bits de début/fin de trame, les arguments de frame et la longueur de la trame (sur 2 octets)
    uint16_t size = sizeof(frame.cmdId) + sizeof(frame.nbArgs) + frame.dataLength + sizeof(uint16_t);
    uint16_t network_order_size = htons(size);
    
    TRACE("frameSize : %d\n", size);

    //création et allocation de la mémoire de la chaîne d'octets
    char * result = malloc(size + sizeof(uint16_t));
    if(!result)
    {
        printf("Erreur malloc");
        return;
    }

    //remplissage de la chaîne result dans l'ordre défini par le protocole de communication
    memcpy(result, &network_order_size, sizeof(uint16_t));
    memcpy(result + sizeof(uint16_t), &frame.cmdId, sizeof(frame.cmdId));
    memcpy(result + sizeof(frame.cmdId) + sizeof(uint16_t), &frame.nbArgs, sizeof(frame.nbArgs));
    memcpy(result + sizeof(frame.cmdId) + sizeof(frame.nbArgs) + sizeof(uint16_t), frame.data, frame.dataLength);

    //affectation des variables de résultat
    *bytes = result;
    *frameSize = size;
}

void protocolSS_buildRawFrame(Frame frame, char **bytes, int *frameSize)
{

    //calcul de la taille de la trame selon les bits de début/fin de trame, les arguments de frame et la longueur de la trame (sur 2 octets)
    uint16_t size = sizeof(uint8_t) + sizeof(frame.cmdId) + sizeof(uint16_t);
    uint16_t network_order_size = htons(size);
    
    TRACE("frameSize : %d\n", size);

    //création et allocation de la mémoire de la chaîne d'octets
    char * result = malloc(size + sizeof(uint16_t));
    if(!result)
    {
        printf("Erreur malloc");
        return;
    }

    //remplissage de la chaîne result dans l'ordre défini par le protocole de communication
    memcpy(result, &network_order_size, sizeof(uint16_t));
    memcpy(result + sizeof(uint16_t), &frame.cmdId, sizeof(frame.cmdId));
    memcpy(result + sizeof(frame.cmdId) + sizeof(uint16_t), &frame.nbArgs, sizeof(frame.nbArgs));

    //affectation des variables de résultat
    *bytes = result;
    *frameSize = size;
}

void protocolSS_destroyFrame(char* frame, char* allArgs)
{
    //libération de la mémoire liée aux arguments
    TRACE("destroy\n");
    if(allArgs != NULL)
    {
        memset(allArgs, 0, strlen(allArgs));
        free(allArgs); 
    }
    //libération de la mémoire liée à la trame
    free(frame);
    TRACE("destroy ok\n");
}

void protocolSS_buildData(Frame frame, char **dataBytes, int *dataSize, char args[])
{

    int nbSeparator = (frame.nbArgs - 1)*sizeof(uint8_t); //détermination du nombre de caractère de séparation '|' 
    if(!frame.nbArgs) nbSeparator = 0;
    //TRACE("nb separator : %d\n", nbSeparator);
    
    //calcul de la taille "réelle" des données avec bits de contrôle et de taille d'arguments
    int size = frame.dataLength + frame.nbArgs*sizeof(uint16_t) - nbSeparator;
    //TRACE("dataLength : %d\n", size);

    //création et allocation de la mémoire de la chaîne d'octets
    char * result = malloc(size);
    if(!result)
    {
        printf("Erreur malloc");
        return;
    }
    
    //séparation du premier argument en chaîne de caractère
    char *currentArg = strtok(args, "|");
    //TRACE("current arg : %s\n", currentArg);

    int padding = 0; //décalage de l'adresse du pointeur result
    for(int k = 0; k < frame.nbArgs; k++)
    {
        //remplissage de la chaîne result pour chaque argument dans l'ordre défini par le protocole de communication

        size_t len = strlen(currentArg);
        uint16_t network_order_size = htons(len);

        memcpy(result + padding, &network_order_size, sizeof(uint16_t));
        
        for (size_t i = 0; i < len; i++) {
            memcpy(result + padding + sizeof(uint16_t) + i*sizeof(char), &currentArg[i], sizeof(char));
        }

        //actualisation du décalage et de l'argument courant
        padding += sizeof(uint16_t) + len*sizeof(char);
        currentArg = strtok(NULL, "|");
    }

    //affectation des variables de résultat
    *dataBytes = result;
    *dataSize = size;
}

Decoded_Frame protocolSS_decode(char* frame, uint16_t frameSize)
{
    uint8_t nbArgs = (uint8_t)frame[0]; //déclaration des paramètres à récupérer dans la trame
    uint16_t lenArgs[nbArgs];

    memset(lenArgs, 0, nbArgs); //initialisation des tableaux/pointeurs de longueurs d'arguments et d'arguments
    char** args = malloc(nbArgs * sizeof(char*));

    uint16_t index = 0;
    uint8_t argIndex = 0;

    //début de la trame
    nbArgs = (uint8_t)frame[index];
    index++;

    for(int i = 0; i < nbArgs; i++)
    {
        //affectation des paramètres selon les flags de lecture 
        lenArgs[i] = protocolSS_getIntLength(frame[index + 1], frame[index]);
        args[i] = malloc(lenArgs[i]);
        index+=2;
        for(uint16_t currentArgIndex = index; currentArgIndex < index + lenArgs[i]; currentArgIndex++)
        {
            args[i][argIndex] = frame[currentArgIndex];
            argIndex++;
        }
        argIndex = 0;
        index+= lenArgs[i];
    }

    //création de la structure de trame décodée et affectations
    Decoded_Frame result;
    result.nbArgs = nbArgs;

    result.lenArgs = malloc(result.nbArgs * sizeof(uint16_t));
    memcpy(result.lenArgs, lenArgs, result.nbArgs * sizeof(uint16_t));

    result.args = malloc(result.nbArgs * sizeof(char*));
    for (int i = 0; i < result.nbArgs; i++) {
        result.args[i] = malloc(lenArgs[i] + 1);  
        memcpy(result.args[i], args[i], lenArgs[i]);
        result.args[i][lenArgs[i]] = '\0';
    }  

    return result;
}

Decoded_Header protocolSS_decodeHeader(char* header)
{
    Decoded_Header result;
    result.size = protocolSS_getIntLength(header[1], header[0]);
    result.cmdId = header[2];

    return result;
}

uint16_t protocolSS_getIntLength(unsigned char lowByte, unsigned char highByte) {
    uint16_t result = (uint16_t)(highByte << 8) + (uint16_t)lowByte;
    return result;
}

uint16_t protocolSS_getIntFromString(char* byteString)
{
    char* endPtr; // pointeur vers la fin de la conversion
    uint16_t value = strtol(byteString, &endPtr, 10); //conversion en entier en base 10

    // vérifie les erreurs de conversion
    if (*endPtr != '\0') {
        printf("Erreur de conversion : la chaîne contient des caractères invalides\n");
        return 0;
    }

    return value;
}

void protocolSS_hexdump(const void* data, size_t size) //fonction d'affichage utilisée en debug
{
    const unsigned char* p = data;
    size_t i;
    size_t j;
    //affichage de l'adresse en hexa selon des lignes de 16 octets
    for (i = 0; i < size; ++i) {
        if (i % 16 == 0) {
            printf("%08x ", (unsigned int)i);
        }
        //affichage en hexa de chaque octets
        printf("%02x ", p[i]);
        if (i % 16 - 15 == 0) {
            for (j = 0; j < 16; ++j) {
                int c = p[i - 15 + j];
                printf("%c", isprint(c) ? c : '.');
            }
            printf("\n");
        }
    }
    //affichage en chaîne de caractère
    if (i % 16 != 0) {
        size_t padding = (16 - i % 16) * 3;
        printf("%*s", (int)padding, "");
        for (j = i - i % 16; j < size; ++j) {
            int c = p[j];
            printf("%c", isprint(c) ? c : '.');
        }
        printf("\n");
    }
}

