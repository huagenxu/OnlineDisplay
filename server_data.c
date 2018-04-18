/******************************************************************************
The server_data.c program is to create a socket which provides the data flow
for online display. The data flow is from the data file on disk. It simulates
the data flow by using socket protocol
Author: H.Xu on Dec.28, 2012
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#include <TROOT.h>
#include <TApplication.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TMapFile.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sys/stat.h>

#include "day_1_online.h"

void error(const char*);
int decoding(int);

int main(int argc, char *argv[])
{
	int listenfd, connfd, portno, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buff[256];

	if(argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided");
		exit(1);
	}
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
		error("ERROR opening socket");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0)
		error("ERROR on socket binding");

	listen(listenfd, 5);

	clilen = sizeof(cli_addr);

	connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
	if(connfd<0)
		error("ERROR on accept");	

	//open data file and read cluster format data
/*	char filename[40],filename2[50],froot[10]={'.','r','o','o','t'};
	puts("Type the file name to be decoded:");
	scanf("%s",filename);
	strcpy(filename2, filename);
	strcat(filename2, froot);
*/
	int f, res, count = 0;
//	f = open(filename, O_RDONLY);
	f = open("test.dat",O_RDONLY);
	if(f<0)
	{
		printf("Open file: %s \n", strerror(errno));
		return 1;
	}else {
		printf("Data file is open!\n");
	}


	u_int32_t *buffer0 = (u_int32_t*)malloc(8);
	u_int32_t *tempbuf = (u_int32_t*)malloc(8);

	do{	
		do{
		bzero(buff, 256);
		n = read(connfd, buff,2);
		if(n < 0) error("ERROR reading from socket");
		printf("Here is the message: %s \n",buff);
		}while(&buff[1]=="#");	

	bufsize = 2;
	
	int res = 0;
	int size;
	const int endiantest = 0x12345678;

	bzero(buf, 8);

	if((res=read(f,buf,4*2))!=4*2)
	{
		printf("In decoding(): read size: res=%d, error=%s\n",res,strerror(errno));
		return 1;
	}

	if(buf[1] != endiantest)
	{
		printf("In decoding(): buffer[1] is not 0x12345678!\n");
	}else {
		size = buf[0];
		printf("size is %d \n",size);
		if(size<8) return 1;
	}

	if(size+2>bufsize)
	{
	buf = (u_int32_t*)realloc(buf,(size+2)*4);
	buffer0 = (u_int32_t*)realloc(buffer0,(size+2)*4);
	bufsize = size + 2;	
	}
	printf("bufsize is %d\n",bufsize);

	for(int i = 0; i<size+2;i++)
	{
	buffer0[i] = buf[i];
	}

	
	n = write(connfd,buffer0,8);
//	printf("The number of %d bytes data was written to socket\n",n);
	if(n < 0) error("ERROR writing to socket");

	n = read(connfd,tempbuf,8);
	if(tempbuf[1]!=0x12345678) error("ERROR on data transfer");	

	read(f, buf+2,(size-1)*4);	

	for(int i = 0; i<buf[0];i++){
	buffer0[i] = buf[i];
	}

	for(int i=0;i<buffer0[0]&&i<8;i++)
	{
//	printf("buffer0[ %d ] is 0x%08x \n",i, buffer0[i]);	
	}

	n = write(connfd,buffer0,buf[0]*4);
	printf("The number of %d bytes data was written to socket\n",n);
	if(n < 0) error("ERROR writing to socket");

	
	do{
	bzero(buff, 255);
	n = read(connfd,buff, 2);
	if(n < 0) error("ERROR writing to socket");
	printf("The message from client is %s\n",buff);
	} while(&buff[0]=="*");

	
	} while(1); 

	close(listenfd);
	close(connfd);

	close(f);
	printf("Close the data file!\n");

	return 0;

}


void error(const char* msg)
{
	perror(msg);
	exit(0);
}

int decoding(int f)
{
	printf("In decoding(): Start decoding!\n");
	bufsize = 2;
	
	int res = 0;
	int size;
	const int endiantest = 0x12345678;

	if((res=read(f,buf,4*2))!=4*2)
	{
		printf("In decoding(): read size: res=%d, error=%s\n",res,strerror(errno));
		return 1;
	}

	if(buf[1] != endiantest)
	{
		printf("In decoding(): buffer[1] is not 0x12345678!\n");
	}else {
		size = buf[0];
		printf("size is %d \n",size);
		if(size<8) return 1;
	}

	if(size+2>bufsize)
	{
	buf = (u_int32_t*)realloc(buf,(size+2)*4);
	bufsize = size + 2;	
	}
	printf("bufsize is %d\n",bufsize);
	
	read(f, buf+2,(size-1)*4);	
}



















