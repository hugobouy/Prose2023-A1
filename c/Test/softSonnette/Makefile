CC = gcc
CFLAGS =  -Wextra -std=gnu99 -pthread -Wall $(shell pkg-config --cflags gtk+-3.0 glib-2.0 gstreamer-1.0 gstreamer-video-1.0) -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter
LDFLAGS = -L../cmocka/cmocka-x86_64/lib -lcmocka-static  $(shell pkg-config --libs glib-2.0 gtk+-3.0 gstreamer-1.0 gstreamer-video-1.0) -pthread

# test Bouncer
#CCFLAGS = $(CFLAGS) -Wl,--wrap=clock_getCurrentTime -WL, --wrap=proxyUISP_signalFaceNotAllowed -Wl, --wrap=cameraman_suspendStreaming -Wl, --wrap=cameraman_takePicture -Wl, --wrap=employeeManager_getEmployeeList -Wl, --wrap=recognitionAI_launchAI -Wl, --wrap=cameraman_resumeStreaming 
# test Cameraman
#CCFLAGS = $(CFLAGS) -Wl,--wrap=uiss_updateCamState -WL, --wrap=streamer_streamEcran -Wl, --wrap=streamer_streamTel -Wl, --wrap=streamer_stopPipeline -Wl, --wrap=streamer_cleanPipeline
#test UISS

TARGET = testCameraman

SRCDIR = .
SRCDIR2 = ../
INCDIR = ../cmocka/cmocka-x86_64/include

SRC = $(wildcard $(SRCDIR)/*.c) ../../production/softSonnette/common.h
#SRC += $(SRCDIR2)/guard.c  # Ajout de guard.c à la liste des fichiers source
#SRC += $(SRCDIR2)/proxyConnectionManager.c  # Ajout de guard.c à la liste des fichiers source
#SRC += $(SRCDIR2)/postmanAOP.c  # Ajout de guard.c à la liste des fichiers source
#SRC += $(SRCDIR2)/protocolSS.c  # Ajout de guard.c à la liste des fichiers source

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)
	@echo "compilation achevée"
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	chmod +x $@   # Définit les permissions d'exécution

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
