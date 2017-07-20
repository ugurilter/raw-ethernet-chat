#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Packets.h"
#include "Utils.h"

char *toHex(void *addr, int len){
	char hex[] = "0123456789ABCDEF";
	char *ptr = addr;
	char *res = malloc(2 * len);

	int i;
	for(i = 0; i < 2 * len; i+=2){
		char c = *((char *) ptr);
		res[i] = hex[c / 16];
		res[i+1] = hex[c % 16];
		ptr++;
	}

	return res;
}

void *toStruct(char *addr){
	void *result;
	char type = (char) addr[0];

	if(type == QUERY_BROADCAST){
		DQB new = (DQB) malloc(sizeof(DQB_t));
		new->type = (char) QUERY_BROADCAST;
		strcpy(new->reqName, addr + sizeof(new->type));
		strcpy(new->reqSurname, addr + sizeof(new->type) + sizeof(new->reqName));
		result = new;
	}
	else if(type == QUERY_UNICAST){
		DQU new = (DQU) malloc(sizeof(DQU_t));
		new->type = (char) QUERY_UNICAST;
		strcpy(new->reqName, addr + sizeof(new->type));
		strcpy(new->reqSurname, addr + sizeof(new->type) + sizeof(new->reqName));
		strcpy(new->targetName, addr + sizeof(new->type) + sizeof(new->reqName) + sizeof(new->reqSurname));
		strcpy(new->targetSurname, addr + sizeof(new->type) + sizeof(new->reqName) + sizeof(new->reqSurname) + sizeof(new->targetName));
		result = new;
	}
	else if(type == HELLO_RESPONSE){
		H_RESP new = (H_RESP) malloc(sizeof(H_RESP_t));
		new->type = (char) HELLO_RESPONSE;
		strcpy(new->respName, addr + sizeof(new->type));
		strcpy(new->respSurname, addr + sizeof(new->type) + sizeof(new->respName));
		strcpy(new->qryName, addr + sizeof(new->type) + sizeof(new->respName) + sizeof(new->respSurname));
		strcpy(new->qrySurname, addr + sizeof(new->type) + sizeof(new->respName) + sizeof(new->respSurname) + sizeof(new->qryName));
		result = new;
	}
	else if(type == CHAT){
		CHAT_MSG new = (CHAT_MSG) malloc(sizeof(CHAT_MSG_t));
		new->type = (char) CHAT;
		new->len = (addr[2] << 8) | addr[1];
		new->packetId = addr[3];
		strcpy(new->message, addr + sizeof(new->type) + sizeof(new->len) + sizeof(new->packetId));
		result = new;
	}
	else if(type == CHAT_ACK){
		CHAT_MSG_ACK new = (CHAT_MSG_ACK) malloc(sizeof(CHAT_MSG_ACK_t));
		new->type = (char) CHAT_ACK;
		new->packetId = addr[1];
		result = new;
	}
	else if(type == EXITING){
		EXIT new = (EXIT) malloc(sizeof(EXIT_t));
		new->type = (char) EXITING;
		strcpy(new->name, addr + sizeof(new->type));
		strcpy(new->surname, addr + sizeof(new->type) + sizeof(new->name));
		result = new;
	}

	else printf("Unknown Packet Type - Discarding . . !");

	return result;
}

void sendPacket(char *destMac, char *interface, int bufSize, char *packet){
	int sockfd;

	struct ifreq if_idx;		/* Socket ioctl için kullanılan iface request structure'ı */
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[bufSize];
	struct ether_header *eh = (struct ether_header *) sendbuf;		/* 10Mb/s ethernet header */
	struct sockaddr_ll socket_address;
	char ifName[IFNAMSIZ];

	strcpy(ifName, interface);

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)				/* name -> if_index mapping	*/
		perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)				/* Get hardware address		*/
		perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, bufSize);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = destMac[0];
	eh->ether_dhost[1] = destMac[1];
	eh->ether_dhost[2] = destMac[2];
	eh->ether_dhost[3] = destMac[3];
	eh->ether_dhost[4] = destMac[4];
	eh->ether_dhost[5] = destMac[5];
	/* Ethertype field */
	eh->ether_type = htons(0x1234);
	tx_len += sizeof(struct ether_header);

	/* Fill data */
	char type = packet[0];
	int size = typeSize(type);
	memcpy(&sendbuf[tx_len], packet, size);
//	for(ctr = 0; ctr < size; ctr++) sendbuf[tx_len++] = packet[ctr];

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;		/* Octets in one ethernet addr	 */
	/* Destination MAC */
	socket_address.sll_addr[0] = destMac[0];
	socket_address.sll_addr[1] = destMac[1];
	socket_address.sll_addr[2] = destMac[2];
	socket_address.sll_addr[3] = destMac[3];
	socket_address.sll_addr[4] = destMac[4];
	socket_address.sll_addr[5] = destMac[5];

	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
		printf("Send failed\n");
}

void sendDQB(){
	char name[10] = "ugur";
	char surname[10] = "ilter";
	char *myDQB = (char *) createDQB(name, surname);
	char destMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	sendPacket(destMac, BRIDGE_IFACE, 1024, myDQB);
	free(myDQB);
}

void sendDQU(char *destMac, char *targetName, char *targetSurname){
	char *myDQU = (char *) createDQU(MY_NAME, MY_SURNAME, targetName, targetSurname);
	sendPacket(destMac, BRIDGE_IFACE, 1024, myDQU);
	free(myDQU);
}

void sendH_RESP(char *destMac, char *targetName, char *targetSurname){
	char *myH_RESP = (char *) createH_RESP(MY_NAME, MY_SURNAME, targetName, targetSurname);
	sendPacket(destMac, BRIDGE_IFACE, 1024, myH_RESP);
	free(myH_RESP);
}

void sendCHAT_MSG(char *destMac, short len, char packetId, char *msg){
	char *myCHAT_MSG = (char *) createCHAT_MSG(len, packetId, msg);
	sendPacket(destMac, BRIDGE_IFACE, 1024, myCHAT_MSG);
	free(myCHAT_MSG);
}

int typeSize(char type){
	if(type == QUERY_BROADCAST) return sizeof(DQB_t);
	if(type == QUERY_UNICAST) return sizeof(DQU_t);
	if(type == HELLO_RESPONSE) return sizeof(H_RESP_t);
	if(type == CHAT) return sizeof(CHAT_MSG_t);
	if(type == CHAT_ACK) return sizeof(CHAT_MSG_ACK_t);
	if(type == EXITING) return sizeof(EXIT_t);
	return 0;
}

void saveMac(char *targetAddr, char *name, char *surname){
	FILE *file = fopen("maclist.txt", "w");
	if (file == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	/* print some text */
	fprintf(file, "%s %s %s", name, surname, targetAddr);

	fclose(file);
}

//int known(char *name, char *surname){
//
//}
