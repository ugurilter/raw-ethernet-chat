-include ${BSPROOT}/.config

APP_TARGET = chatApp

APP_SOURCES = Application.c
APP_DEPS = Utils.c Packets.c	

CFLAGS += -Wall -pthread

default: all

all: $(APP_TARGET) $(RECEIVE_TARGET)

$(APP_TARGET): $(APP_SOURCES) $(APP_DEPS)
	$(CC) $(CFLAGS) $(APP_SOURCES) $(APP_DEPS) -o $(APP_TARGET)

clean:
	rm -rf $(TARGET)


::D:D:D:D
