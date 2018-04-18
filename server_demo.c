/**************************************************************
This demo program is to create a server for socket with TCP
protocol. The data flow format is STREAM.
Author H.XU on Dec.27, 2012
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

void error(const char *);
void dostuff(int);

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, n;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
/*
	struct sockaddr_in
	{
		short sin_family; //must be AF_INET
		u_short sin_port; //port number
		struct in_addr sin_addr;
		char sin_zero[8]; //not used, must be zero
	}
*/

	if(argc<2)
	{
		fprintf(stderr,"ERROR, no port provided");
		exit(1);
	}

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
		error("ERROR opening socket");

	bzero((char*)&serv_addr, sizeof(serv_addr));
/*  or using the memset function to reset the address memory
	memset(&serv_addr,'0',sizeof(serv_addr));
*/	
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	error("ERROR on binding socket");

	listen(sockfd,5);

	clilen = sizeof(cli_addr);
	while(1) {
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if(newsockfd < 0)
		error("ERROR on accept");
	int pid = fork();
	if(pid < 0) error("ERROR on fork");
	if(pid == 0) 
		{
			close(sockfd);
			dostuff(newsockfd);
			exit(0);		
		} else 
			close(newsockfd);


	} //end of while

	bzero(buffer, 256);
/*	n = read(newsockfd, buffer,255);
	if(n < 0) error("ERROR reading from socket");
	printf("Here is the message: %s \n",buffer);
*/	
//	char message[] = {"I got your message"}
	n = write(newsockfd,"I got your message",18);
	if(n < 0) error("ERROR writing to socket");

	close(newsockfd);
	close(sockfd);

	return 0;

}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void dostuff(int sock)
{
	int n;
	char buff[256];

//	bzero(buff, 256);
	memset(buff, 0,sizeof(buff));
//	n = read(sock, buff, 255);
//	if(n < 0) error("ERROR reading from socket");
//	printf("Here is the message: %s \n",buff);
	n = write(sock, "I got your message", 18);
	if(n < 0) error("ERROR writing to socket");

}



