CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
LDFLAGS = -L../cmocka/cmocka-x86_64/lib -lcmocka -pthread -Wl,-rpath=../cmocka/cmocka-x86_64/lib
CCFLAGS = $(CFLAGS) -Wl,--wrap=proxyConnectionManager_validatePass
TARGET = testGuard

SRCDIR = .
SRCDIR2 = ../../production/softSonnette/
INCDIR = ../cmocka/cmocka-x86_64/include

SRC = $(wildcard $(SRCDIR)/*.c)
#SRC += $(SRCDIR2)/guard.c  # Ajout de guard.c à la liste des fichiers source
SRC += $(SRCDIR2)/proxyConnectionManager.h  # Ajout de guard.c à la liste des fichiers source
#SRC += $(SRCDIR2)/postmanAOP.c  # Ajout de guard.c à la liste des fichiers source
#SRC += $(SRCDIR2)/protocolSS.c  # Ajout de guard.c à la liste des fichiers source


OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)
	@echo "compilation achevée"
$(TARGET): $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)
	chmod +x $@   # Définit les permissions d'exécution

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

