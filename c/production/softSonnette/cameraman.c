/**
 * @file 	cameraman.c
 * @brief 	code de la classe cameraman
 * @author 	LETENNEUR Laurent avec l'aide du code de Jérome Delatour
 * @date 	10/05/2023
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
 * DEFINE
 */


/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
//#include <gst/gst.h>
#include <string.h>
//#include <glib.h> 
//#include <gtk/gtk.h>
#include <time.h>

#include <stddef.h> //  Macros, type dont NULL et size_t
#include <unistd.h> // Macros, type Posix and co
#include <mqueue.h>
#include <sys/stat.h> // pour constantes de mode_t
#include <errno.h>

#include "streamer.h"
#include "cameraman.h"
#include "uiss.h"
//#include "common.h"

#define NAME_MQ_BOX_CAMERAMAN  "/cameraman_mq" 
#define MQ_MSG_COUNT 10 // Nbre de message dans mq


static bool cameraAlive;
static Picture currentVideoPicture;
static bool alreadylive;

pthread_t cameraman_video;

typedef enum  {S_FORGET = 0, S_IDLE, S_SUSPENDED, S_SCREEN_ONLY, S_SCREEN_AND_AOP, S_CHOIX, S_DEATH, STATE_NB} State;  //State
typedef enum  {E_VIDE = 0, E_ENABLE, E_NOT_ENABLE, E_SUSPENDED, E_RESUME, E_STARTSTREAMING, E_STOPSTREAMING, EVENT_NB} Event; // Déclencheur
typedef enum  {A_NOP = 0, A_STREAM_SCREEN_ONLY, A_STREAM_SCREEN_AND_AOP, A_SUSPENDED, A_CHOIX, A_STOP} Action ; // effets

typedef struct
{
	State destinationState;
	Action action;
} Transition;

typedef struct
{
	Event event;
} MqMsgData;

typedef union
{
	MqMsgData data;
  char buffer[sizeof(MqMsgData)];
} MqMsg;

/**
 * @brief 
 * Méthode permettant de vérifier la présence d'une caméra 
 * path : chemin ou se trouve le fichier permettant de savoir si la caméra est présente
 * si il est présent la caméra vie sinon elle est morte.
 * present : variable qui enregistre si la caméra est vivante ou morte
 */
static void cameraman_checkCameraConnected(void);

static 	mqd_t mq_cameraman;
static void cameraman_mqReceive(MqMsg * aMsg);
static void cameraman_mqSend(MqMsg * aMsg);
static void cameraman_performAction(Action anAction, MqMsg * aMsg);
static void * cameraman_run(void * arg);

static Transition mySm [STATE_NB-1][EVENT_NB] =
  {
    [S_IDLE][E_STARTSTREAMING]= {S_CHOIX, A_CHOIX},
    [S_CHOIX][E_STARTSTREAMING]= {S_SCREEN_ONLY, A_STREAM_SCREEN_ONLY},
    [S_CHOIX][E_STOPSTREAMING]= {S_DEATH, A_STOP},
    [S_SCREEN_ONLY][E_ENABLE]= {S_SCREEN_AND_AOP, A_STREAM_SCREEN_AND_AOP},
    [S_SCREEN_AND_AOP][E_NOT_ENABLE]={S_SCREEN_ONLY, A_STREAM_SCREEN_ONLY},
    [S_SCREEN_ONLY][E_SUSPENDED]= {S_SUSPENDED, A_SUSPENDED},
    [S_SCREEN_AND_AOP][E_SUSPENDED]= {S_SUSPENDED, A_SUSPENDED},
    [S_SUSPENDED][E_RESUME]= {S_SCREEN_ONLY, A_STREAM_SCREEN_ONLY},
    [S_SUSPENDED][E_STOPSTREAMING]= {S_DEATH, A_STOP},
    [S_SCREEN_AND_AOP][E_STOPSTREAMING]= {S_DEATH, A_STOP},
    [S_SCREEN_ONLY][E_STOPSTREAMING]= {S_DEATH, A_STOP},

  };

void cameraman_new()
{
  int check;
  struct mq_attr mqa = 
  {
  .mq_maxmsg = MQ_MSG_COUNT,
  .mq_msgsize = sizeof(MqMsg),
  .mq_flags = 0,
  };

  // Destruction de la mq 
  check = mq_unlink(NAME_MQ_BOX_CAMERAMAN);
  if ((check == -1) && (errno != ENOENT))
  {
    perror("Erreur de destruction de mq");
  }
  // création de la mq 
  mq_cameraman = mq_open(NAME_MQ_BOX_CAMERAMAN, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mqa);
  if (mq_cameraman == -1)
  {
    perror("Erreur creation de mq");
  }
}

void cameraman_start()
{   
  int check;
  check = pthread_create(&cameraman_video, NULL, cameraman_run, NULL);
  if (check != 0)
  {
    perror("thread pas constructible");
    fflush(stderr);
  }
}

void cameraman_stop()
{
  cameraman_stopStreaming();
}

