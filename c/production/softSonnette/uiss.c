/**
 * @file 	uiss.c
 * @brief 	Classe pour UISS
 * @author 	CHIRON Paul
 * @date 	11/05/2023
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <sys/wait.h>

#include <gdk/gdkwayland.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include "cameraman.h"
#include "proxyUISP.h"
#include "bouncer.h"
#include "streamer.h"
#include "employeeManager.h"
#include "common.h"
#include "uiss.h"


/*
 * DEFINES
 */

#define WIDTH 700 //Dimensions de la fenêtre GTK
#define HEIGHT 500

#define NAME_MQ_BOX_UISS  "/mq_uiss" //Boîte aux lettres liée à UISS
#define MQ_MSG_COUNT 10

typedef enum  {S_FORGET = 0, S_INIT, S_IDLE, S_CHOICE, S_ERROR_COM, S_RUNNING, S_ERROR_CAM_NOT_CONNECTED, S_DEATH, STATE_NB} State; //Etats d'UISS
typedef enum  {E_ASK_START_COM = 0, E_TIMEOUT_TASS, E_UPDATE_CAM, E_UPDATE_CAM_TRUE, E_UPDATE_CAM_FALSE, E_UPDATE_DOOR, E_RING, E_STOP, EVENT_NB} Event; //Evenements d'UISS : demandes d'affichage de l'état de la caméra, état de la porte et visage non reconnu
typedef enum  {A_NOP = 0, A_ACK_START_COM, A_ERROR_COM, A_UPDATE_CAM, A_UPDATE_CAM_TRUE, A_UPDATE_CAM_FALSE, A_UPDATE_DOOR, A_RING, A_STOP} Action ; //Actions réalisées par UISS

typedef enum {
    STATE_IDLE = 0,
    STATE_WEBCAM_CONNECTED ,
    STATE_WEBCAM_NOT_CONNECTED,
    STATE_ERROR_COM
} SSState; //Etats de appState

/*
 * TYPEDEF
 */

/**
 * @brief Structure des transitions liées à UISS. Etat de destination et action à réaliser.
 * 
 */
typedef struct
{
	State destinationState;
	Action action;
} Transition;

/**
 * @brief Structure des données d'un message de la boîte aux lettres. Evènement à traiter et éventuelle donnée, ici un état de WebCam/porte.
 * 
 */
typedef struct
{
	Event event;
    bool state;
} MqMsgData;

/**
 * @brief Structure d'un message de la boîte aux lettres. Donnée définie dans la structure MqMsgData et son buffer associé.
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

static void uiss_display(SSState state);

static void uiss_on_quit_button_clicked(GtkWidget *widget, gpointer data);

static void uiss_on_ring_button_clicked(GtkWidget *widget, gpointer data);

static void uiss_mqReceive(MqMsg * aMsg);

static void uiss_mqSend(MqMsg * aMsg);

static void uiss_performAction(Action anAction, MqMsg * aMsg);

static void * uiss_run(void * aParam);

static void * uiss_gtkMain(void *);

static void load_css();

static void uiss_refresh_door_state(bool state);

static void uiss_timer_launch();

static void uiss_cancel_timer();

static void uiss_timeout_tass(union sigval val);

static void uiss_evaluateState(bool state);

static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data); 
/*
 * LOCAL VARIABLES
 */

static pthread_t uiss_thread; //Thread associé à UISS
static pthread_t uiss_gtk_thread; //Thread associé à GTK
static mqd_t uiss_mq; //Boîte aux lettres d'UISS
static timer_t uiss_timer;

static Transition mySm [STATE_NB-1][EVENT_NB] = //Transitions état-action selon l'état courant et l'évènement reçu
{
    [S_INIT][E_ASK_START_COM] = {S_IDLE, A_ACK_START_COM},
    [S_INIT][E_TIMEOUT_TASS]= {S_ERROR_COM, A_ERROR_COM},
    [S_IDLE][E_UPDATE_CAM] = {S_CHOICE, A_UPDATE_CAM},
    [S_CHOICE][E_UPDATE_CAM_TRUE] = {S_RUNNING, A_UPDATE_CAM_TRUE},
    [S_CHOICE][E_UPDATE_CAM_FALSE] = {S_ERROR_CAM_NOT_CONNECTED, A_UPDATE_CAM_FALSE},
    [S_RUNNING][E_RING] = {S_RUNNING, A_RING},
    [S_RUNNING][E_UPDATE_DOOR] = {S_RUNNING, A_UPDATE_DOOR},
    [S_RUNNING][E_STOP] = {S_DEATH, A_STOP},
    [S_IDLE][E_STOP] = {S_DEATH, A_STOP},
    [S_INIT][E_STOP] = {S_DEATH, A_STOP},
    [S_ERROR_COM][E_STOP] = {S_DEATH, A_STOP},
    [S_ERROR_CAM_NOT_CONNECTED][E_STOP] = {S_DEATH, A_STOP}
};

