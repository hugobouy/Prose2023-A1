/**
 * @file         bouncer.c
 * @brief        Module used to check if an employee is recognized or not, and if he is allowed to come in.
 * @author         JURET Paul
 * @date         11/05/2023
 *
 * \b License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

#include "bouncer.h"
#include "recognitionAI.h"
#include "cameraman.h"
#include "employeeManager.h"
#include "proxyDoorManager.h"
#include "proxyUISP.h"     
#include "clock.h"

//#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>
//#include <stdlib.h>

#define EMPLOYEE_NUMBER 10
#define RESULT_AI 256

#define NAME_MQ_BOX_BOUNCER  "/bouncer_mq" 
#define MQ_MSG_COUNT 10 // Nbre de message dans mq



typedef enum  {S_FORGET= 0, S_CHECK_ALLOW, S_WAIT_RECOGNITION, S_IDLE, S_CHOICE, S_DEATH, STATE_NB} State;                                            // State
typedef enum  {E_ASKFACERECO = 0, E_RECOGNIZED_FACE, E_RECOGNIZED_TRUE, E_RECOGNIZED_FALSE , E_ASK_OPEN_DOOR,  E_NOT_ALLOWED, E_STOP, EVENT_NB} Event;                                   // Déclencheur
typedef enum  {A_NOP = 0, A_ASKFACERECO, A_RECOGNIZED_TRUE, A_RECOGNIZE_FACE, A_CHECK_ALLOW, A_RECOGNIZED_FALSE, A_ASK_OPEN_DOOR, A_SIGNAL_NOT_ALLOWED, A_SIGNAL_FACE_NOT_ALLOWED, A_STOP} Action ;   // Effects


typedef struct
{
	State destinationState;
	Action action;
} Transition;

typedef struct
{
    Event event;
} MqMsgData;

typedef union //send or receive messages into the mq without converting into Event
{
    MqMsgData data;
    char buffer[sizeof(MqMsgData)];
} MqMsg;


static Picture picture;                                  //The picture of the person we want to recognize
static Employee_t employeeList[EMPLOYEE_NUMBER] = {};      //The list of person we can recognize
static Employee_t *employeeListPtr;
static EmployeeID recognizedEmployee;                    //If the person in front of the camera is recognized or not
static Time currentTime;                                 //Real time
static mqd_t mq_bouncer;                                 //message queue of bouncer
static State bouncerState;
static pthread_t bouncer;                                     //thread of bouncer








/**
 * @brief vérifie si l'employé reconnu est autorisé à entrer selon ses heures de travail
 * 
 * @param time 
 * @param employeeID 
 * @return true 
 * @return false 
 */
static bool checkEmployeeAllow(Time time, EmployeeID employeeID);


/**
 * @brief fonction du thread de bouncer
 * 
 * @param args 
 * @return void* 
 */
static void * bouncerFct(void * args);   



/**
 * @brief recevoir des messages depuis la message queue
 * 
 * @param aMsg 
 */
static void bouncer_mqReceive(MqMsg * aMsg);



/**
 * @brief envoyer des messages dans la message queue
 * 
 * @param aMsg 
 */
static void bouncer_mqSend(MqMsg * aMsg);


/**
 * @brief 
 * 
 * @param action 
 * @param msg 
 */
static void bouncer_performAction(Action action, MqMsg * msg);


/**
 * @brief exécute la séquence de reconnaissance faciale
 * 
 */
static void bouncer_FaceRecognition(void);

/**
 * @brief envoie le message de changement d'état dans la mq
 * 
 */
static void bouncer_setRecoFace(void);

static void bouncer_sayRecognize(void);

static void bouncer_sayNotRecognize(void);
/**
 * @brief envoie le message de changement d'état dans la mq
 * 
 */
static void bouncer_ask4Door(void);

/**
 * @brief envoie le message de changement d'état dans la mq
 * 
 */
static void bouncer_refuseEntry(void);
                        
static Transition mySm [STATE_NB-1][EVENT_NB] =
  {
    [S_IDLE][E_ASKFACERECO]={S_WAIT_RECOGNITION,A_ASKFACERECO},
    [S_WAIT_RECOGNITION][E_RECOGNIZED_FACE] ={S_CHOICE, A_RECOGNIZE_FACE},
    [S_CHOICE][E_RECOGNIZED_TRUE] ={S_CHECK_ALLOW, A_CHECK_ALLOW },
    [S_CHOICE][E_RECOGNIZED_FALSE]={S_IDLE, A_SIGNAL_FACE_NOT_ALLOWED},
    [S_CHECK_ALLOW][E_ASK_OPEN_DOOR]={S_IDLE, A_ASK_OPEN_DOOR},
    [S_CHECK_ALLOW][E_NOT_ALLOWED]={S_IDLE, A_SIGNAL_NOT_ALLOWED},
    [S_IDLE][E_STOP]= {S_DEATH, A_STOP},
    [S_WAIT_RECOGNITION][E_STOP]= {S_DEATH, A_STOP},
    [S_CHOICE][E_STOP]= {S_DEATH, A_STOP},
    [S_CHECK_ALLOW][E_STOP]= {S_DEATH, A_STOP},
  };
         

