#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Packets.h"

DQB createDQB(char *name, char *surname){
	DQB dummy = (DQB) malloc (sizeof(DQB_t));
	dummy->type = (char) QUERY_BROADCAST;
	strcpy(dummy->reqName, name);
	strcpy(dummy->reqSurname, surname);
	return dummy;
}

DQU createDQU(char *reqName, char *reqSurname, char *targetName, char *targetSurname){
	DQU dummy = (DQU) malloc (sizeof(DQU_t));
	dummy->type = (char) QUERY_UNICAST;
	strcpy(dummy->reqName, 			reqName);
	strcpy(dummy->reqSurname, 		reqSurname);
	strcpy(dummy->targetName,		targetName);
	strcpy(dummy->targetSurname,	targetSurname);
	return dummy;
}

H_RESP createH_RESP(char *respName, char *respSurname, char *qryName, char *qrySurname){
	H_RESP dummy = (H_RESP) malloc (sizeof(H_RESP_t));
	dummy->type = (char) HELLO_RESPONSE;
	strcpy(dummy->respName, 	respName);
	strcpy(dummy->respSurname,	respSurname);
	strcpy(dummy->qryName, 		qryName);
	strcpy(dummy->qrySurname,	qrySurname);
	return dummy;
}

CHAT_MSG createCHAT_MSG(short len, char packetId, char *msg){
	CHAT_MSG dummy = (CHAT_MSG) malloc (sizeof(CHAT_MSG_t));
	dummy->type = (char) CHAT;
	dummy->len = (short) len;
	dummy->packetId = packetId;
	strcpy(dummy->message,		msg);
	return dummy;
}

CHAT_MSG_ACK createCHAT_MSG_ACK(char packetId){
	CHAT_MSG_ACK dummy = (CHAT_MSG_ACK) malloc (sizeof(CHAT_MSG_ACK_t));
	dummy->type = (char) CHAT_ACK;
	dummy->packetId = packetId;
	return dummy;
}

EXIT createEXIT(char *name, char *surname){
	EXIT dummy = (EXIT) malloc (sizeof(EXIT_t));
	dummy->type = (char) EXITING;
	strcpy(dummy->name,		name);
	strcpy(dummy->surname,	surname);
	return dummy;
}

DQB init_dummy_DQB(){
	DQB dummy = (DQB) malloc (sizeof(DQB_t));

	dummy->type = (char) QUERY_BROADCAST;
	strcpy(dummy->reqName, "Ugur");
	strcpy(dummy->reqSurname, "Ilter");

	return dummy;
}

DQU init_dummy_DQU(){
	DQU dummy = (DQU) malloc (sizeof(DQU_t));

	dummy->type = (char) QUERY_UNICAST;
	strcpy(dummy->reqName, "Pelin");
	strcpy(dummy->reqSurname, "Fidangul");
	strcpy(dummy->targetName, "Ugur");
	strcpy(dummy->targetSurname, "Ilter");

	return dummy;
}

H_RESP init_dummy_H_RESP(){
	H_RESP dummy = (H_RESP) malloc (sizeof(H_RESP_t));

	dummy->type = (char) HELLO_RESPONSE;
	strcpy(dummy->respName, "Deli");
	strcpy(dummy->respSurname, "Cevat");
	strcpy(dummy->qryName, "Gokhan");
	strcpy(dummy->qrySurname, "Dabak");

	return dummy;
}

CHAT_MSG init_dummy_CHAT_MSG(){
	CHAT_MSG dummy = (CHAT_MSG) malloc (sizeof(CHAT_MSG_t));

	dummy->type = (char) CHAT;
	dummy->len = (short) 23;
	dummy->packetId = (char) 80;
	strcpy(dummy->message, "Bu bir deneme mesajidir");

	return dummy;
}

CHAT_MSG_ACK init_dummy_CHAT_MSG_ACK(){
	CHAT_MSG_ACK dummy = (CHAT_MSG_ACK) malloc (sizeof(CHAT_MSG_ACK_t));

	dummy->type = (char) CHAT_ACK;
	dummy->packetId = (char) 8;

	return dummy;
}

EXIT init_dummy_EXIT(){
	EXIT dummy = (EXIT) malloc (sizeof(EXIT_t));

	dummy->type = (char) EXITING;
	strcpy(dummy->name, "Barack");
	strcpy(dummy->surname, "Obama");

	return dummy;
}
