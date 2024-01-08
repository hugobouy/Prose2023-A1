Package comprenant le code du protocole de communication ainsi que les dispatcheur et facteur de SoftSonnette

# Protocole de communication

Cette partie décrit la structure de la trame de communication TCP/IP et indique comment utiliser les fonctions d'encodage et de décodage pour la partie C 

## Structure de la trame

Voici la forme de la trame de communication :
```
+----------------+--------------+--------------+--------------+----------------+----------------+
| Début de trame | Longueur de  | Identifiant  | Nombre       | Données        | Fin de trame   |
|                | trame        | de commande  | d'arguments  |                |                |
| (1 octet)      | (2 octets)   | (1 octet)    | (1 octet)    | (variable)     | (1 octet)      |
+----------------+--------------+--------------+--------------+----------------+----------------+
```
Détail de la partie Données (si elle existe, soit nbArgs > 0) : 
```
+----------------+--------------+--------------+----------------+----------------+
| Début          | Longueur de  | Identifiant  | Argument       | Fin d'argument |
| d'argument     | l'argument   | de commande  |                |                |  (fois nbArgs...)
| (1 octet)      | (2 octets)   | (1 octet)    | (lenArg)       | (1 octet)      |
+----------------+--------------+--------------+----------------+----------------+
```
Remarques importantes : 

Début de trame : 0x3c

Longueur de trame : entier uint16_t codé sur 2 octets en format binaire et en LITTLE ENDIAN (1 = 0x01 0x00)

Identifiant de commande : entier uint8_t, peut prendre les valeurs suivantes (à confirmer) : 

* 0 (0x00) : ERR
* 1 (0x01) : ACK 
* 2 (0x02) : SUBSCRIBE_VIDEO
* 3 (0x03) : SET_VIDEO_PICTURE 
* 4 (0x04) : ASK_CHECK_PASS
* 5 (0x05) : VALIDATE_PASS
* 6 (0x06) : SET_TIME
* 7 (0x07) : ASK_OPEN_DOOR
* 8 (0x08) : GET_DOOR_STATE
* 9 (0x09) : UPDATE_DOOR_STATE
* 10 (0x0a) : ADD_EMPLOYEE 
* 11 (0x0b) : DELETE_EMPLOYEE
* 12 (0x0c) : GET_EMPLOYEE_LIST
* 13 (0x0d) : GET_EMPLOYEE
* 14 (0x0e) : RETURN_DOOR_STATE
* 15 (0x0f) : RETURN_GET_EMPLOYEE

Nombre d'arguments : entier uint8_t

Début d'argument : 0x7b

Longueur de l'argument : entier uint16_t codé sur 2 octets en format binaire et en LITTLE ENDIAN (1 = 0x01 0x00)

Argument : chaîne de caractères, si un des arguments est un entier il sera encodé sous forme de char (ex 20 = 0x32 0x30) mais est reformé sous forme d'entier lors du décodage

Fin d'argument : 0x7d

Fin de trame : 0x3e

## Exemples

Ci dessous les hexdumps de quelques trames : 

Trame brute (sans données, pour les méthodes sans arguments), avec un identifiant de commande de 1 et un nombre d'arguments de 0 : 

00000000 3c 06 00 01 00 3e                               <....>

Longueur de la trame = 0x06 0x00 = 6


Trame avec un identifiant de commande de 2 et un nombre d'arguments de 2 : 

00000000 3c 10 00 02 02 7b 01 00 35 7d 7b 01 00 30 7d 3e <....{..5}{..0}>

Longueur de la trame = 0x10 0x00 = 16
Longueur argument 1 = 0x01 0x00 = 1, donnée = 0x35 = '5'
Longueur argument 2 = 0x01 0x00 = 1, donnée = 0x30 = '0'


Trame avec un identifiant de commande de 4 et un nombre d'arguments de 1 : 

00000000 3c 0e 00 04 01 7b 04 00 31 32 33 34 7d 3e       <....{..1234}>

