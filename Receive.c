/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include "Packets.h"
#include "Utils.h"

#define DEST_MAC0	0x54
#define DEST_MAC1	0xEE
#define DEST_MAC2	0x75
#define DEST_MAC3	0xAD
#define DEST_MAC4	0x74
#define DEST_MAC5	0xBB

#define ETHER_TYPE	0x1234

#define DEFAULT_IF	"enp3s0"
#define BUF_SIZ		1024

int main(int argc, char *argv[])
{
	char sender[INET6_ADDRSTRLEN];
	int sockfd, ret;
	int sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct ifreq if_ip;		/* get ip addr */
	struct sockaddr_storage their_addr;
	uint8_t buf[BUF_SIZ];
	char ifName[IFNAMSIZ];

	/* Get interface name */
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName, DEFAULT_IF);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;
	struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
	struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	memset(&if_ip, 0, sizeof(struct ifreq));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");
		return -1;
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
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
	//printf("listener: got packet %lu bytes\n", numbytes);

	/* Check the packet is for me */
	if ((eh->ether_dhost[0] == DEST_MAC0 &&
			eh->ether_dhost[1] == DEST_MAC1 &&
			eh->ether_dhost[2] == DEST_MAC2 &&
			eh->ether_dhost[3] == DEST_MAC3 &&
			eh->ether_dhost[4] == DEST_MAC4 &&
			eh->ether_dhost[5] == DEST_MAC5) ||

				   (eh->ether_dhost[0] == 0xFF &&
					eh->ether_dhost[1] == 0xFF &&
					eh->ether_dhost[2] == 0xFF &&
					eh->ether_dhost[3] == 0xFF &&
					eh->ether_dhost[4] == 0xFF &&
					eh->ether_dhost[5] == 0xFF)) {
		printf("\n\n");
	} else {
		/*printf("Wrong destination MAC: %x:%x:%x:%x:%x:%x\n",
						eh->ether_dhost[0],
						eh->ether_dhost[1],
						eh->ether_dhost[2],
						eh->ether_dhost[3],
						eh->ether_dhost[4],
						eh->ether_dhost[5]);*/
		ret = -1;
		goto done;
	}

	/* Get source IP */
	((struct sockaddr_in *)&their_addr)->sin_addr.s_addr = iph->saddr;
	inet_ntop(AF_INET, &((struct sockaddr_in*)&their_addr)->sin_addr, sender, sizeof sender);

	/* Look up my device IP addr if possible */
	strncpy(if_ip.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0) { /* if we can't check then don't */
		printf("Source IP: %s\n My IP: %s\n", sender,
				inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
		/* ignore if I sent it */
		if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)	{
			printf("but I sent it :(\n");
			ret = -1;
			goto done;
		}
	}

	/* UDP payload length */
	ret = ntohs(udph->len) - sizeof(struct udphdr);

	/* Print packet */
	char *xx = malloc(numbytes-14);
	char *targetAddr = malloc(6);
	memcpy(targetAddr, &buf[6], 6);
	memcpy(xx, &buf[14], numbytes-14);
	char type = (char) buf[14];
	if(type == QUERY_BROADCAST){
		DQB myDQB = (DQB) toStruct(xx);
		printf("\nDQB - [%d][%s][%s]", myDQB->type, myDQB->reqName, myDQB->reqSurname);
		fflush(stdout);
		char name[10] = "ugur";
		char surname[10] = "ilter";
		char *response = (char *) createH_RESP(name, surname, myDQB->reqName, myDQB->reqSurname);
		sendPacket(targetAddr, DEFAULT_IF, 1024, response);
		free(myDQB);
		free(response);
	}
	else if(type == QUERY_UNICAST){
		DQU myDQU = (DQU) toStruct(xx);
		printf("\nDQU - [%d][%s][%s][%s][%s]", myDQU->type, myDQU->reqName, myDQU->reqSurname, myDQU->targetName, myDQU->targetSurname);
		fflush(stdout);
		char name[10] = "ugur";
		char surname[10] = "ilter";
		char *response = (char *) createH_RESP(name, surname, myDQU->targetName, myDQU->targetSurname);
		sendPacket(targetAddr, DEFAULT_IF, 1024, response);
		free(myDQU);
		free(response);
	}
	else if(type == HELLO_RESPONSE){
		H_RESP myH_RESP = (H_RESP) toStruct(xx);
		printf("\nH_RESP - [%d][%s][%s][%s][%s]", myH_RESP->type, myH_RESP->respName, myH_RESP->respSurname, myH_RESP->qryName, myH_RESP->qrySurname);

		saveMac(targetAddr, myH_RESP->qryName, myH_RESP->qrySurname);

		fflush(stdout);
		free(myH_RESP);
	}
	else if(type == CHAT){
		CHAT_MSG myCHAT_MSG = (CHAT_MSG) toStruct(xx);
		char packetId = myCHAT_MSG->packetId;
		printf("\nCHAT - [%d][%hi][%d][%s]", myCHAT_MSG->type, myCHAT_MSG->len, myCHAT_MSG->packetId, myCHAT_MSG->message);
		fflush(stdout);

		char *response = (char *) createCHAT_MSG_ACK(packetId);
		sendPacket(targetAddr, DEFAULT_IF, 1024, response);
		free(myCHAT_MSG);
		free(response);
	}
	else if(type == CHAT_ACK){
		CHAT_MSG_ACK myCHAT_MSG_ACK = (CHAT_MSG_ACK) toStruct(xx);
		printf("\nCHAT_MSG_ACK - [%d][%d]", myCHAT_MSG_ACK->type, myCHAT_MSG_ACK->packetId);
		fflush(stdout);
		free(myCHAT_MSG_ACK);
	}
	else if(type == CHAT_ACK){
		EXIT myEXIT = (EXIT) toStruct(xx);
		printf("\nEXIT - [%d][%s][%s]", myEXIT->type, myEXIT->name, myEXIT->surname);
		fflush(stdout);
		free(myEXIT);
	}

	free(xx);
	free(targetAddr);

done: goto repeat;
	close(sockfd);
	return ret;
}
