/**
 * @file 	streamer.c
 * @brief 	code de la classe streamer
 * @author 	LETENNEUR Laurent
 * @date 	25/05/2023
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

#define TARGET "192.168.72.13"

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <string.h>

#include <glib.h> 
#include <gtk/gtk.h>

#include <stddef.h> //  Macros, type dont NULL et size_t
#include <unistd.h> // Macros, type Posix and co
#include <mqueue.h>
#include <sys/stat.h> // pour constantes de mode_t
#include <errno.h>

#include "streamer.h"

#include "cameraman.h"


/*
 * LOCAL FUNCTIONS
 */

/*
 * LOCAL VARIABLES
 */

 CustomData inforstream;

/******************************************************************************/
void streamer_initGstreamer()
{
    memset (&inforstream, 0, sizeof (inforstream));
}


void streamer_streamEcran()
{
    streamer_initGstreamer();
    TRACE("pipeline Ecran allumer \n");
    inforstream.pipeline = gst_parse_launch ("v4l2src device=/dev/video0 ! video/x-raw,width=320,height=240 ! videoconvert  ! waylandsink name=waylandsink0", NULL);
    gst_element_set_state (inforstream.pipeline, GST_STATE_PLAYING);

    
}

void streamer_streamTel()
{
    streamer_initGstreamer();
    TRACE("pipeline Ecran et TEL allumer \n");
  gchar* pipeline_str = g_strdup_printf("v4l2src device=/dev/video0 ! video/x-raw,width=320,height=240 ! tee name=t t.! queue ! videoconvert ! queue ! avenc_mpeg4 ! queue ! udpsink host=%s port=5000  t. ! queue ! videoconvert ! waylandsink name=waylandsink0 ", TARGET);
  inforstream.pipeline = gst_parse_launch (pipeline_str, NULL);
  g_free(pipeline_str);
  gst_element_set_state (inforstream.pipeline, GST_STATE_PLAYING);
    
}

void streamer_cleanPipeline()
{
    gst_object_unref (inforstream.pipeline);
}

void streamer_stopPipeline()
{
    gst_element_set_state (inforstream.pipeline, GST_STATE_NULL);
}
