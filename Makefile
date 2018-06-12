APP_TARGET = chatApp
APP_SOURCES = Application.c
APP_DEPS = Utils.c Packets.c	

CFLAGS += -Wall -pthread

default: all

all: 
	$(CC) $(CFLAGS) $(APP_SOURCES) $(APP_DEPS) -o $(APP_TARGET)

clean:
	rm -rf $(APP_TARGET)
