#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "Packets.h"
#include "Utils.h"

void *toStruct(char *addr){
	void *result;
	char type = (char) addr[0];

	if(type == QUERY_BROADCAST){
		DQB new = (DQB) malloc(sizeof(DQB_t));
		memset(new, 0, sizeof(DQB_t));
		new->type = (char) QUERY_BROADCAST;
		strcpy(new->reqName, addr + sizeof(new->type));
		strcpy(new->reqSurname, addr + sizeof(new->type) + sizeof(new->reqName));
		result = new;
	}
	else if(type == QUERY_UNICAST){
		DQU new = (DQU) malloc(sizeof(DQU_t));
		memset(new, 0, sizeof(DQU_t));
		new->type = (char) QUERY_UNICAST;
		strcpy(new->reqName, addr + sizeof(new->type));
		strcpy(new->reqSurname, addr + sizeof(new->type) + sizeof(new->reqName));
		strcpy(new->targetName, addr + sizeof(new->type) + sizeof(new->reqName) + sizeof(new->reqSurname));
		strcpy(new->targetSurname, addr + sizeof(new->type) + sizeof(new->reqName) + sizeof(new->reqSurname) + sizeof(new->targetName));
		result = new;
	}
	else if(type == HELLO_RESPONSE){
		H_RESP new = (H_RESP) malloc(sizeof(H_RESP_t));
		memset(new, 0, sizeof(H_RESP_t));
		new->type = (char) HELLO_RESPONSE;
		strcpy(new->respName, addr + sizeof(new->type));
		strcpy(new->respSurname, addr + sizeof(new->type) + sizeof(new->respName));
		strcpy(new->qryName, addr + sizeof(new->type) + sizeof(new->respName) + sizeof(new->respSurname));
		strcpy(new->qrySurname, addr + sizeof(new->type) + sizeof(new->respName) + sizeof(new->respSurname) + sizeof(new->qryName));
		result = new;
	}
	else if(type == CHAT){
		CHAT_MSG new = (CHAT_MSG) malloc(sizeof(CHAT_MSG_t));
		memset(new, 0, sizeof(CHAT_MSG_t));
		new->type = (char) CHAT;
		new->len = (addr[2] << 8) | addr[1];
		new->packetId = addr[3];
		strcpy(new->message, addr + sizeof(new->type) + sizeof(new->len) + sizeof(new->packetId));
		result = new;
	}
	else if(type == CHAT_ACK){
		CHAT_MSG_ACK new = (CHAT_MSG_ACK) malloc(sizeof(CHAT_MSG_ACK_t));
		memset(new, 0, sizeof(CHAT_MSG_ACK_t));
		new->type = (char) CHAT_ACK;
		new->packetId = addr[1];
		result = new;
	}
	else if(type == EXITING){
		EXIT new = (EXIT) malloc(sizeof(EXIT_t));
		memset(new, 0, sizeof(EXIT_t));
		new->type = (char) EXITING;
		strcpy(new->name, addr + sizeof(new->type));
		strcpy(new->surname, addr + sizeof(new->type) + sizeof(new->name));
		result = new;
	}

	else printf("Unknown Packet Type - Discarding . . !");

	return result;
}

void sendPacket(u_int8_t *destMac, char *interface, int bufSize, char *packet){
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
	memcpy(eh->ether_shost, ((uint8_t *)&if_mac.ifr_hwaddr.sa_data), 6);
	memcpy(eh->ether_dhost, destMac, 6);

	/* Ethertype field */
	eh->ether_type = htons(0x1234);
	tx_len += sizeof(struct ether_header);

	/* Fill data */
	char type = packet[0];
	int ctr, size = typeSize(type);
	//memcpy(&sendbuf[tx_len], packet, size);
	for(ctr = 0; ctr < size; ctr++) sendbuf[tx_len++] = packet[ctr];

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;		/* Octets in one ethernet addr	 */
	/* Destination MAC */
	memcpy(socket_address.sll_addr, destMac, 6);

	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
		printf("Send failed\n");
}

void *startReceiver()
{
	u_int8_t MY_MAC[6] = {0x54, 0xEE, 0x75, 0xAD, 0x74, 0xBB};
	u_int8_t BCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	char sender[INET6_ADDRSTRLEN];
	int sockfd;
	int sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct ifreq if_ip;		/* get ip addr */
	struct sockaddr_storage their_addr;
	uint8_t buf[BUF_SIZ];
	char ifName[IFNAMSIZ];

	/* Get interface name */
	strcpy(ifName, DEFAULT_IFACE);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;

	memset(&if_ip, 0, sizeof(struct ifreq));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");
	}

	/* Set interface to promiscuous mode - do we need to do this every time? */
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

