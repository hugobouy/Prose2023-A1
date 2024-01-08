PROG = softSonnette.out
SRCS = starter.c cameraman.c dispatcherAOP.c postmanAOP.c protocolSS.c dispatcherSP.c postmanSP.c protocolSP.c uiss.c bouncer.c recognitionAI.c employeeManager.c clock.c guard.c proxyConnectionManager.c streamer.c proxyGUI.c proxyDoorManager.c proxyUISP.c
# a modifier pour ce qu'on veux compiler + faire le main

CLEANFILES = $(PROG)

# Add / change option in CFLAGS and LDFLAGS
CFLAGS += -Wall $(shell pkg-config --cflags gtk+-3.0 gstreamer-1.0 gstreamer-video-1.0)
LDFLAGS += $(shell pkg-config --libs gtk+-3.0 gstreamer-1.0 gstreamer-video-1.0) -ljpeg

all: $(PROG)

$(PROG): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(CLEANFILES) $(patsubst %.c,%.o, $(SRCS))
