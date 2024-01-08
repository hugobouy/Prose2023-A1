/**
 * @file 	main.c
 * @brief 	Fichier de démarrage de SoftSonnette
 * @author 	CHIRON Paul
 * @date 	15/05/2023
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include <gst/gst.h>
#include <gtk/gtk.h>

#include "common.h"
#include "postmanAOP.h"
#include "postmanSP.h"
#include "cameraman.h"
#include "uiss.h"
#include "bouncer.h"

/*
 * DEFINES
 */


/*
 * TYPEDEF
 */

/*
 * LOCAL FUNCTIONS
 */

static void cleanup();

/*
 * LOCAL VARIABLES
 */

pthread_barrier_t start_barrier; //barrière de démarrage de l'application. Synchronise les appels aux starts des objets
pthread_barrier_t stop_barrier; //barrière d'arrêt de l'application. Synchronise les appels aux stop des objets
pthread_barrier_t quit_barrier;
pthread_mutex_t employee_mutex;

/******************************************************************************/

/**
 * @brief Fonction starter de l'application SoftSonnette
 * 
 * @return int 
 */
int main(){

    //Initialisation des barrières
    pthread_barrier_init(&start_barrier, NULL, 4);
    pthread_barrier_init(&stop_barrier, NULL, 5);
    pthread_barrier_init(&quit_barrier, NULL, 3);
    pthread_mutex_init(&employee_mutex, NULL);

    //Initialisation de GStreameur
    gst_init(NULL, NULL);

    //Initialisation de l'objet Cameraman
    cameraman_new();

    //Initialisation de l'objet Postman
    postmanAOP_new();

    postmanSP_new();

    //Initialisation de l'objet Bouncer
    bouncer_new();

    //Initialisation et démarrage de l'objet UISS
    uiss_launchSS();

    //Démarrage des différents objets
    cameraman_start();
    postmanAOP_start();
    postmanSP_start();
    bouncer_start();

    //Attente de l'appui sur le bouton "Quit" d'UISS et de l'arrêt d'UISS
    pthread_barrier_wait(&quit_barrier);
    cameraman_stop();
    bouncer_stop();
    //Arrêt des autres objets
    postmanAOP_stop();
    postmanSP_stop();
    

    //Synchronisation de l'arrêt des objets avant leur destruction
    TRACE("wait for stop\n");
    pthread_barrier_wait(&stop_barrier);
    TRACE("all stop barrier passed\n");
    
    cleanup();

    exit(EXIT_SUCCESS);
}

/**
 * @brief Fonction de nettoyage et de libération des mémoires allouées aux différents objets
 * 
 */
static void cleanup()
{
    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&stop_barrier);
    pthread_barrier_destroy(&quit_barrier);
    pthread_mutex_destroy(&employee_mutex);
    uiss_free();
    TRACE("UISS free\n");
    postmanAOP_free();
    TRACE("postAOP free\n");
    postmanSP_free();
    TRACE("postSP free\n");
    cameraman_free();
    TRACE("cam free\n");
    bouncer_free();
    TRACE("bouncer free\n");
    TRACE("end of cleaning \n");
}