repeat:
	//printf("listener: Waiting to recvfrom...\n");
	memset(buf, 0, 1024);
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
	//printf("listener: got packet %lu bytes\n", numbytes);

	/* Check the packet is for me */
	if (!(macCompare(eh->ether_dhost, MY_MAC) || macCompare(eh->ether_dhost, BCAST_MAC))) goto done;

	/* Get source IP */
	inet_ntop(AF_INET, &((struct sockaddr_in*)&their_addr)->sin_addr, sender, sizeof sender);

	/* Look up my device IP addr if possible */
	strncpy(if_ip.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0) { /* if we can't check then don't */
//		printf("Source IP: %s\n My IP: %s\n", sender,
//				inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
		/* ignore if I sent it */
		if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)	{
			printf("but I sent it :(\n");
			goto done;
		}
	}

	/* UDP payload length */

	/* Print packet */
	char *packet = malloc(numbytes - ETH_HLEN);
	memset(packet, 0, numbytes-ETH_HLEN);
	u_int8_t *targetAddr = malloc(ETH_ALEN);
	memset(targetAddr, 0, ETH_ALEN);
	memcpy(targetAddr, &buf[ETH_ALEN], ETH_ALEN);
	memcpy(packet, &buf[ETH_HLEN], numbytes - ETH_HLEN);
	char type = (char) buf[14];
	if(type == QUERY_BROADCAST){
		DQB myDQB = (DQB) toStruct(packet);
		fflush(stdout);
		char name[sizeof(myDQB->reqName)] = "ugur";
		char surname[sizeof(myDQB->reqSurname)] = "ilter";
		char *response = (char *) createH_RESP(name, surname, myDQB->reqName, myDQB->reqSurname);
		memcpy(targetAddr, &buf[ETH_ALEN], ETH_ALEN);
		sendPacket(BCAST_MAC, DEFAULT_IFACE, 1024, response);
		free(myDQB);
		free(response);
	}
	else if(type == QUERY_UNICAST){
		DQU myDQU = (DQU) toStruct(packet);
		fflush(stdout);
		char name[sizeof(myDQU->reqName)] = "ugur";
		char surname[sizeof(myDQU->reqSurname)] = "ilter";
		char *response = (char *) createH_RESP(name, surname, myDQU->reqName, myDQU->reqSurname);
		sendPacket(BCAST_MAC, DEFAULT_IFACE, 1024, response);
		free(myDQU);
		free(response);
	}
	else if(type == HELLO_RESPONSE){
		H_RESP myH_RESP = (H_RESP) toStruct(packet);
		saveMac(targetAddr, myH_RESP->respName, myH_RESP->respSurname);
		fflush(stdout);
		free(myH_RESP);
	}
	else if(type == CHAT){
		CHAT_MSG myCHAT_MSG = (CHAT_MSG) toStruct(packet);
		char packetId = myCHAT_MSG->packetId;
		printf("Incoming MSG from %s < %s", targetAddr, myCHAT_MSG->message);
		fflush(stdout);
		char *response = (char *) createCHAT_MSG_ACK(packetId);
		sendPacket(targetAddr, DEFAULT_IFACE, 1024, response);
		free(myCHAT_MSG);
		free(response);
	}
	else if(type == CHAT_ACK){
		CHAT_MSG_ACK myCHAT_MSG_ACK = (CHAT_MSG_ACK) toStruct(packet);
		printf("CHAT_MSG_ACK - [%d][%d]", myCHAT_MSG_ACK->type, myCHAT_MSG_ACK->packetId);
		fflush(stdout);
		free(myCHAT_MSG_ACK);
	}
	else if(type == EXITING){
		EXIT myEXIT = (EXIT) toStruct(packet);
		printf("EXIT - [%d][%s][%s]", myEXIT->type, myEXIT->name, myEXIT->surname);
		fflush(stdout);
		free(myEXIT);
	}

	free(packet);
	free(targetAddr);

done: goto repeat;
	close(sockfd);
}

void sendDQB(){
	char name[10] = "ugur";
	char surname[10] = "ilter";
	char *myDQB = (char *) createDQB(name, surname);
	u_int8_t destMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	sendPacket(destMac, DEFAULT_IFACE, 1024, myDQB);
	free(myDQB);
}

void sendDQU(u_int8_t *destMac, char *targetName, char *targetSurname){
	char name[10] = "ugur";
	char surname[10] = "ilter";
	char *myDQU = (char *) createDQU(name, surname, targetName, targetSurname);
	sendPacket(destMac, DEFAULT_IFACE, 1024, myDQU);
	free(myDQU);
}

void sendH_RESP(u_int8_t *destMac, char *targetName, char *targetSurname){
	char name[10] = "ugur";
	char surname[10] = "ilter";
	char *myH_RESP = (char *) createH_RESP(name, surname, targetName, targetSurname);
	sendPacket(destMac, DEFAULT_IFACE, 1024, myH_RESP);
	free(myH_RESP);
}

void sendCHAT_MSG(u_int8_t *destMac, short len, char packetId, char *msg){
	char *myCHAT_MSG = (char *) createCHAT_MSG(len, packetId, msg);
	sendPacket(destMac, DEFAULT_IFACE, 1024, myCHAT_MSG);
	free(myCHAT_MSG);
}

int macCompare(u_int8_t *a, u_int8_t *b){
	if((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]) || (a[3] != b[3]) || (a[4] != b[4]) || (a[5] != b[5])) return 0;
	return 1;
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

void saveMac(u_int8_t *targetAddr, char *name, char *surname){
	FILE *file = fopen("maclist.txt", "a");

	if (file == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	u_int8_t *mac = byteToHex((u_int8_t *) targetAddr);
	fprintf(file, "%s %s %s\n", name, surname, mac);

	fclose(file);
}

unsigned char *byteToHex(u_int8_t *str){
	int i;
	u_int8_t *result = malloc(12);
	for(i=0; i<6; i++){
		int num = (int) str[i];
		result[2*i] = (u_int8_t) hexToDec(num / 16);
		result[2*i + 1] = (u_int8_t) hexToDec(num % 16);
	}
	return result;
}

char hexToDec(int num){
	char c;
	if(num >=0 && num <= 9) c = (char) num + 48;
	else c = (char) num + 55;
	return c;
}