/**
 * @brief Check if the recognized person is allowed to come in (checking his workTime)
 * 
 * @param time 
 * @param employeeID 
 * @return true 
 * @return false 
 */
bool checkEmployeeAllow(Time currentTime, EmployeeID employeeID)
{
    //WAITING FOR THE FACE RECOGNITION PROGRAemployeeListM
Hour *employeeStart = &employeeList[employeeID.ID-1].workingHours[currentTime.dayOfWeek][0];        //heure de début de l'employé
Hour *employeeStop  = &employeeList[employeeID.ID-1].workingHours[currentTime.dayOfWeek][1];        //heure de fin de l'employé

Hour demieHeure; demieHeure.wh = currentTime.hour*2 + (currentTime.minute >=30 ? 1:0);      //conversion du temps réels en demies heures


if(demieHeure.wh >= employeeStart->wh) //&& (demieHeure.wh >= employeeStart->minute) && (demieHeure.wh >= employeeStart->second))
{
    if(demieHeure.wh <= employeeStop->wh) //&& (currentTime.minute <= employeeStop->minute) && (currentTime.second <= employeeStop->second))
    {
        TRACE("entré\n");
        return true;            // l'heure actuelle est supérieure à l'heure de début et antérieure à l'heure de fin
    }
}
return false; //l'heure actuelle n'est pas dans le cadre des heures de travail, l'employé n'est pas autorisé à entrer.

}

void bouncer_setRecognizeFace(EmployeeID employeeID)
{
    char answer[RESULT_AI];
    printf("Veuillez décliner votre identité \n");      // cette partie sert à simuler le retour fait par l'ia.
    scanf("%s",answer);                                 // Ceci n'est qu'une simulation.
    printf("Identité renseignée \n");            //read the last line on the terminal to get the ID of the employee
    if (strcmp(answer, "UNKNOWN") == 0)
    {
        TRACE("No matching picture");
        bouncer_sayNotRecognize();
    }
    else
    {
        for(int count =0; count < EMPLOYEE_NUMBER; count++)
        {
            if (strcmp(answer, employeeList[count].name)==0)
            {
                count+=1;
                employeeID.ID = count;                      //  store the id of the employee
                recognizedEmployee.ID=count;
                printf("%d\n",employeeID.ID);
                currentTime= clock_getCurrentTime();        //  get the actual Time to check if the employee is on his working hours
                bouncer_sayRecognize();
                break;

            }
        }
    }
}

void bouncer_FaceRecognition(void)
{
    cameraman_suspendStreaming();    
    picture = cameraman_takePicture();                                                        
    employeeListPtr = employeeManager_getEmployeeList();

    // Copier les éléments de la liste pointée par employeeListPtr dans employeeList
    for (int i = 0; i < MAX_EMPLOYEE; i++) 
    {
        employeeList[i] = employeeListPtr[i];
    }
    recognitionAI_launchAI(picture, employeeList);                                          //always recognized without AI. 
    TRACE("recognized id : %d\n", recognizedEmployee.ID);

    cameraman_resumeStreaming();   

    bouncer_setRecoFace();                                       
}

void bouncer_new(void)
{
    int check;
    struct mq_attr mq = 
    {
    .mq_maxmsg = MQ_MSG_COUNT,
    .mq_msgsize = sizeof(MqMsg),
    .mq_flags = 0,
    };

    // Destruction de la mq 
    check = mq_unlink(NAME_MQ_BOX_BOUNCER);
    if (check == -1 && errno != ENOENT) 
    {
        perror("Une erreur s'est produite lors de l'opération");
    }   
    // création de la mq 
    mq_bouncer = mq_open(NAME_MQ_BOX_BOUNCER, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mq);
    if(mq_bouncer == -1)
    {
        perror("Une erreur s'est produite lors de l'opération");
    }

}
void bouncer_start(void)
{
    int check;
    check = pthread_create(&bouncer, NULL, bouncerFct, NULL);
    if (check != 0)
    {
        perror("echec de la construction du thread");
        fflush(stderr);

    }

}
void bouncer_stop(void)
{
    MqMsg msg = {.data.event = E_STOP};
    bouncer_mqSend(&msg);
}

