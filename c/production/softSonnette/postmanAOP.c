/**
 * @file 	postmanAOP.c
 * @brief 	Module permettant la liaison TCP/IP avec les autres packages. Envoie et reçoit les trames.
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

//#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//#include <stdint.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
//#include <pthread.h>
#include <mqueue.h>
#include "postmanAOP.h"
#include "dispatcherAOP.h"
#include "protocolSS.h"
#include "common.h"

/*
 * DEFINES
 */
 
#define NAME_MQ_POSTMAN_AOP  "/mq_postman_aop" 
#define MQ_MSG_COUNT 10 

/*
 * TYPEDEF
 */

struct sockaddr_in my_address, client_address;

typedef enum  {S_FORGET = 0, S_ON, S_DEATH, STATE_NB} State; //Etats du facteur 
typedef enum  {E_SEND, E_STOP, EVENT_NB} Event; //Evenements du facteur : demandes de lecture, envoi et arrêt
typedef enum  {A_NOP = 0, A_SEND, A_STOP} Action ; // Action réalisées par le facteur : read et write sur la socket

/**
 * @brief Structure des transitions liées au facteur. Etat de destination et action à réaliser
 * 
 */
typedef struct
{
	State destinationState;
	Action action;
} Transition;

/**
 * @brief Structure des données d'un message de la boîte aux lettres. Evènement à traiter et éventuelle donnée, ici une trame à envoyer ou lire
 * 
 */
typedef struct
{
	Event event;
    char frame[MAX_TCP_LENGTH];
    uint16_t frameSize;
} MqMsgData;

/**
 * @brief Structure d'un message de la boîte aux lettres. Donnée définie dans la structure MqMsgData et son buffer associé
 * 
 */
typedef union
{
    MqMsgData data;
    char buffer[sizeof(MqMsgData)];
} MqMsg;

/*
 * LOCAL FUNCTIONS
 */
static void * postmanAOP_run();
static void postmanAOP_mqReceive(MqMsg * aMsg);
static void postmanAOP_mqSend(MqMsg * aMsg);
static void postmanAOP_performAction(Action anAction, MqMsg * aMsg);
static void * postmanAOP_setUpConnection();

 /*
 * LOCAL VARIABLES
 */
static pthread_t postman_thread; //Thread associé au facteur
static pthread_t connection_thread;
static mqd_t postman_mq; //Boîte aux lettres du facteur

static int socketSoftSonnette; //Socket d'écoute 
static int socketAOP; //Socket de donnée

static Transition mySm [STATE_NB-1][EVENT_NB] =
{
    [S_ON][E_SEND]={S_ON, A_SEND},
    [S_ON][E_STOP]= {S_DEATH, A_STOP},
};

/*******************************************************************************************/
uint8_t postmanAOP_start()
{
    if(pthread_create(&connection_thread, NULL, postmanAOP_setUpConnection, NULL) != 0)
    {
        perror("pthread_create postmanAOP connection error\n");
		return 1;
    }
    else
    {
        pthread_detach(connection_thread);
    }
    if(pthread_create(&postman_thread, NULL, postmanAOP_run, NULL) != 0)
    {
		perror("pthread_create postmanAOP read error\n");
		return 1;
    }
    else
    {
        pthread_detach(postman_thread);
    }

    dispatcherAOP_start();

    return 0;
}

uint8_t postmanAOP_new()
{
    int check;
    struct mq_attr mqa = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(MqMsg),
        .mq_flags = 0,
    };
    check = mq_unlink(NAME_MQ_POSTMAN_AOP);
    if((check == -1) && (errno == ENOENT))
    {
        TRACE("mq doesn't exist : %d\n", check);
    }
	postman_mq = mq_open(NAME_MQ_POSTMAN_AOP, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mqa);
    if(postman_mq == -1)
    {
		perror("mq_open postmanAOP error\n");
		return 1;
    }

    if(dispatcherAOP_new() != 0)
    {
        postmanAOP_free();
        return 1;
    }
    return 0;
}

uint8_t postmanAOP_free()
{
	int check;
    check = mq_close(postman_mq);
    if(check == -1)
    {
		perror("mq_close postmanAOP error\n");
		return 1;
    }
	check = mq_unlink(NAME_MQ_POSTMAN_AOP);
    if(check == -1)
    {
		perror("mq_unlink postmanAOP error\n");
		return 1;
    }
    return 0;
}

void postmanAOP_stop(void)
{
    MqMsg msg = {.data.event = E_STOP};
    postmanAOP_mqSend(&msg);
    dispatcherAOP_stop();
}

