#ifndef UTILS_H
#define UTILS_H

#define DEFAULT_IFACE	"enp3s0"
#define ETHER_TYPE		0x1234
#define BUF_SIZ			1024

void *toStruct(char *addr);
void sendPacket(u_int8_t *destMac, char *interface, int bufSize, char *packet);
void *startReceiver();

void sendDQB();
void sendDQU(u_int8_t *destMac, char *targetName, char *targetSurname);
void sendH_RESP(u_int8_t *destMac, char *targetName, char *targetSurname);
void sendCHAT_MSG(u_int8_t *destMac, short len, char packetId, char *msg);

int typeSize(char type);

char hexToDec(int num);
unsigned char *byteToHex(unsigned char *str);
void saveMac(u_int8_t *targetAddr, char *name, char *surname);
int macCompare(u_int8_t *a, u_int8_t *b);

#endif

