#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Packets.h"

DQB createDQB(char *name, char *surname) {
	DQB dummy = (DQB) malloc(sizeof(DQB_t));
	memset(dummy, 0, sizeof(DQB_t));
	dummy->type = (char) QUERY_BROADCAST;
	memcpy(dummy->reqName, name, sizeof(dummy->reqName));
	memcpy(dummy->reqSurname, surname, sizeof(dummy->reqSurname));
	return dummy;
}

DQU createDQU(char *reqName, char *reqSurname, char *targetName, char *targetSurname) {
	DQU dummy = (DQU) malloc(sizeof(DQU_t));
	memset(dummy, 0, sizeof(DQU_t));
	dummy->type = (char) QUERY_UNICAST;
	memcpy(dummy->reqName, reqName, sizeof(dummy->reqName));
	memcpy(dummy->reqSurname, reqSurname, sizeof(dummy->reqSurname));
	memcpy(dummy->targetName, targetName, sizeof(dummy->targetName));
	memcpy(dummy->targetSurname, targetSurname, sizeof(dummy->targetSurname));
	return dummy;
}

H_RESP createH_RESP(char *respName, char *respSurname, char *qryName, char *qrySurname) {
	H_RESP dummy = (H_RESP) malloc(sizeof(H_RESP_t));
	memset(dummy, 0, sizeof(H_RESP_t));
	dummy->type = (char) HELLO_RESPONSE;
	memcpy(dummy->respName, respName, sizeof(dummy->respName));
	memcpy(dummy->respSurname, respSurname, sizeof(dummy->respSurname));
	memcpy(dummy->qryName, qryName, sizeof(dummy->qryName));
	memcpy(dummy->qrySurname, qrySurname, sizeof(dummy->qrySurname));
	return dummy;
}

CHAT_MSG createCHAT_MSG(short len, char packetId, char *msg) {
	CHAT_MSG dummy = (CHAT_MSG) malloc(sizeof(CHAT_MSG_t));
	memset(dummy, 0, sizeof(CHAT_MSG_t));
	dummy->type = (char) CHAT;
	dummy->len = (short) len;
	dummy->packetId = packetId;
	memcpy(dummy->message, msg, sizeof(dummy->message));
	return dummy;
}

CHAT_MSG_ACK createCHAT_MSG_ACK(char packetId) {
	CHAT_MSG_ACK dummy = (CHAT_MSG_ACK) malloc(sizeof(CHAT_MSG_ACK_t));
	memset(dummy, 0, sizeof(CHAT_MSG_ACK_t));
	dummy->type = (char) CHAT_ACK;
	dummy->packetId = packetId;
	return dummy;
}

EXIT createEXIT(char *name, char *surname) {
	EXIT dummy = (EXIT) malloc(sizeof(EXIT_t));
	memset(dummy, 0, sizeof(EXIT_t));
	dummy->type = (char) EXITING;
	memcpy(dummy->name, name, sizeof(dummy->name));
	memcpy(dummy->surname, surname, sizeof(dummy->surname));
	return dummy;
}