Longueur de la trame = 0x0e 0x00 = 14
Longueur argument 1 = 0x04 0x00 = 4, donnée = 0x31 0x32 0x33 0x34 = '1234'

## Utilisation (C)

#include "protocol.h"

Pour créer une trame sans argument : 

```
char* frame = protocol_encode(1,0,NULL); //encode prend en argument l'identifiant de commande, le nombre d'argument et la chaîne d'arguments
//Utilisation de la trame (ex envoi)
protocol_destroyFrame(frame, NULL); //destroyFrame prend en argument la trame et la chaîne d'arguments
```

Pour créer une trame avec arguments :
 
```
bool boolean;
uint8_t entier;
char caractere;
char chaine[];

char* allArgs = malloc(sizeof(boolean) + sizeof(entier) + strlen(caractere) + strlen(chaine); //selon données à envoyer
allArgs[0] = '\0';

protocol_convertData(allArgs, "%d|%d|%c|%s", boolean, entier, caractere, chaine); 
//convertData prend un nombre d'argument variable pour convertir l'ensemble des données en une même chaîne de caractère. 
//Séparer manuellement chaque argument par un '|'.
//Exemple avec un argument : protocol_convertData(allArgs, "%d", entier);

char* frame = protocol_encode(13,1,allArgs);
protocol_destroyFrame(frame, allArgs);
```

Ci dessous le main.c utilisé pour tester la classe lors de la production

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "protocol.h"
#include "postmanSoftSonnette.h"
#include "pispatcherSoftSonnette.h"

#define TEST_FRAME 0
#define TEST_COM 1

void testEncodeTrame(uint8_t entier);
void testDecodeTrame(char* frame);
void testEncodeTrame2(char string[]);

int main(){
    #if TEST_FRAME
    testEncodeTrame(56);
    testEncodeTrame2("1234");
    #elif TEST_COM
    postmanSoftSonnette_setUpConnection();
    dispatcherSoftSonnette_start();

	pthread_exit(EXIT_SUCCESS);
    #endif
}

void testEncodeTrame(uint8_t entier)
{
    char* allArgs = malloc(sizeof(entier));
    allArgs[0] = '\0';
    protocol_convertData(allArgs, "%d", entier);
    char* frame = Protocol_encode(13,1,allArgs);
    protocol_destroyFrame(frame, allArgs);
}

void testEncodeTrame2(char string[])
{
    char* allArgs = malloc(strlen(string));
    allArgs[0] = '\0';
    protocol_convertData(allArgs, "%s", string);
    char* frame = protocol_encode(4,1,allArgs);
    protocol_destroyFrame(frame, allArgs);
}
```
Ci dessous un programme client.c utilisé pour simulé la connexion Android pour envoyer une trame au facteur SoftSonnette

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "protocol.h"
#include "postmanSoftSonnette.h"


int main()
{
    int un_socket;
    
    struct sockaddr_in adresse_du_serveur;

    un_socket = socket(PF_INET, SOCK_STREAM, 0);

    adresse_du_serveur.sin_family = AF_INET;
    adresse_du_serveur.sin_port = htons(SERVER_PORT);
    adresse_du_serveur.sin_addr = *((struct in_addr *)gethostbyname ("127.0.0.1")->h_addr_list[0]);

    connect(un_socket, (struct sockaddr *)&adresse_du_serveur, sizeof(adresse_du_serveur));


    uint8_t entier = 5;
    char* allArgs = malloc(sizeof(entier));
    allArgs[0] = '\0';
    protocol_convertData(allArgs, "%d", entier);
    char* frame = protocol_encode(13,1,allArgs);
    
    int frameLen = protocol_getIntLength(frame[2], frame[1]);
    int quantite_envoyee = write(un_socket, frame, frameLen);
    protocol_destroyFrame(frame, allArgs);

    close(un_socket);

    return 0;
}
```
