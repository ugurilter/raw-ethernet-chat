#ifndef PACKETS_H
#define PACKETS_H

typedef enum {
   QUERY_BROADCAST,
   QUERY_UNICAST,
   HELLO_RESPONSE,
   CHAT,
   CHAT_ACK,
   EXITING,
} PACKET_TYPE;

typedef struct DQB_s{
	char type;
	char reqName[10];
	char reqSurname[10];
}__attribute__((__packed__)) DQB_t[1], *DQB;

typedef struct DQU_s{
	char type;
	char reqName[10];
	char reqSurname[10];
	char targetName[10];
	char targetSurname[10];
}__attribute__((__packed__)) DQU_t[1], *DQU;

typedef struct H_RESP_s{
	char type;
	char respName[10];
	char respSurname[10];
	char qryName[10];
	char qrySurname[10];
}__attribute__((__packed__)) H_RESP_t[1], *H_RESP;

typedef struct CHAT_MSG_s{
	char type;
	short len;
	char packetId;
	char message[256];
}__attribute__((__packed__)) CHAT_MSG_t[1], *CHAT_MSG;

typedef struct CHAT_MSG_ACK_s{
	char type;
	char packetId;
}__attribute__((__packed__)) CHAT_MSG_ACK_t[1], *CHAT_MSG_ACK;

typedef struct EXIT_s{
	char type;
	char name[10];
	char surname[10];
}__attribute__((__packed__)) EXIT_t[1], *EXIT;

DQB createDQB(char *name, char *surname);
DQU createDQU(char *reqName, char *reqSurname, char *targetName, char *targetSurname);
H_RESP createH_RESP(char *respName, char *respSurname, char *qryName, char *qrySurname);
CHAT_MSG createCHAT_MSG(short len, char packetId, char *msg);
CHAT_MSG_ACK createCHAT_MSG_ACK(char packetId);
EXIT createEXIT(char *name, char *surname);

#endif
