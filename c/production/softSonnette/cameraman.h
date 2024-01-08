/**
 * @file 	cameraman.h
 * @brief 	header du fichier cameraman.c
 * @author 	LETENNEUR Laurent
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
 * INCLUDES
 */

#ifndef CAMERAMAN_H
#define CAMERAMAN_H

//#include <stdbool.h>
#include "common.h"
#include <glib.h> 
#include <gtk/gtk.h>
#include <gst/gst.h>

/**
 * @brief Structure regroupant les éléments permettant de gérer le flux video
 * 
 */
typedef struct _CustomData {
  gboolean is_live;
  GstElement *pipeline;
  GMainLoop *loop;
  GstStateChangeReturn ret;
  GstBus *bus;
} CustomData;


extern CustomData inforstream;


/**
 * @brief Méthode permettant de creer la message queue de cameraman.
 * 
 */
void cameraman_new();

/**
 * @brief Méthode permettant le démarage du thread de cameraman.
 * 
 */
void cameraman_start();

/**
 * @brief Méthode permettant d'arrêter le thread de cameraman.
 * 
 */
void cameraman_stop();

/**
 * @brief Méthode permettant de libérer la message queue.
 * 
 */
void cameraman_free();


/**
 * @brief 
 * Méthode permettant de ce connecter au flux video
 * 
 * @param [in] enable Paramètre pour savoir si la demande vient de l'AOP ou de SoftSonnette
 */

void cameraman_subscribeToVideoStream(bool enable);

/**
 * @brief Get the Camera Picture object
 * 
 * @return Picture Retourne une image préenregistrer pour la reconnaissance faciale.
 */

Picture cameraman_takePicture(void);

/**
 * @brief Méthode permettant de démarrer le flux vidéo
 * 
 */
void cameraman_startStreaming(void);

/**
 * @brief Méthode permettant d'arreter le flux vidéo
 * 
 */
void cameraman_stopStreaming(void);

/**
 * @brief Méthode permttant de mettre en pause le flux vidéo
 * 
 */
void cameraman_suspendStreaming(void);

/**
 * @brief Méthode permettant de reprendre le cours du flux vidéo
 * 
 */
void cameraman_resumeStreaming(void);



#endif // cameraman
