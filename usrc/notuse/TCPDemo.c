/*
  SIMCOM SIM7600 TCP DEMO
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "common.h"


#define MAX_BUFF_LEN      512

int TCPTest(char *serverIP, int serverPort, uint8_t *send_data, uint32_t send_len)
{
	int socketfd;
	int length;
	fd_set fds;
	int ret;
	struct timeval timeout = {30,0};
	
	struct sockaddr_in socketaddr;
	uint8_t buff[MAX_BUFF_LEN] = {0};
	memset(&socketaddr, 0, sizeof(socketaddr));
	socketaddr.sin_family = AF_INET;
	socketaddr.sin_port = htons(serverPort);
	socketaddr.sin_addr.s_addr = inet_addr(serverIP);

	if((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		printf("create socket fail\n");
		return -1;
	}

	if (connect(socketfd, (struct sockaddr*) &socketaddr, sizeof(socketaddr)) == -1)
	{
		printf("Connect to server fail\n");
		close(socketfd);
	}

	length = send(socketfd, send_data, send_len, 0);
	if (length < 0)
	{
		printf("send fail.\n");
		close(socketfd);
		return -1;
	}
	else if(length == 0)
	{
		printf("server socket close\n");
		close(socketfd);
		return -1;
	}
	else 
	{
		printf("send success\n");
	}

	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	memset(buff, 0, sizeof(buff));
	ret = recv(socketfd, buff, sizeof(buff)-1, 0);
	if(ret <= 0)
	{
		printf("read error.\n");
	}else
	{
		printf("recv msg: %s\n", buff);
	}

	close(socketfd);
	return 0;
}

int app_udp_send_data(char *serverIP, int serverPort, uint8_t *send_data, uint32_t send_len)
{
	int socketfd;
	int length;
	fd_set fds;
	int ret;
	struct timeval timeout = {30,0};
	
	struct sockaddr_in socketaddr;
	uint8_t buff[MAX_BUFF_LEN] = {0};
	memset(&socketaddr, 0, sizeof(socketaddr));
	socketaddr.sin_family = AF_INET;
	socketaddr.sin_port = htons(serverPort);
	socketaddr.sin_addr.s_addr = inet_addr(serverIP);

	if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("create socket fail\n");
		return -1;
	}

	if (connect(socketfd, (struct sockaddr*) &socketaddr, sizeof(socketaddr)) == -1)
	{
		printf("Connect to server fail\n");
		close(socketfd);
	}

	length = send(socketfd, send_data, send_len, 0);
	if (length < 0)
	{
		printf("send fail.\n");
		close(socketfd);
		return -1;
	}
	else if(length == 0)
	{
		printf("server socket close\n");
		close(socketfd);
		return -1;
	}
	else 
	{
		printf("send success\n");
	}

	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	memset(buff, 0, sizeof(buff));
	ret = recv(socketfd, buff, sizeof(buff)-1, 0);
	if(ret <= 0)
	{
		printf("read error.\n");
	}else
	{
		printf("recv msg: %s\n", buff);
	}

	close(socketfd);
	return 0;
}