static GtkWidget *window_widget;
static GtkWindow *window; //fenêtre principale d'UISS
static GtkWidget *quit_button; //bouton quitter
static GtkWidget *ring_button; //bouton sonner
static GtkWidget *box; //boîte d'agencement des différents éléments sur la fenêtre
static GtkColorButton *color_button;
static GtkWidget *label_door;
static GtkWidget *label_app_state;
static GdkRGBA color;

static SSState appState = STATE_IDLE;

/******************************************************************************/

uint8_t uiss_new(void)
{
    int check;
    struct mq_attr mqa = {
        .mq_maxmsg = MQ_MSG_COUNT,
        .mq_msgsize = sizeof(MqMsg),
        .mq_flags = 0,
    }; //Attributs de la bal
    
    check = mq_unlink(NAME_MQ_BOX_UISS);
    if((check == -1) && (errno =! ENOENT))
    {
        TRACE("mq_unlink UISS error\n");
    }
    uiss_mq = mq_open(NAME_MQ_BOX_UISS, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mqa); //ouverture de la bal
    if(uiss_mq == -1)
    {
		perror("mq_open UISS error\n");
		return 1;
    }

	// Création du timer.
	struct sigevent event;

	event.sigev_notify = SIGEV_THREAD;
	event.sigev_value.sival_ptr = NULL;
	event.sigev_notify_function = uiss_timeout_tass;
	event.sigev_notify_attributes = NULL;

	if (timer_create(CLOCK_REALTIME, &event, &uiss_timer) != 0) {
		perror("timer_create");
        return 1;
	}
    return 0;
}

uint8_t uiss_start(void)
{
    if(pthread_create(&uiss_thread, NULL, uiss_run, NULL) != 0) //création du thread run d'UISS
    {
        fprintf(stderr, "pthread_create UISS error\n");
        fflush(stderr);
        return 1;
    }
    if(pthread_create(&uiss_gtk_thread, NULL, uiss_gtkMain, NULL) != 0) //création du thread main de GTK
    {
        fprintf(stderr, "pthread_create UISS GTK error\n");
        fflush(stderr);
        return 1;
    }
    return 0;
}

void uiss_stop(void)
{
	MqMsg msg = {.data.event = E_STOP}; //envoi de l'évènement STOP via mq
	uiss_mqSend(&msg);
}

uint8_t uiss_free(void)
{
    int check;
    check = mq_close(uiss_mq); //fermeture de la boîte aux lettres
    if(check == -1)
    {
		perror("mq_close UISS error\n");
		return 1;
    }
    check = mq_unlink(NAME_MQ_BOX_UISS); //suppression de la boîte aux lettres
    if(check == -1)
    {
		perror("mq_unlink UISS error\n");
		return 1;
    }
    return 0;
}

void uiss_launchSS(void)
{
    if(uiss_new() != 0) //initialisation et démarrage d'UISS
    {
        perror("uiss_launch error\n");
        exit(EXIT_FAILURE);
    }
    if(uiss_start() != 0)
    {
        perror("uiss_launch error\n");
        exit(EXIT_FAILURE);
    }
    employeeManager_load();
    //proxyUISP_launchSP();
    uiss_timer_launch();
    
    
}

void uiss_quitSS(void)
{
    uiss_stop(); 
}

void uiss_ring(void)
{
	MqMsg msg = {.data.event = E_RING}; //envoi de l'évènement RING via mq
	uiss_mqSend(&msg);
}

void uiss_updateCamState(bool state)
{
    MqMsg msg = {.data.event = E_UPDATE_CAM, .data.state = state}; //envoi de l'évènement E_UPDATE_CAM via mq
    uiss_mqSend(&msg);
}

