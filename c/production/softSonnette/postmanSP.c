/**
 * @file 	postmanSP.c
 * @brief 	Module permettant la liaison entre SoftSonnette et SoftPorte avec les autres packages. Envoie et reçoit les trames.
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
#include "postmanSP.h"
#include "dispatcherSP.h"
#include "protocolSP.h"
#include "common.h"

/*
 * DEFINES
 */
 
#define NAME_MQ_POSTMAN_SP  "/mq_postman_sp" 
#define MQ_MSG_COUNT 10 

/*
 * TYPEDEF
 */

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
    char frame[SP_LENGTH];
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
static void * postmanSP_run();
static void postmanSP_mqReceive(MqMsg * aMsg);
static void postmanSP_mqSend(MqMsg * aMsg);
static void postmanSP_performAction(Action anAction, MqMsg * aMsg);
static void * postmanSP_setUpConnection();

 /*
 * LOCAL VARIABLES
 */
static pthread_t postman_thread; //Thread associé au facteur
static pthread_t connection_thread;
static mqd_t postman_sp_mq; //Boîte aux lettres du facteur

static Transition mySm [STATE_NB-1][EVENT_NB] =
{
    [S_ON][E_SEND]={S_ON, A_SEND},
    [S_ON][E_STOP]= {S_DEATH, A_STOP},
};

static int vuart_fd;
static char tx_buffer[BUFFER_SIZE];
static char rx_buffer[BUFFER_SIZE];

/*******************************************************************************************/
uint8_t postmanSP_start()
{
    if(pthread_create(&connection_thread, NULL, postmanSP_setUpConnection, NULL) != 0)
    {
		fprintf(stderr, "pthread_create postmanSP error\n");
		fflush(stderr);
		return 1;
    }
    else
    {
        pthread_detach(connection_thread);
    }
    if(pthread_create(&postman_thread, NULL, postmanSP_run, NULL) != 0)
    {
		fprintf(stderr, "pthread_create postmanSP error\n");
		fflush(stderr);
		return 1;
    }
    else
    {
        pthread_detach(postman_thread);
    }

    dispatcherSP_start();

    return 0;
}

uint8_t postmanSP_new()
{
    int check;
    struct mq_attr mqa = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(MqMsg),
        .mq_flags = 0,
    };
    check = mq_unlink(NAME_MQ_POSTMAN_SP);
    if((check == -1) && (errno == ENOENT))
    {
        TRACE("mq doesn't exist : %d\n", check);
    }
	postman_sp_mq = mq_open(NAME_MQ_POSTMAN_SP, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mqa);
    if(postman_sp_mq == -1)
    {
		perror("mq_open postmanSP error\n");
		return 1;
    }

    if(dispatcherSP_new() != 0)
    {
        postmanSP_free();
        return 1;
    }
    return 0;
}

uint8_t postmanSP_free()
{
	int check;
    check = mq_close(postman_sp_mq);
    if(check == -1)
    {
		perror("mq_close postmanSP error\n");
		return 1;
    }
	check = mq_unlink(NAME_MQ_POSTMAN_SP);
    if(check == -1)
    {
		perror("mq_unlink postmanSP error\n");
		return 1;
    }
    return 0;
}

void postmanSP_stop(void)
{
    MqMsg msg = {.data.event = E_STOP};
    postmanSP_mqSend(&msg);
    dispatcherSP_stop();
}

char* postmanSP_readMessage(int size) 
{
    memset(rx_buffer, 0, sizeof(rx_buffer));
    TRACE("wait read softPorte\n");
    int check = read(vuart_fd, rx_buffer, size);
    if(check < 0)
    {
        perror("Failed to read from VUART RPMsg channel");
        close(vuart_fd);
    }
    TRACE("SP message received, %d\n", check);
    //protocolSP_hexdump(rx_buffer);

    return rx_buffer;
}


void postmanSP_askSendMessage(char* frame)
{
    MqMsg msg;
    msg.data.event = E_SEND;
    memcpy(msg.data.frame, frame, SP_LENGTH);

    postmanSP_mqSend(&msg);
}

/**
 * @brief Fonction d'initialisation du serveur
 * 
 */
static void * postmanSP_setUpConnection()
{
    FILE *state_file;

    // Démarrage du Cortex-M 
    state_file = fopen(REMOTEPROC_DEV "state", "w");
    if (state_file == NULL) {
        perror("Failed to open state file");
        return NULL;
    }
    fprintf(state_file, "start");
    fclose(state_file);

    sleep(1);

    vuart_fd = open("/dev/ttyRPMSG0", O_RDWR);
    if (vuart_fd < 0) {
        perror("Failed to open VUART RPMsg channel");
        return NULL;
    }

    dispatcherSP_setConnected(true);
    TRACE("Cortex-M firmware started successfully.\n");

    return NULL;
}

/**
 * @brief Fonction de réception des messages sur la boîte aux lettres
 * 
 * @param aMsg Le message à lire
 */
static void postmanSP_mqReceive(MqMsg * aMsg)
{
	int check;
	check = mq_receive(postman_sp_mq, aMsg->buffer, sizeof(MqMsg), NULL);
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
static void postmanSP_mqSend(MqMsg * aMsg)
{
	int check;
    check = mq_send(postman_sp_mq, aMsg->buffer, sizeof(MqMsg), 0);
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
static void postmanSP_performAction(Action anAction, MqMsg * aMsg)
{
	switch (anAction)
    {
    case A_NOP: 
        break;
    case A_SEND:
        TRACE("about to send\n");
        strcpy(tx_buffer, aMsg->data.frame);
        int check = write(vuart_fd, tx_buffer, SP_LENGTH);
        if(check < 0)
        {
            perror("Failed to write to VUART RPMsg channel");
            close(vuart_fd);
        }
        else
        {
            TRACE("sent : %d\n", check);
            tx_buffer[0] = '\0';
        }
        break;
    case A_STOP:
        FILE *state_file;
        state_file = fopen(REMOTEPROC_DEV "state", "w");
        if (state_file == NULL) {
            perror("Failed to open state file");
        }
        fprintf(state_file, "stop");
        fclose(state_file);
        TRACE("postmanSP stopped\n");
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
static void * postmanSP_run()
{
	MqMsg msg;
    State myState = S_ON;
    Transition * myTrans;
	while (myState != S_DEATH)
    {
        postmanSP_mqReceive(&msg);
        myTrans = &mySm[myState][msg.data.event];
        //TRACE("myState : %d\n", myState);
        //TRACE("event : %d\n", msg.data.event);
        //TRACE("dest state : %d\n", myTrans->destinationState);
        TRACE("action : %d\n", myTrans->action);
        if (myTrans->destinationState != S_FORGET)
        {
            postmanSP_performAction(myTrans->action, &msg);
            myState = myTrans->destinationState;
        }
        else
        {
            TRACE("PostmanSP : Event %d ignoré\n", msg.data.event);
        }
    }

    return NULL;
}
