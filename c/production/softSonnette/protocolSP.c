/**
 * @file 	protocolSP.c
 * @brief 	Module définissant le protocole de communication entre SoftSonnette et SoftPorte, les fonctions liées à l'encodage et au décodage des trames.
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

/*
 * INCLUDES
 */

//#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <stdbool.h>
#include "protocolSP.h"
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

char* protocolSP_encode(uint8_t cmd_id, uint8_t target)
{
    char* frame =  malloc(MAX_TCP_LENGTH);
    memcpy(frame, &cmd_id, sizeof(uint8_t));
    memcpy(frame + sizeof(uint8_t), &target, sizeof(uint8_t));
    return frame;
}

Decoded_Frame_SP protocolSP_decode(char* frame)
{
    Decoded_Frame_SP result;

    result.cmdId = frame[0];
    result.target = frame[1];

    return result;
}

void protocolSP_hexdump(const void* data)
{
    const unsigned char* p = data;
    size_t i;
    size_t j;
    //affichage de l'adresse en hexa selon des lignes de 16 octets
    for (i = 0; i < 2; ++i) {
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
        for (j = i - i % 16; j < 2; ++j) {
            int c = p[j];
            printf("%c", isprint(c) ? c : '.');
        }
        printf("\n");
    }
}
