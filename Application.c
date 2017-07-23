#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "Packets.h"
#include "Utils.h"

int main(){
    pthread_t receiverThread;
    pthread_create(&receiverThread, NULL, startReceiver, NULL);
	char *msg = malloc(256);
	int packetId = 0;
//	int choice;

	u_int8_t target[] = { 0x20, 0x89, 0x84, 0x44, 0xB4, 0x41 };
	char *targetName = malloc(10);
	memset(targetName, 0, 10);
	memcpy(targetName, "pelin", 10);
	char *targetSurname = malloc(10);
	memset(targetSurname, 0, 10);
	memcpy(targetSurname, "fidangul", 10);

	while(1){
//		scanf("%d", &choice);

//		switch(choice){
//			case 0:
//				sendDQB();
//				sleep(2);
//				break;
//			case 1:
//				sendDQU(target, targetName, targetSurname);
//				sleep(2);
//				break;
//			case 2:
//				sendH_RESP(target, targetName, targetSurame);
//				sleep(2);
//				break;
//			case 3:
				memset(msg, 0, 256);
				fflush(stdout);
				fgets(msg,256,stdin);
				sendCHAT_MSG(target, strlen(msg)-1, (char)packetId++, msg);
				printf("Outgoing MSG from ugur > %s", msg);
//				sleep(3);
//				break;
//			default:
//				break;
//		}

	}

	return 0;
}
