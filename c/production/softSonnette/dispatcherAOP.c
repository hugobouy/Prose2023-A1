/**
 * @file 	dispatcherAOP.c
 * @brief 	Module permettant le décodage des trames TCP/IP reçues de AOP ainsi que leur répartition.
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
//#include <stdlib.h>
//#include <stdint.h>
#include <string.h>
#include <errno.h>
//#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <semaphore.h>

//include objets
#include "cameraman.h"
#include "clock.h"
#include "employeeManager.h"
#include "guard.h"
#include "proxyDoorManager.h"
#include "dispatcherAOP.h"

//include facteur et trame
#include "postmanAOP.h"
#include "protocolSS.h"
//#include "common.h"


/*
 * DEFINES
 */
#define NAME_MQ_DISPATCHER_AOP  "/mq_dispatcher_aop" 
#define MQ_MSG_COUNT 10 
#define HEADER_SIZE 3

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
static void * dispatcherAOP_dispatch();

/*
 * LOCAL VARIABLES
 */

static pthread_t dispatcher_thread;
static mqd_t dispatcher_mq; //Boîte aux lettres du dispatcher

static sem_t connection_sem;
static volatile bool aop_connected = false;

/*******************************************************************************************/

uint8_t dispatcherAOP_start()
{
    if(pthread_create(&dispatcher_thread, NULL, dispatcherAOP_dispatch, NULL) != 0) //création du thread du dispatcher
    {
		perror("pthread_create dispatcherAOP error\n");
		return 1;
    }
    else
    {
        pthread_detach(dispatcher_thread);
    }
    return 0;
}

uint8_t dispatcherAOP_new()
{
    int check;
    struct mq_attr mqa = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(MqMsg),
        .mq_flags = 0,
    };
    check = mq_unlink(NAME_MQ_DISPATCHER_AOP);
    if((check == -1) && (errno == ENOENT))
    {
        TRACE("mq doesn't exist : %d\n", check);
    }
	dispatcher_mq = mq_open(NAME_MQ_DISPATCHER_AOP, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mqa);
    if(dispatcher_mq == -1)
    {
		perror("mq_open dispatcherAOP error\n");
		return 1;
    }

    if(sem_init(&connection_sem, 0, 0) != 0)
    {
		perror("sem_init connection sem error\n");
		return 1;
    }
    return 0;
}

void dispatcherAOP_stop()
{
    dispatcherAOP_setConnected(false);
}

uint8_t dispatcherAOP_free()
{
    int check;
    check = mq_close(dispatcher_mq);
    if(check == -1)
    {
		perror("mq_close dispatcherAOP error\n");
        return 1;
    }
	check = mq_unlink(NAME_MQ_DISPATCHER_AOP);
    if(check == -1)
    {
		perror("mq_unlink dispatcherAOP error\n");
        return 1;
    }

    if(sem_destroy(&connection_sem) != 0)
    {
		perror("sem_destroy connection sem error\n");
		return 1;
    }
    return 0;
}

