-include ${BSPROOT}/.config

APP_TARGET = application
RECEIVE_TARGET = receive

APP_SOURCES = Application.c
APP_DEPS = Utils.c Packets.c	

RECEIVE_SOURCES = Receive.c
RECEIVE_DEPS = Utils.c Packets.c

CFLAGS += -Wall

default: all

all: $(APP_TARGET) $(RECEIVE_TARGET)

$(APP_TARGET): $(APP_SOURCES) $(APP_DEPS)
	$(CC) $(CFLAGS) $(APP_SOURCES) $(APP_DEPS) -o $(APP_TARGET)
	
$(RECEIVE_TARGET): $(RECEIVE_SOURCES) $(RECEIVE_DEPS)
	$(CC) $(CFLAGS) $(RECEIVE_SOURCES) $(RECEIVE_DEPS) -o $(RECEIVE_TARGET)

clean:
	rm -rf $(TARGET)