void uiss_askStartCom()
{
	MqMsg msg = {.data.event = E_ASK_START_COM}; //envoi de l'évènement E_ASK_START_COM via mq
	uiss_mqSend(&msg);
}

void uiss_updateDoorState(bool state)
{
    MqMsg msg = {.data.event = E_UPDATE_DOOR, .data.state = state}; //envoi de l'évènement E_UPDATE_DOOR via mq
	uiss_mqSend(&msg);
}

/**
 * @brief Fonction de timeout du timer TASS pour la communication avec SoftPorte
 * 
 * @param val Le signal déclenché par le timeout
 */
static void uiss_timeout_tass(union sigval val)
{
	MqMsg msg = {.data.event = E_TIMEOUT_TASS}; //envoi de l'évènement E_TIMEOUT_TASS via mq
	uiss_mqSend(&msg);
}

/**
 * @brief Fonction de réception des messages sur la boîte aux lettres
 * 
 * @param aMsg Le message à lire
 */
static void uiss_mqReceive(MqMsg * aMsg)
{
	int check;
	check = mq_receive(uiss_mq, aMsg->buffer, sizeof(MqMsg), NULL); //réception d'un message de la mq
    if (check != sizeof(MqMsg))
    {
        perror("Error receiving message via uiss mqueue");
    }
}

/**
 * @brief Fonction d'envoi des messages sur la boîte aux lettres
 * 
 * @param aMsg Le message à envoyer
 */
static void uiss_mqSend(MqMsg * aMsg)
{
    int check;
    check = mq_send(uiss_mq, aMsg->buffer, sizeof(MqMsg), 0); //envoi d'un message à la mq
    if (check == -1) {
        perror("Error sending message via uiss mqueue");
    }
    else
    {
        TRACE("Message sent via uiss mq\n");
    }
}

/**
 * @brief Fonction de rafraichissement de l'affichage sur l'écran de la Board
 * 
 * @param state L'état courant d'UISS
 */
static void uiss_display(SSState state)
{
    if(G_IS_OBJECT(label_app_state))
    {
        gtk_widget_destroy(label_app_state);
    }
    switch(state)
    {
        case STATE_WEBCAM_CONNECTED:
            label_app_state = gtk_label_new("Webcam connectée");
            break;
        case STATE_WEBCAM_NOT_CONNECTED:
            label_app_state = gtk_label_new("Webcam non connectée");
            break;
        case STATE_ERROR_COM:
            label_app_state = gtk_label_new("Erreur communication avec SoftPorte");
            break;
        case STATE_IDLE:
            break;
    }

    if(state != STATE_IDLE)
    {
        gtk_box_pack_start(GTK_BOX(box), label_app_state, FALSE, FALSE, 0);
        gtk_widget_set_halign(label_app_state, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label_app_state, GTK_ALIGN_CENTER);
    }

    // affichage des éléments graphiques
    gtk_widget_show_all(window_widget);
}

/**
 * @brief Fonction de callback du bouton "Quitter"
 * 
 * @param widget Le widget associé
 * @param data Donnée potentielle passée en paramètre (ici NULL)
 */
static void uiss_on_quit_button_clicked(GtkWidget *widget, gpointer data)
{
    uiss_quitSS(); //demande d'arrêt d'UISS
    gtk_widget_destroy(window_widget); //destruction des widgets
    gtk_main_quit(); //arrêt du thread gtk
    TRACE("gtk stopped\n");
}

/**
 * @brief Fonction de callback du bouton "Sonner"
 * 
 * @param widget Le widget associé
 * @param data Donnée potentielle passée en paramètre (ici NULL)
 */
static void uiss_on_ring_button_clicked(GtkWidget *widget, gpointer data)
{
    uiss_ring();
}

/**
 * @brief Fonction de traitement des actions liées à UISS
 * 
 * @param anAction L'action courante à traiter
 * @param aMsg Le message associé
 */
