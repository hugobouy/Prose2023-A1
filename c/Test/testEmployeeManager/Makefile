CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
LDFLAGS = -L../cmocka/cmocka-x86_64/lib -lcmocka -pthread -Wl,-rpath=../cmocka/cmocka-x86_64/lib
CCFLAGS = $(CFLAGS) -Wl,--wrap=proxyGUI_setEmployeeList		
TARGET = testEmployeeManager

SRCDIR = .
SRCDIR2 = ../../production/softSonnette/
INCDIR = ../cmocka/cmocka-x86_64/include

SRC = $(wildcard $(SRCDIR)/*.c)

SRC += $(SRCDIR2)/proxyGUI.h  



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