static void * dispatcherAOP_dispatch()
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
            TRACE("end dispatcherAOP loop\n");
            return NULL;
        }
    }
    TRACE("exit sem_wait\n");

    while(aop_connected) //tant que la connexion est active
    {
        char* header = postmanAOP_readMessage(HEADER_SIZE); //attente de la réception d'une trame
        char* frame;
        uint16_t frameSize;

        if(header != NULL)
        {
            //protocolSS_hexdump(header, HEADER_SIZE);
            TRACE("start decoding the header\n");
        }
        else
        {
            frame = NULL;
            frameSize = 0;
        }

        Decoded_Header decodedHeader = protocolSS_decodeHeader(header);
        TRACE("Cmd : %d\n", decodedHeader.cmdId);
        TRACE("Taille : %d\n", decodedHeader.size);
        if(decodedHeader.cmdId > AOP_NB_CMD) //Cmd en dehors des limites, header non conforme
        {
            frame = NULL;
            frameSize = 0;
        }
        else
        {
            frameSize = decodedHeader.size - HEADER_SIZE; //decodedHeader.size est la taille totale de la trame
            frame = postmanAOP_readMessage(frameSize);
            //protocolSS_hexdump(frame, frameSize);
        }

        if(frame != NULL) //On ne décode que si le header est conforme
        {
            Decoded_Frame decodedFrame = protocolSS_decode(frame, frameSize); //décodage de la trame en une instance de la structure Decoded_Frame
            TRACE("decoded\n");
            char* arg1 = NULL; 
            char* arg2 = NULL;
            char* arg3 = NULL;
            char* arg4 = NULL;
            char* arg5 = NULL;

            //initilaisation des potentiels arguments selon la variable de nombre d'arguments nbArgs
            if (decodedFrame.nbArgs >= 1) {
                arg1 = malloc(decodedFrame.lenArgs[0] + 1); 
                if (arg1 == NULL) {
                    TRACE("Erreur d'allocation de mémoire pour arg1\n");
                }
            }
            if (decodedFrame.nbArgs >= 2) {
                arg2 = malloc(decodedFrame.lenArgs[1] + 1); 
                if (arg2 == NULL) {
                    TRACE("Erreur d'allocation de mémoire pour arg2\n");
                }
            }
            if (decodedFrame.nbArgs >= 3) {
                arg3 = malloc(decodedFrame.lenArgs[2] + 1); 
                if (arg3 == NULL) {
                    TRACE("Erreur d'allocation de mémoire pour arg3\n");
                }
            }
            if (decodedFrame.nbArgs >= 4) {
                arg4 = malloc(decodedFrame.lenArgs[3] + 1); 
                if (arg4 == NULL) {
                    TRACE("Erreur d'allocation de mémoire pour arg4\n");
                }
            }
            if (decodedFrame.nbArgs == 5) {
                arg5 = malloc(decodedFrame.lenArgs[4] + 1); 
                if (arg5 == NULL) {
                    TRACE("Erreur d'allocation de mémoire pour arg5\n");
                }
            }
            
            //répartition des trames selon leur identifiant de commande
            switch(decodedHeader.cmdId)
            {
                case AOP_CMD_ERR:
                    TRACE("Err");
                    break;
                case AOP_CMD_SUBSCRIBE_TO_VIDEO_STREAM:
                    TRACE("subscribeToVideoStream\n");
                    uint8_t enable = atoi(decodedFrame.args[0]); //argument 1 : enable

                    cameraman_subscribeToVideoStream(enable);
                    break;
                case AOP_CMD_ASK_CHECK_PASS:
                    TRACE("askCheckPass\n");
                    strcpy(arg1, decodedFrame.args[0]); //argument 1 : password

                    guard_askCheckPass(arg1);
                    break;
                case AOP_CMD_SET_TIME:
                    TRACE("setCurrentTime\n");
                    Time time;
                    time.year = (decodedFrame.args[0][0] << 8) | decodedFrame.args[0][1];
                    time.month = decodedFrame.args[0][2];
                    time.day = decodedFrame.args[0][3];
                    time.hour = decodedFrame.args[0][4];
                    time.minute = decodedFrame.args[0][5];
                    time.second = decodedFrame.args[0][6];

                    clock_setCurrentTime(time);
                    break;
                case AOP_CMD_ASK_OPEN_DOOR:
                    TRACE("askOpenDoor\n");

                    proxyDoorManager_askOpenDoor();
                    break;
                case AOP_CMD_ASK_DOOR_STATE:
                    TRACE("askDoorState\n");

                    proxyDoorManager_askDoorState();
                    break;
                case AOP_CMD_ADD_EMPLOYEE:
                    TRACE("addEmployee\n");
                    strcpy(arg1, decodedFrame.args[0]); //argument 1 : name
                    strcpy(arg2, decodedFrame.args[1]); //argument 2 : firstName
                    Picture picture = {.filePath = malloc(50)};
                    sprintf(picture.filePath, "%s_%s_image.jpg", arg2, arg1);
                    employeeManager_storeImage(decodedFrame.args[2], picture.filePath);
                    Role_e role = atoi(decodedFrame.args[3]); //argument 4 : role
                    Hour workingHours[7][2]; //argument 5 : workingHours
                    for(int i = 0; i < 7; i++)
                    {
                        for(int j = 0; j < 2; j++)
                        {
                            workingHours[i][j].wh = decodedFrame.args[4][2*i + j] - 1;  //variable talbeauDeWorkingHours.workingHour
                        }
                    }

                    employeeManager_addEmployee(arg1, arg2, picture, role, workingHours);
                    free(picture.filePath);
                    break;
                case AOP_CMD_DELETE_EMPLOYEE:
                    TRACE("deleteEmployee\n");
                    EmployeeID employeeID;                 
                    employeeID.ID = atoi(decodedFrame.args[0]); //argument 1 : employeeID

                    employeeManager_deleteEmployee(employeeID);
                    break;
                case AOP_CMD_ASK_EMPLOYEE_LIST:
                    TRACE("askEmployeeList\n");

                    employeeManager_askEmployeeList();
                    break;
                default:
                    TRACE("Cmd_Id n°%d non traité pour l'instant\n", decodedHeader.cmdId);
                    break;
            }
            //libération des mémoires allouées aux arguments et à la trame
            if(arg1 != NULL) free(arg1);
            if(arg2 != NULL) free(arg2);
            if(arg3 != NULL) free(arg3);
            if(arg4 != NULL) free(arg4);
            if(arg5 != NULL) free(arg5);

            if(frame != NULL) free(frame); //libération de la mémoire allouée aux chaînes de caractères contenant la trame et le header
        }
        free(header);
    }
    TRACE("end dispatcher loop\n");

    return NULL;
}

void dispatcherAOP_setConnected(bool state)
{
    if(state)
    {
        aop_connected = true;
        sem_post(&connection_sem);
        TRACE("sem_post\n");
    }
    else
    {
        aop_connected = false;
    }
}