static void uiss_performAction(Action anAction, MqMsg * aMsg)
{
    switch (anAction)
    {
        case A_NOP: 
            break;
        case A_ACK_START_COM:
            proxyUISP_ackStartCom(); 
            uiss_cancel_timer();
            cameraman_startStreaming(); //UISS demande le flux vidéo à Cameraman
            break;
        case A_ERROR_COM:
            appState = STATE_ERROR_COM;
            uiss_display(appState);
            break;
        case A_UPDATE_CAM:
            uiss_evaluateState(aMsg->data.state);
            break;
        case A_UPDATE_CAM_TRUE: //affichage du message webcam connected sur UISS
            //g_print("WebCam connected\n");
            appState = STATE_WEBCAM_CONNECTED;
            uiss_display(appState);
            break;
        case A_UPDATE_CAM_FALSE: //affichage du message webcam not connected sur UISS
            //g_print("WebCam not connected\n");
            appState = STATE_WEBCAM_NOT_CONNECTED;
            uiss_display(appState);
            break;
        case A_RING: //demande de reconnaissance faciale à bouncer suite à la demande d'entrer
            //g_print("Sonner\n");
            bouncer_askFaceRecognition();
            break;
        case A_UPDATE_DOOR: //affichage du message door opended ou closed sur UISS
            uiss_refresh_door_state(aMsg->data.state);
            break;
        case A_STOP: //signale au thread principal l'arrêt d'UISS
            cameraman_stopStreaming();
            employeeManager_save();
            proxyUISP_quitSP();
            pthread_barrier_wait(&quit_barrier);
            break;
        default:
            TRACE("Action inconnue\n");
            break;
    }
}

static void uiss_evaluateState(bool state)
{
    if(state)
    {
        MqMsg msg = {.data.event = E_UPDATE_CAM_TRUE}; //envoi de l'évènement E_UPDATE_CAM_TRUE via mq
	    uiss_mqSend(&msg);
    }
    else
    {
        MqMsg msg = {.data.event = E_UPDATE_CAM_FALSE}; //envoi de l'évènement E_UPDATE_CAM_FALSE via mq
	    uiss_mqSend(&msg);
    }
}

/**
 * @brief Fonction de mise à jour de l'état de la porte sur l'écran d'UISS
 * 
 * @param state L'état courant de la porte
 */
static void uiss_refresh_door_state(bool state)
{
    gtk_widget_destroy(label_door);
    if(state == true)
    {
        //g_print("Door opened\n");
        label_door = gtk_label_new("Porte ouverte");
        color.red = 0.0;
        color.green = 1.0;
    }
    else
    {
        //g_print("Door closed\n");
        label_door = gtk_label_new("Porte fermée");
        color.red = 1.0;
        color.green = 0.0;
    }
    gtk_box_pack_end(GTK_BOX(box), label_door, FALSE, FALSE, 0);
    gtk_widget_set_halign(label_door, GTK_ALIGN_START);
    gtk_widget_set_valign(label_door, GTK_ALIGN_START);
    gtk_color_button_set_rgba(color_button, &color);

    uiss_display(appState);
}

/**
 * @brief Fonction d'initialisation du timer TASS
 * 
 */
static void uiss_timer_launch()
{
	struct itimerspec itimer;

	itimer.it_interval.tv_sec = 0;
	itimer.it_interval.tv_nsec = 0;
	itimer.it_value.tv_sec = TASS;
	itimer.it_value.tv_nsec = 0;

	if (timer_settime(uiss_timer, 0, &itimer, NULL) != 0) {
		perror("timer_settime launch");
	}
}

/**
 * @brief Fonction d'annulation du timer TASS
 * 
 */
static void uiss_cancel_timer()
{
	struct itimerspec itimer;

	itimer.it_interval.tv_sec = 0;
	itimer.it_interval.tv_nsec = 0;
	itimer.it_value.tv_sec = 0;
	itimer.it_value.tv_nsec = 0;

	if (timer_settime(uiss_timer, 0, &itimer, NULL) != 0) {
		perror("timer_settime cancel");
	}
}

/**
 * @brief Fonction d'arrière plan du thread lié à UISS
 * 
 * @return void* 
 */
static void * uiss_run(void * aParam)
{
	MqMsg msg;
    State myState = S_INIT;
    Transition * myTrans;
    pthread_barrier_wait(&start_barrier);
	while (myState != S_DEATH)
    {
        uiss_mqReceive(&msg);
        myTrans = &mySm[myState][msg.data.event];
        //TRACE("myState : %d\n", myState);
        //TRACE("event : %d\n", msg.data.event);
        //TRACE("dest state : %d\n", myTrans->destinationState);
        TRACE("action : %d\n", myTrans->action);
        if (myTrans->destinationState != S_FORGET)
        {
            uiss_performAction(myTrans->action, &msg);
            myState = myTrans->destinationState;
        }
        else
        {
            TRACE("UISS : Event %d ignored\n", msg.data.event);
        }
    }
    TRACE("uiss stopped, wait end of thread\n");
    pthread_barrier_wait(&stop_barrier);
    TRACE("end of thread uiss\n");
    return NULL;
}