void cameraman_free()
{
  int check;
  // fermeture de la mq 
  check = mq_close(mq_cameraman);
  if (check == -1)
  {
    perror("mq non fermable");
  }
  // destruction de la mq 
  check = mq_unlink(NAME_MQ_BOX_CAMERAMAN);
  if (check == -1)
  {
    perror("mq non destructible");
  }
}

static void cameraman_mqReceive(MqMsg * aMsg)
{
	int check;
	check = mq_receive(mq_cameraman, aMsg->buffer, sizeof(MqMsg), NULL);
  if (check != sizeof(MqMsg))
  {
    perror("taille du message reçu incorrect");
    fflush(stderr);
  }
}

static void cameraman_mqSend(MqMsg * aMsg)
{
	int check;
  check = mq_send(mq_cameraman, aMsg->buffer, sizeof(MqMsg), 0);
  if (check != 0)
  {
    perror("erreur dans l'envoie de msg");
    fflush(stderr);
  }
}

static void cameraman_performAction(Action anAction, MqMsg * aMsg)
{

  /* Initialize our data structure */
	switch (anAction)
    {
    case A_NOP:
      break;
    case A_STREAM_SCREEN_ONLY:
    if (alreadylive == TRUE)
    {
       streamer_stopPipeline();
       streamer_cleanPipeline();
    }
      streamer_streamEcran();
      alreadylive = TRUE;
      break;
    case A_STREAM_SCREEN_AND_AOP:
      if (alreadylive == TRUE)
      {
        streamer_stopPipeline();
        streamer_cleanPipeline();
      }
      streamer_streamTel();
      break;
    case A_SUSPENDED:
      TRACE("supendu \n");
      streamer_stopPipeline();
      streamer_cleanPipeline();
      alreadylive = FALSE;
      break;
    case A_CHOIX:
      cameraman_checkCameraConnected();
      if (cameraAlive == true)
      {
        cameraman_startStreaming();
      }
      else 
      {
        cameraman_stopStreaming();
      }
      break;
    case A_STOP:
      if (inforstream.pipeline == NULL)
      {
        TRACE("pipeline = NULL lors de l'arret de la MaE \n");
      }
      else 
      {
        streamer_stopPipeline();
        streamer_cleanPipeline();
      }
      alreadylive = FALSE;
      break;
    default:
      break;
    }
}

static void * cameraman_run(void * arg)
{
	MqMsg msg ; 
  State myState = S_IDLE;
  Transition * myTrans;
  pthread_barrier_wait(&start_barrier);
	while (myState != S_DEATH)
    {
      cameraman_mqReceive(&msg);
      myTrans = &mySm[myState][msg.data.event];
      if (myTrans->destinationState != S_FORGET)
        {
          cameraman_performAction(myTrans->action, &msg);
          myState = myTrans->destinationState;
        }
    }

  TRACE("cameraman stopped, wait end of thread\n");

  pthread_barrier_wait(&stop_barrier);
  TRACE("end of thread cameraman\n");
  return NULL;
}


static void cameraman_checkCameraConnected(void)
{
  char path[] = "/sys/class/video4linux/video0"; 
  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    TRACE("La caméra n'est pas branchée \n");
    uiss_updateCamState(false);
    cameraAlive = 0;
    //exit(EXIT_SUCCESS); //a tester 
  } 
  else 
  {
    TRACE("La caméra est branchée\n");
    uiss_updateCamState(true);
    cameraAlive = 1;
    fclose(fp);
   }
}

Picture cameraman_takePicture(void)
{

  {

    GstElement *pipeline;
    GError *error = NULL;
    time_t chrono_picture = time(NULL);

    // Création du pipeline à partir de la description préexistant
    pipeline = gst_parse_launch("v4l2src device=/dev/video0 ! video/x-raw,width=320,height=240 ! videoconvert ! jpegenc ! filesink location=/home/weston/image.jpg", &error);

    // Démarrer le pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    TRACE("prise photo \n");

    // Attendre la fin de la capture de la photo (ou l'erreur)
    //sleep(1); // solution de secoure
    
  while (time(NULL) - chrono_picture < 1)
  {
    
  }

       
    // Nettoyer
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    TRACE("extinction photo \n");

    currentVideoPicture.filePath = "/home/weston/image.jpg";
    return currentVideoPicture;
  }
}


void cameraman_subscribeToVideoStream(bool enable) 
{
    if (enable == 1)
    {
      MqMsg msg = {.data.event = E_ENABLE};
      cameraman_mqSend(&msg);
    } 
    else
    {
      MqMsg msg = {.data.event = E_NOT_ENABLE};
      cameraman_mqSend(&msg);
    }

}

void cameraman_startStreaming(void)
{
  MqMsg msg = {.data.event = E_STARTSTREAMING};
  cameraman_mqSend(&msg);
}

void cameraman_stopStreaming(void)
{
  MqMsg msg = {.data.event = E_STOPSTREAMING};
  cameraman_mqSend(&msg);
}

void cameraman_suspendStreaming(void)
{
  MqMsg msg = {.data.event = E_SUSPENDED};
  cameraman_mqSend(&msg);
}

void cameraman_resumeStreaming(void)
{
  MqMsg msg = {.data.event = E_RESUME};
  cameraman_mqSend(&msg);
}


