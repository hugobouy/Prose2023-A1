/**
 * @file 	dispatcherSP.c
 * @brief 	Module permettant le décodage des trames reçues ainsi que leur répartition.
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
#include <stdint.h>
#include <string.h>
#include <errno.h>
//#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <semaphore.h>
#include "dispatcherSP.h"

//include objets
#include "uiss.h"
#include "proxyGUI.h"

//include facteur et trame
#include "postmanSP.h"
#include "protocolSP.h"

//#include "common.h"

/*
 * DEFINES
 */

#define NAME_MQ_DISPATCHER_SP  "/mq_dispatcher_sp" 
#define MQ_MSG_COUNT 10 

/*
 * TYPEDEF
 */
typedef enum  {S_FORGET = 0, S_ON, S_DEATH, STATE_NB} State; //Etats du dispatcher 
typedef enum  {E_READ = 0, E_DISPATCH, E_STOP, EVENT_NB} Event; //Evenements du dispatcher : demandes de lecture, envoi et arrêt
typedef enum  {A_NOP = 0, A_DISPATCH, A_STOP} Action ; // Action réalisées par le dispatcher : read et write sur la socket

/**
 * @brief Structure des transitions liées au dispatcher. Etat de destination et action à réaliser
 * 
 */
typedef struct
{
	State destinationState;
	Action action;
} Transition;

/**
 * @brief Structure des données d'un message de la boîte aux lettres. Evènement à traiter et éventuelle donnée
 * 
 */
typedef struct
{
	Event event;
    char* frame;
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

/**
 * @brief Fonction de dispatch. Joue le rôle de décodeur des trames reçues et répartit les messages selon les destinataires.
 */
static void * dispatcherSP_dispatch();


/*
 * LOCAL VARIABLES
 */

static pthread_t dispatcher_thread;
static mqd_t dispatcher_mq; //Boîte aux lettres du dispatcher

static sem_t connection_sem;
static volatile bool sp_connected = false;

/*******************************************************************************************/

uint8_t dispatcherSP_start()
{
    if(pthread_create(&dispatcher_thread, NULL, dispatcherSP_dispatch, NULL) != 0)
    {
		fprintf(stderr, "pthread_create dispatcherSP error\n");
		fflush(stderr);
		return 1;
    }
    else
    {
        pthread_detach(dispatcher_thread);
    }
    return 0;
}

uint8_t dispatcherSP_new()
{
    int check;
    struct mq_attr mqa = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(MqMsg),
        .mq_flags = 0,
    };
    check = mq_unlink(NAME_MQ_DISPATCHER_SP);
    if((check == -1) && (errno == ENOENT))
    {
        TRACE("mq doesn't exist : %d\n", check);
    }
	dispatcher_mq = mq_open(NAME_MQ_DISPATCHER_SP, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mqa);
    if(dispatcher_mq == -1)
    {
		perror("mq_open dispatcherSP error\n");
		return 1;
    }

    if(sem_init(&connection_sem, 0, 0) != 0)
    {
		perror("sem_init connection sem error\n");
		return 1;
    }
    return 0;
}

void dispatcherSP_stop()
{
    dispatcherSP_setConnected(false);
}

uint8_t dispatcherSP_free()
{
    int check;
    check = mq_close(dispatcher_mq);
    if(check == -1)
    {
		perror("mq_close dispatcherSP error\n");
        return 1;
    }
	check = mq_unlink(NAME_MQ_DISPATCHER_SP);
    if(check == -1)
    {
		perror("mq_unlink dispatcherSP error\n");
        return 1;
    }

    if(sem_destroy(&connection_sem) != 0)
    {
		perror("sem_destroy connection sem error\n");
		return 1;
    }
    return 0;

}

static void * dispatcherSP_dispatch()
{
    while(sem_wait(&connection_sem) == -1)
    {
        if(errno == EINTR)
        {
            continue;
        }
        else
        {
            perror("sem_wait connection sem error\n");
            TRACE("end dispatcherSP loop\n");
            return NULL;
        }
    }

    while(sp_connected)
    {
        char* frame = postmanSP_readMessage(SP_LENGTH); //attente de la réception d'une trame

        //protocolSP_hexdump(frame);
        TRACE("start decoding the frame\n");
        Decoded_Frame_SP decodedFrame = protocolSP_decode(frame);

        if(decodedFrame.cmdId > SP_NB_CMD)
        {
            frame = NULL;
        }

        if(frame != NULL)
        {
            Decoded_Frame_SP decodedFrame = protocolSP_decode(frame); //décodage de la trame en une instance de la structure Decoded_Frame_SP
            TRACE("decoded\n");

            //répartition des trames selon leur identifiant de commande
            switch(decodedFrame.cmdId)
            {
                case SP_CMD_ERR:
                    TRACE("Err");
                    break;
                case SP_CMD_UPDATE_DOOR_STATE_TRUE:
                    TRACE("updateDoorState(true)\n");

                    if(decodedFrame.target == 0)
                    {
                        uiss_updateDoorState(true);
                    }
                    else if(decodedFrame.target == 1)
                    {
                        proxyGUI_updateDoorState(true);
                    }

                    break;
                case SP_CMD_UPDATE_DOOR_STATE_FALSE:
                    TRACE("updateDoorState(false)\n");

                    if(decodedFrame.target == 0)
                    {
                        uiss_updateDoorState(false);
                    }
                    else if(decodedFrame.target == 1)
                    {
                        proxyGUI_updateDoorState(false);
                    }

                    break;
                case SP_CMD_ASK_START_COM:
                    TRACE("askStartCom\n");

                    uiss_askStartCom();
                    break;
                default:
                    TRACE("Cmd_Id n°%d non traité pour l'instant\n", decodedFrame.cmdId);
                    break;
            }
        }
    }
    TRACE("end dispatcher loop\n");

    return NULL;
}

void dispatcherSP_setConnected(bool state)
{
    if(state)
    {
        sp_connected = true;
        sem_post(&connection_sem);
    }
    else
    {
        sp_connected = false;
    }
}