/**
 * @brief Fonction principale de Gtk, initialise la fenêtre et les widgets associés
 * 
 * @return void* 
 */
static void * uiss_gtkMain(void *)
{
    // initialisation de GTK
    gtk_init(NULL, NULL);

    // création de la fenêtre
    window_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    window = GTK_WINDOW(window_widget);
    gtk_window_set_title(GTK_WINDOW(window), "SoftSonnette");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // création de la boîte de dialogue
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

        // bouton "Quitter"
    quit_button = gtk_button_new_with_label("Quit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(uiss_on_quit_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), quit_button, FALSE, FALSE, 0);

    gtk_widget_set_halign(quit_button, GTK_ALIGN_START);
    gtk_widget_set_valign(quit_button, GTK_ALIGN_START);

    // bouton "Sonner"
    ring_button = gtk_button_new_with_label("Ring");
    gtk_style_context_add_class(gtk_widget_get_style_context(ring_button), "circular");
    gtk_widget_set_size_request(ring_button, 100, 100);
    g_signal_connect(ring_button, "clicked", G_CALLBACK(uiss_on_ring_button_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(box), ring_button, FALSE, FALSE, 0);

    gtk_widget_set_halign(ring_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(ring_button, GTK_ALIGN_CENTER);

    color_button = (GtkColorButton*)gtk_color_button_new();
    color.red = 1.0;
    color.green = 0.0;
    color.blue = 0.0;
    color.alpha = 1.0;
    gtk_color_button_set_rgba(color_button, &color);
    gtk_box_pack_end(GTK_BOX(box), (GtkWidget*)color_button, FALSE, FALSE, 0);
    gtk_widget_set_sensitive((GtkWidget*)color_button, FALSE);

    gtk_widget_set_halign((GtkWidget*)color_button, GTK_ALIGN_START);
    gtk_widget_set_valign((GtkWidget*)color_button, GTK_ALIGN_START);

    label_door = gtk_label_new("Porte fermée");
    gtk_box_pack_end(GTK_BOX(box), label_door, FALSE, FALSE, 0);
    gtk_widget_set_halign(label_door, GTK_ALIGN_START);
    gtk_widget_set_valign(label_door, GTK_ALIGN_START);


    // zone d'affichage du flux vidéo
    GtkWidget *video_area = gtk_drawing_area_new();
    // Code pour afficher le flux vidéo dans la zone d'affichage
    gtk_box_pack_start(GTK_BOX(box), video_area, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(video_area), "draw", G_CALLBACK(draw_callback), NULL);

    load_css();
    uiss_display(appState);
    
    pthread_barrier_wait(&start_barrier);
    gtk_main();
    TRACE("end of thread gtk\n");
    pthread_barrier_wait(&quit_barrier);

    return NULL;
}

/**
 * @brief Fonction de chargement des éléments css présents sur l'écran d'UISS
 * 
 */
static void load_css()
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    const gchar *css_style_file = "style.css";
    GFile *css_fp = g_file_new_for_path(css_style_file);
    GError *error = 0;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider, css_fp, &error);

    g_object_unref(provider);
}

/**
 * @brief Fonction de callback de l'affichage de la fenêtre vidéo
 * 
 * @param widget Le widget lié à la fenêtre vidéo
 * @param cr Le contexte du dessin pour l'affichage de la fenêtre
 * @param data Paramètre non utilisé
 * @return gboolean 
 */
gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data) {
    
    gint width, height;
    gtk_window_get_size(window, &width, &height); //récupération de la taille de la fenêtre

    //dessin d'un rectangle à la place de la vidéo
    double rectangle_width_percent = 0.6; 
    double rectangle_height_percent = 0.8; 

    double rectangle_width = width * rectangle_width_percent;
    double rectangle_height = height * rectangle_height_percent;

    double rectangle_x = (width - rectangle_width) / 2.0; 
    double rectangle_y = (height - rectangle_height) / 4.0; 

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); //noir
    cairo_rectangle(cr, rectangle_x, rectangle_y, rectangle_width, rectangle_height);

    cairo_fill(cr);

    return FALSE;

    
}
