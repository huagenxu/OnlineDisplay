/******************************************************************
This demo client program is to create a client which 
connects to server for data sending and receiving
Author H. Xu on Dec.27, 2012
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

void error(const char*);

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

/* it defines a host computer on the internet
	struct hostent
	{
		char *h_name; //official name of host
		char **h_aliases; //alias list
		int h_addrtype; //host address type
		int h_length; //length of address
		char **h_addr_list; //list of addresses from name server
		#define h_addr h_addr_list[0] //address, for bacward compatibility
	}
*/	

	if(argc < 3)
	{
		fprintf(stderr,"usage %s hostname port", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host");
		exit(0);	
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		  (char *)&serv_addr.sin_addr.s_addr, 
		  server->h_length);
	serv_addr.sin_port = htons(portno);

	//now connect the socket to server
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
		error("ERROR connecting");	
/*
	printf("Please enter the message: ");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	n = write(sockfd, buffer, strlen(buffer));
	if(n < 0)
		error("ERROR writing to socket");
*/
	bzero(buffer, 256);
	n = read(sockfd, buffer, 255);
	if(n < 0)
		error("ERROR reading from socket");
	printf("%s \n", buffer);

	return 0;

}

void error(const char* msg)
{
	perror(msg);
	exit(0);
}