void bouncer_free(void)
{
    int check;
    // fermeture de la mq 
    check = mq_close(mq_bouncer);
    if(check == -1)
    {
        perror("Une erreur s'est produite lors de l'opération");
    }
    // destruction de la mq 
    check = mq_unlink(NAME_MQ_BOX_BOUNCER);
    if(check == -1)
    {
        perror("Une erreur s'est produite lors de l'opération");
    }

}

/**
 * @brief receive some message from the message queue
 * 
 * @param aMsg 
 */
static void bouncer_mqReceive(MqMsg * aMsg)
{
    int check;
    check = mq_receive(mq_bouncer, aMsg->buffer, sizeof(MqMsg), NULL);
    if (check != sizeof(MqMsg))
    {
        perror("taille du message reçu incorrect");
        fflush(stderr);
    }

}

/**
 * @brief send some messages into the message queue
 * 
 * @param aMsg 
 */
static void bouncer_mqSend(MqMsg * aMsg)
{
    int check;
    check = mq_send(mq_bouncer, aMsg->buffer, sizeof(MqMsg), 0);
    if (check != 0)
    {
        perror("erreur dans l'envoie de msg");
        fflush(stderr);
    }

}

/**
 * @brief will launch askFaceRecognition in a thread to save ressources
 * 
 * @param arg
 * @return void*
 */
void * bouncerFct(void * arg)
{
    //bouncer_askFaceRecognition();
    MqMsg msg ;
    bouncerState = S_IDLE;
    Transition * myTrans;
    pthread_barrier_wait(&start_barrier);
    while (bouncerState != S_DEATH)
    {
        bouncer_mqReceive(&msg);
        myTrans = &mySm[bouncerState][msg.data.event];
         if (myTrans->destinationState != S_FORGET)
        {
          bouncer_performAction(myTrans->action, &msg);
          bouncerState = myTrans->destinationState;
        }
    }
    pthread_barrier_wait(&stop_barrier);
    return NULL;
}


void bouncer_performAction(Action action, MqMsg * msg)
{
    switch(action)
    {
        case A_NOP:
            break;
        case A_ASKFACERECO:                         
            bouncer_FaceRecognition();                                      // lance la procédure de reconnaissance faciale
            break;
        case A_RECOGNIZE_FACE:                                                                    
            bouncer_setRecognizeFace(recognizedEmployee);                   // transforme le nom de l'employé reconnu en son ID
            break;
        case A_ASK_OPEN_DOOR:
            proxyDoorManager_askOpenDoor();                                 // Demande l'ouverture de la porte pour l'employé
            TRACE("demande d'ouverture de la porte");
            break;
        case A_CHECK_ALLOW:
            if(checkEmployeeAllow(currentTime, recognizedEmployee))         // regarde si l'employé se présente dans ses heures de travail
            {
                bouncer_ask4Door(); //demande l'ouverture de la porte 
            }
            else
            {
                bouncer_refuseEntry(); //signal le refus d'ouverture de la porte
            }
            break;
        case A_SIGNAL_NOT_ALLOWED:
            proxyUISP_signalNotAllowed();                                   // l'employé est reconnu mais pas dans ses heures de travail
            break; 
        case A_SIGNAL_FACE_NOT_ALLOWED:
            proxyUISP_signalNotAllowed();                               // la personne n'est pas reconnue
            break;  
        case A_STOP:
            break;
  
        default :
            break;
    }
}

void bouncer_askFaceRecognition(void)           //envoie dans A_ASKFACERECO
{
    MqMsg msg = {.data.event = E_ASKFACERECO};
    bouncer_mqSend(&msg);
}

void bouncer_setRecoFace(void)      //envoie dans A_CHECK_ALLOW ou //A_SIGNAL_FACE_NOT_ALLOW
{
        MqMsg msg = {.data.event = E_RECOGNIZED_FACE};
        bouncer_mqSend(&msg);
}

void bouncer_sayRecognize()
    {
        MqMsg msg = {.data.event = E_RECOGNIZED_TRUE};
        bouncer_mqSend(&msg);
    }


void bouncer_sayNotRecognize()
    {
        MqMsg msg = {.data.event = E_RECOGNIZED_FALSE};
        bouncer_mqSend(&msg);
    }


void bouncer_ask4Door(void)
{
    MqMsg msg = {.data.event = E_ASK_OPEN_DOOR};
    bouncer_mqSend(&msg);
}

void bouncer_refuseEntry(void)
{
    MqMsg msg = {.data.event = E_NOT_ALLOWED};
    bouncer_mqSend(&msg);
}