char* postmanAOP_readMessage(int size) 
{
    char* msg = malloc(size);
    int bytesRead = 0;
    int bytesToRead = size;

    while (bytesRead < bytesToRead) {
        int result = read(socketAOP, msg + bytesRead, bytesToRead - bytesRead);

        if (result < 0) {
            TRACE("Error occurred while reading socket with size %d\n", size);
            free(msg);
            msg = NULL;
            break;
        } else {
            bytesRead += result;
        }
    }
    return msg;
}


void postmanAOP_askSendMessage(char* frame)
{
    MqMsg msg;
    msg.data.event = E_SEND;
    msg.data.frameSize = protocolSS_getIntLength(frame[1], frame[0]);
    memcpy(msg.data.frame, frame, msg.data.frameSize);

    postmanAOP_mqSend(&msg);
    //protocolSS_hexdump(msg.data.frame, msg.data.frameSize);
}

/**
 * @brief Fonction d'initialisation du serveur
 * 
 */
static void * postmanAOP_setUpConnection()
{
    socketSoftSonnette = socket(AF_INET, SOCK_STREAM, 0); //initialisation de la socket d'écoute
    memset(&my_address, 0, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(SERVER_PORT);
    my_address.sin_addr.s_addr = INADDR_ANY;

    bind(socketSoftSonnette, (struct sockaddr *)&my_address, sizeof(my_address)); //liaison de la socket à l'adresse du serveur
    listen(socketSoftSonnette, MAX_PENDING_CONNECTIONS); //passage en mode attente de connexion
    socklen_t clientAddressLength = sizeof(client_address);
    TRACE("wait accept\n");
    socketAOP = accept(socketSoftSonnette, (struct sockaddr *)&client_address, &clientAddressLength);
    if (socketAOP < 0) {
        perror("Erreur lors de la création du socket client");
        exit(EXIT_FAILURE);
    }
    if (socketAOP>0)
    {
        TRACE("connection established\n");
        dispatcherAOP_setConnected(true);
    }

    return NULL;
}

/**
 * @brief Fonction de réception des messages sur la boîte aux lettres
 * 
 * @param aMsg Le message à lire
 */
static void postmanAOP_mqReceive(MqMsg * aMsg)
{
	int check;
	check = mq_receive(postman_mq, aMsg->buffer, sizeof(MqMsg), NULL);
    if (check != sizeof(MqMsg)) {
        perror("Error receiving message via postman mqueue");
    }
    else
    {
        TRACE("Message received from postman mq\n");
    }
}

/**
 * @brief Fonction d'envoi des messages sur la boîte aux lettres
 * 
 * @param aMsg Le message à envoyer
 */
static void postmanAOP_mqSend(MqMsg * aMsg)
{
	int check;
    check = mq_send(postman_mq, aMsg->buffer, sizeof(MqMsg), 0);
    if (check == -1) {
        perror("Error sending message via postman mqueue");
    }
    else
    {
        TRACE("Message sent via postman mq\n");
    }
}

/**
 * @brief Fonction de traitement des actions liées au facteur
 * 
 * @param anAction L'action courante à traiter
 * @param aMsg Le message associé, utilisé s'il contient des trames
 */
static void postmanAOP_performAction(Action anAction, MqMsg * aMsg)
{
	switch (anAction)
    {
    case A_NOP: 
        break;
    case A_SEND:
        ssize_t bytes_sent = write(socketAOP, &aMsg->data.frame, aMsg->data.frameSize);
        while(bytes_sent < aMsg->data.frameSize)
        {
            bytes_sent+= write(socketAOP, &aMsg->data.frame, aMsg->data.frameSize);
        }
        TRACE("MESSAGE SENT : %d\n", bytes_sent);
        break;
    case A_STOP:
        close(socketSoftSonnette);
        break;
    default:
        TRACE("Action inconnue\n");
        break;
    }
}

/**
 * @brief Fonction d'arrière plan du thread lié au facteur; Reçoie les message de la boîte aux lettres
 * 
 * @param aParam Paramètre non utilisé
 * @return void* 
 */
static void * postmanAOP_run()
{
	MqMsg msg;
    State myState = S_ON;
    Transition * myTrans;
	while (myState != S_DEATH)
    {
        postmanAOP_mqReceive(&msg);
        myTrans = &mySm[myState][msg.data.event];
        //TRACE("myState : %d\n", myState);
        //TRACE("event : %d\n", msg.data.event);
        //TRACE("dest state : %d\n", myTrans->destinationState);
        TRACE("action : %d\n", myTrans->action);
        if (myTrans->destinationState != S_FORGET)
        {
            postmanAOP_performAction(myTrans->action, &msg);
            myState = myTrans->destinationState;
        }
        else
        {
            TRACE("PostmanAOP : Event %d ignoré\n", msg.data.event);
        }
    }
    TRACE("postman stopped, wait end of thread\n");
    pthread_barrier_wait(&stop_barrier);
    TRACE("end of thread postman\n");

    return NULL;
}

