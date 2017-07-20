#ifndef UTILS_H
#define UTILS_H

#define MY_NAME			"Ugur"
#define MY_SURNAME		"Ilter"
#define BRIDGE_IFACE	"enp3s0"

char *toHex(void *addr, int len);
void *toStruct(char *addr);
void sendPacket(char *destMac, char *interface, int bufSize, char *packet);

void sendDQB();
void sendDQU(char *destMac, char *targetName, char *targetSurname);
void sendH_RESP(char *destMac, char *targetName, char *targetSurname);

int typeSize(char type);

void saveMac(char *targetAddr, char *name, char *surname);

#endif

