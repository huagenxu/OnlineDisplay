/****************************************************************************
This client_data.c program is to create a socket and read the data
from server_socket. And the memory mapped file will be created
for online display
Author: H.Xu on Dec.28, 2012
Merge socket data from event_distributor on Jan. 24, 2013 by Huagen
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TMapFile.h>
#include <TSystem.h>
#include <iostream>
#include <TCanvas.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>

#include "day_1_online.h"

using namespace std;

//typedef unsigned int ems_u32;

#define SWAP_32(n) (((n & 0xff000000) >> 24) | \
    ((n & 0x00ff0000) >>  8) | \
    ((n & 0x0000ff00) <<  8) | \
    ((n & 0x000000ff) << 24))

struct event_buffer {
    size_t size;
    char* data;
    int number_of_users;
};

struct input_event_buffer {
    struct event_buffer* event_buffer;
    ems_u32 head[2];
    size_t position;
    int valid;
};

struct sock {
    struct event_buffer* event_buffer;
    size_t position;
    struct sockaddr_in address;
    int p;
};

typedef enum {intype_connect, intype_accept, intype_stdin} intypes;
typedef enum {swaptype_check, swaptype_always, swaptype_never} swaptypes;
struct sock** socks;
int num_socks, max_socks;
int quiet, old, close_old;
intypes intype;
swaptypes swaptype;
char *inname, *outname;
int inport, outport;
unsigned int inhostaddr;

/******************************************************************************/
static void
sigpipe(int num)
{
    if (!quiet) printf("sigpipe received\n");
}
/******************************************************************************/
static const char*
conn2string(struct sockaddr_in* addr)
{
    static char s[1024];
    snprintf(s, 1024, "%s:%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    return s;
}

/*****************************************************************************/
static struct event_buffer*
create_event_buffer(size_t size)
{
    struct event_buffer* buf;
    if ((buf=(struct event_buffer*)malloc(sizeof(struct event_buffer)))==0) {
        printf("In create_event_buffer():malloc struct event_buffer: %s\n", strerror(errno));
        return 0;
    }
    if ((buf->data=(char*)malloc(size))==0) {
        printf("In create_event_buffer():malloc data (%llu byte): %s\n", (unsigned long long)size,
                strerror(errno));
        free(buf);
        return 0;
    }
    buf->number_of_users=0;
    return buf;
}

/******************************************************************************/
static void
clear_new_event(struct input_event_buffer* buf)
{
    if (buf->event_buffer && !buf->event_buffer->number_of_users) {
        free(buf->event_buffer->data);
        free(buf->event_buffer);
    }
    buf->event_buffer=0;
    buf->position=0;
    buf->valid=0;
}
/******************************************************************************/
static void
delete_old_event(ems_u32* buf)
{  // cout<<"In delete_old_event: free event buffer!"<<endl;
    if (buf){
        free(buf);
       // free(buf->event_buffer);
    }
	//	buf = 0;
//	cout<<"Ptr buf is 0!"<<endl;

}
/******************************************************************************/
static int
create_connecting_socket (unsigned int host, int port)
{
    int sock, res;
    struct sockaddr_in address;

    if ((sock=socket(AF_INET, SOCK_STREAM, 0))<0) {
        printf("create connecting_socket: %s\n", strerror(errno));
        return -1;
    }

    if (fcntl(sock, F_SETFL, O_NDELAY)==-1) {
        printf("fcntl(O_NDELAY) connecting_socket: %s\n", strerror(errno));
        return -1;
    }

    bzero((void*)&address, sizeof(struct sockaddr_in));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    address.sin_addr.s_addr=host;

    res=connect(sock, (struct sockaddr*)&address, sizeof(struct sockaddr_in));
    if ((res==0) || (errno==EINPROGRESS)) return sock;

    printf("connect inputsocket: %s\n", strerror(errno));
    close(sock);
    return -1;
}

/******************************************************************************/
static int
is_connected(int sock)
{
    int opt, res;
    unsigned int len;

    len=sizeof(opt);
    if ((res=getsockopt(sock, SOL_SOCKET, SO_ERROR, &opt, &len))<0) {
        printf("getsockopt(SO_ERROR): %s\n", strerror(errno));
        return -1;
    }
    if (opt==0) {
        if (!quiet) printf("insocket connected.\n");
        return 1;
    }
    if (!quiet) printf("connect insocket: %s\n", strerror(opt));
    return -1;
}


/******************************************************************************/
static int
do_accept(int p, struct sockaddr_in* address, char *dir)
{
    unsigned int len;
    int ns;

    len=sizeof(struct sockaddr_in);
    ns=accept(p, (struct sockaddr*)address, &len);
    if (ns<0) {
        printf("accept: %s\n", strerror(errno));
        return -1;
    }
    if (!quiet) printf("%s accepted from %s\n", dir, conn2string(address));
    if (fcntl(ns, F_SETFL, O_NDELAY)==-1) {
        printf("fcntl O_NDELAY: %s\n", strerror(errno));
        close(ns);
        return -1;
    }
    return ns;
}

/******************************************************************************/
static int
do_read(int pin, struct input_event_buffer* buf)
{
    int res;
    size_t n=0;
    int HEADERSIZE=old?sizeof(ems_u32):2*sizeof(ems_u32);

    if (!buf->event_buffer) { /* read header and prepare buffer */
        res=read(pin, ((char*)buf->head)+buf->position, HEADERSIZE-buf->position);
        if (res<0) {
            if (errno==EINTR) return 0; /* try later */
            if (!quiet) printf("In do_read():read header: %s\n", strerror(errno));
            return -1; /* error, don't try again */
        } else if (res==0) {
            if (!quiet) printf("In do_read():read header: %s\n", strerror(EPIPE));
            return -1; /* error, don't try again */
        }
        buf->position+=res;
        if (buf->position<HEADERSIZE) return 0; /* try later */

        if (old) {
            switch (swaptype) {
            case swaptype_check:
                n=(buf->head[0]&0xffff0000)?
                        SWAP_32(buf->head[0]):
                        buf->head[0];
                break;
            case swaptype_always:
                n=SWAP_32(buf->head[0]);
                break;
            case swaptype_never:
                n=buf->head[0];
                break;
            }
        } else {
			for(int i=0;i<2;i++)
			{
		//	printf("buf->head[ %d ] is 0x%08x \n",i, buf->head[i]);
			}

            switch (buf->head[1]) {
            case 0x12345678: n=buf->head[0]; break;
            case 0x78563412: n=SWAP_32(buf->head[0]); break;
            default:
                printf("In do_read():unknown endien 0x%08x\n", buf->head[1]);
                return -1;
            }
        }

        n=(n+1)*sizeof(ems_u32);
        if ((buf->event_buffer=create_event_buffer(n))==0) {
            return -1;
        }
 	//	cout<<"In do_read():the cluster size n is "<<n<<endl;
        buf->event_buffer->size=n;
    //    cout<<"In do_read():create buffer size"<<endl;
        buf->position=HEADERSIZE;
	//	cout<<"In do_read():buf-position = HEADERSIZE"<<endl;
        bcopy((char*)buf->head, buf->event_buffer->data, HEADERSIZE);
    }

    /* header is complete, read the data */
 //	cout<<"In do_read():test for event header"<<endl;
    res=read(pin, buf->event_buffer->data+buf->position,
    buf->event_buffer->size-buf->position);
 //   cout<<"In do_read():test for event header2"<<endl;

    if (res<0) {
        if ((errno==EINTR)||(errno==EWOULDBLOCK)) return 0; /* try later */
        if (!quiet) printf("read data: %s\n", strerror(errno));
        return -1; /* error, don't try again */
    } else if (res==0) {
        if (!quiet) printf("In do_read():read data returns 0\n");
        return -1;
    }
  //  cout<<"In do_read():res is "<<res<<endl;
    buf->position+=res;
    if (buf->position<buf->event_buffer->size) return 0; /* try later */
    buf->valid=1;
    return 0;
}


/****************************************************************************/
void error(const char* msg)
{
	perror(msg);
	exit(0);
}

/***************************************************************************/
int decoding(ems_u32*,int);


/***************************************************************************/
int main(int argc, char *argv[])
{
		TApplication app("app",0,0);
		TMapFile::SetMapAddress(0xb46a5000);
                TFile *file;
		//TString path = "/home/huage/code/cluster_decoding/DayOneOnline";
		mfile = TMapFile::Create("histprod.map","RECREATE",10000000,"DayOneOnline");

   		h217 = new TH1F("ADC2_ch17","Si_12_rear",nbinsx_madc, xlow_madc, xup_madc);
   		h218 = new TH1F("ADC2_ch18","Si_15_rear",nbinsx_madc, xlow_madc, xup_madc);
   		h219 = new TH1F("ADC2_ch19","Ge_5mm_rear",nbinsx_madc, xlow_madc, xup_madc);
   		h220 = new TH1F("ADC2_ch20","Ge_11mm_rear",nbinsx_madc, xlow_madc, xup_madc);

		Si_13_hits = new TH2S("Si_12_strips","Spectrum on Si_12",49,0.,49., 1024,0.,8192.);
		Si_15_hits = new TH2S("Si_15_strips","Spectrum on Si_15",65,0.,65., 1024,0.,8192.);
		Ge_5mm_hits = new TH2S("Ge_5mm_strips","Spectrum on Ge_5mm",33,0.,33., 1024,0.,8192.);
		Ge_11mm_hits = new TH2S("Ge_11mm_strips","Spectrum on Ge_11mm",33,0.,33., 1024,0.,8192.);

 		Scint_qhits = new TH2S("ScintillatorsQ","Scint_QDC",33,0,33,1024,0,4096);
                Scint_thits = new TH2S("ScintillatorsT","Scint_TDC",33,0,33,4096,0,75536);

/*
   		h701 = new TH1F("QDC1_ch1","Big_Up",nbinsx_mqdc, xlow_mqdc, xup_mqdc);
   		h702 = new TH1F("QDC1_ch2","Big_Down",nbinsx_mqdc, xlow_mqdc, xup_mqdc);
   		h703 = new TH1F("QDC1_ch3","Small_Up1",nbinsx_mqdc, xlow_mqdc, xup_mqdc);
   		h705 = new TH1F("QDC1_ch5","Small_Down1",nbinsx_mqdc, xlow_mqdc, xup_mqdc);
   		h707 = new TH1F("QDC1_ch7","Small_Left1",nbinsx_mqdc, xlow_mqdc, xup_mqdc);
   		h709 = new TH1F("QDC1_ch9","Small_Right1",nbinsx_mqdc, xlow_mqdc, xup_mqdc);
*/

		mfile->Print();

	//create client socket
		int sockfd, portno,inport,outport, n, n1, n2, n3;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		char* buffer = (char*)malloc(8);
		char buff[256];
		printf("the size of int is %d\n",sizeof(int));

		if(argc < 3)
		{
		fprintf(stderr,"usage %s hostname port", argv[0]);
		exit(0);
		}

		portno = atoi(argv[2]);
		inport = portno;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd < 0)
		error("ERROR opening socket");

		server = gethostbyname(argv[1]);
		if (server == NULL)
		{
		fprintf(stderr,"ERROR, no such host");
		exit(0);
		}


		bzero((char*)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(portno);
		bcopy((char *)server->h_addr,
		      (char *)&serv_addr.sin_addr.s_addr,
		      server->h_length);

        struct hostent *host;
        char hostname[1024];
        intype=intype_connect;

        if (server!=0) {
            inhostaddr=*(unsigned int*)(server->h_addr_list[0]);
        } else {
           // inhostaddr=inet_addr(hostname);
            if (inhostaddr==-1) {
                printf("unknown host: %s\n", hostname);
                return 1;
              }
        }

	//preparation for reading data
		int insock_l=-1, outsock_l=3;
		char *p;
		quiet = 0;
		intype=intype_connect;

		printf("%s:%s for input;\n",argv[1],argv[2]);

		signal (SIGPIPE, sigpipe);

		struct input_event_buffer ibs, evtbuf;
		fd_set readfds, writefds;
    	int insock;
    	struct timeval last;

    	last.tv_sec=0;
    	ibs.event_buffer=0;
//		evtbuf.event_buffer = 0;
    	clear_new_event(&ibs);

    	num_socks=0; max_socks=0; socks=0;
		insock = -1;

		ems_u32* event = 0;// (ems_u32*)malloc(8);

		int count = 0;

	while(1){
	cout<<endl;

//	cout<<endl<<"Starting loop for readout data from socket and decoding"<<endl;

		int nfds, idx, need_data, res;
        struct timeval to, *timeout;

        timeout=0;
        nfds=-1;
        FD_ZERO(&readfds);

		need_data=0;

          //  if (evtbuf.event_buffer) {
			if(event){
					int size = event[0];
			//		cout<<"The size of event is (event[0]) "<<size<<endl;
					decoding(event,size);
						/*
						for(int i=0;i<8;i++)
						{
				//		printf("event[ %d ] is 0x%08x \n",i, event[i]);
						}
						for(int i=230;i<269;i++)
						{
				//		printf("event[ %d ] is 0x%08x \n",i, event[i]);
						}
						*/
			delete_old_event(event);
			event = 0;

			//	if(!event) printf("Decoding has been done, event buffer was dumped\n");
				//continue;
            } else {
                need_data=1;
			//	printf("Need data! \n");
            }


        if (need_data) {
				FD_SET(insock, &readfds);
            if (insock>=0) {
					//	printf("insock>=0\n");
//                FD_SET(insock, &readfds);
                if (insock>nfds) nfds=insock;
            } else if (intype==intype_connect) {
						//	printf("intye==intype_connect\n");
                if (insock_l<0) {
                    struct timeval now;
                    gettimeofday(&now, 0);
                    if (now.tv_sec-last.tv_sec>30) {
                        last=now;

						if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
						error("ERROR connecting");
                        insock_l=sockfd; //create_connecting_socket(inhostaddr, inport);

						//	printf("waiting time >30, insock_l = sockfd = %d\n",insock_l);
                    } else {
                        to.tv_sec=10; to.tv_usec=0; timeout=&to;
                    }
                }
                if (insock_l>=0) {
				//	printf("if insock_l>=0, FD_SET(insock_l, &writefds),insock_l = %d \n", insock_l);
                    if (insock_l>nfds) nfds=insock_l;
                }
            }
        }



		//	cout<<"insock_l is "<<insock_l<<endl;
        /* new connections */
        if (insock_l>=0) {
			if (intype==intype_connect) {
                    switch (is_connected(sockfd)) {
                    case 1: insock=insock_l; insock_l=-1; break;
                    case -1: close(insock_l); insock_l=-1;break;
                    /* default: still in progress */
                    }
            }
        }



        /* input? */
        if ((insock>=0) && FD_ISSET(insock, &readfds)) {
	//	printf("In main loop: insock>=0, res = do_read(insock, &ibs)\n");
            res=do_read(insock, &ibs);
	//		cout<<"In main loop: res of do_read(insock, &ibs) is "<<res<<endl;
            if (res<0) {
                clear_new_event(&ibs);
			}else {
                if (ibs.valid) {
		//	for(int i=0;i<160;i++)
		//	{	printf("ibs.event_buffer->data[ %d ] is 0x%08x \n",i, ibs.event_buffer->data[i]);	}
			//printf("In main_loop:res = do_read(insock, &ibs)>0, ibs valid\n");

                      //  if (!evtbuf.event_buffer) {
						if (!event){
							int n =0;
						// convert char data to ems_u32 format
							n = ((ibs.event_buffer->data[3]&0x000000ff)<<24)
							   +((ibs.event_buffer->data[2]&0x000000ff)<<16)
							   +((ibs.event_buffer->data[1]&0x000000ff)<<8)
							   +(ibs.event_buffer->data[0]&0x000000ff);

							printf("the size of data is %d\n",n);

						//	n=(n+1)*4*sizeof(ems_u32); //comment out on 2015 10 24 by Huagen, bug of segmentation violation

        				//	if ((evtbuf.event_buffer=create_event_buffer(n))==0) {
						//	printf("Failed to create buffer for evtbuf!\n");
				         //   return -1;
							//} else {cout<<"The ibs data[0] is 0"<<endl;continue;}
        				//}

							event = (ems_u32*)realloc(event, (n+1)*4);


							for(int i=0;i<(n+1)*4;i++)
							{
							//printf("n is %d ,loop data %d \n",(n+1)*4,i);
							if((i+1)%4 == 0)  event[(i+1)/4-1]=((ibs.event_buffer->data[i]&0x000000ff)<<24)
											+((ibs.event_buffer->data[i-1]&0x000000ff)<<16)
											+((ibs.event_buffer->data[i-2]&0x000000ff)<<8)
											+(ibs.event_buffer->data[i-3]&0x000000ff);
							}

		//					for (int i=0;i<size_n+1;i++) event[i] = evtbuf.event_buffer->data[i];
							for(int i=0;i<10;i++)
							{
								printf("event[ %d ] is 0x%08x \n",i, event[i]);
							}

                        }
                    clear_new_event(&ibs);
                }
            }
        }

		if(++count%100==0) printf("The loop number is %d\n", count);
	//	gSystem->Sleep(1000);
	}
	return 0;
}

/*****************************************************************************************************************/
int decoding(ems_u32 *buffer, int size)
{
	int ii =  0;
	//loop one cluster data with datasize of "size"
	for (int n=0;n<size+1;n++)
	{//	cout<<"The data size is "<<size<<endl;
        //   printf("Buf [%d] is 0x%08x \n", n, buffer[n]);
		int temp_ID=0;

/**********************************************************
*ADC data
**********************************************************/

           if((buffer[n]&0xFFF00000) == 0x40100000 || (buffer[n]&0xFFF00000) == 0x40200000 || (buffer[n]&0xFFF00000) == 0x40300000)
		{
	//	cout<<"buffer["<<n<<"] is event header"<<endl;
		int adcres = buffer[n]>>12 & 0x7;		//printf("the ADC resolution is %d \n",adcres);
		int nrwords = buffer[n]&0xfff;		//printf("the following words are %d \n",nrwords);
                int id = (buffer[n]>>16)&0xff;		//printf("the XDC id is %d \n",id);

		temp_ID=id;

		if(id<22){// identify ADC data by module ID
		id=id-15; //id number for the array
                for(int i=1;i<=nrwords;i++)
		{
			if((buffer[n+i]&0xf4E00000)==0x04000000)
			{
			int ch= (buffer[n+i]>>16) & 0x1F;    //  cout<<" ch = "<<ch<<endl;
			data1[id][ch] = (buffer[n+i]) & 0x1FFF;   //  cout<<" buffer["<<n+i<<"] is "<<buffer[ch]<<" "<<endl;
		//	printf("data[%d][ %d ] is %d \n",id, ch, data1[id][ch]);
		//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);
			} else continue;
		} //loop one ADC data

	        n += nrwords;

		}
		else if(id>31&&id<33){//Identify QDC data by module ID

		id=id-25; //id number for the array
                for(int i=1;i<=nrwords;i++)
		{
			if((buffer[n+i]&0xf4E00000)==0x04000000)
			{
			int ch= (buffer[n+i]>>16) & 0x1F;    //  cout<<" ch1 = "<<ch<<endl;
			data1[id][ch] = (buffer[n+i]) & 0xFFF;   //  cout<<" buffer["<<n+i<<"] is "<<buffer[ch1]<<" "<<endl;
		//	printf("data[%d][ %d ] is %d \n",id1, ch1, data1[id1][ch1]);
		//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);
			} else continue;
		} //loop one QDC data

	        n += nrwords;

		}
		else if(id>46&&id<50){
			id=id-40; //id number for the data array
		//cout<<"The TDC data"<<endl;

                for(int i=1;i<=nrwords;i++)
		{
			if((buffer[n+i]&0xf4C00000)==0x04000000)
			{
			int ch= (buffer[n+i]>>16) & 0x1F;     // cout<<" ch2 = "<<ch<<"the id is "<<id<<endl;
			data1[id][ch] = (buffer[n+i]) & 0xFFFF;  //   cout<<" buffer["<<n+i<<"] is "<<buffer[ch]<<" "<<endl;
		//	printf("data[%d][ %d ] is %d \n",id2, ch2, data1[id2][ch2]);
		//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);
			} else continue;
		} //loop one ADC data

	        n += nrwords;

		} else continue;


		} //ADC/QDC/TDC data
	else if((buffer[n]&0xFFF00000) == 0x40000000)
		{
	//	cout<<"buffer["<<n<<"] is event header"<<endl;
		int adcres = buffer[n]>>12 & 0x7;		//printf("the ADC resolution is %d \n",adcres);
		int nrwords = buffer[n]&0xfff;		//printf("the following words are %d \n",nrwords);
                int id = (buffer[n]>>16)&0xff;		//printf("the XDC id is %d \n",id);

		temp_ID=id;

		if(id>0&&id<7){// identify ADC data by module ID
		//id=id-15; //id number for the array
    for(int i=1;i<=nrwords;i++)
		{
			if((buffer[n+i]&0xf4E00000)==0x04000000)
			{
			int ch= (buffer[n+i]>>16) & 0x1F;    //  cout<<" ch = "<<ch<<endl;
			data1[id][ch] = (buffer[n+i]) & 0x1FFF;   //  cout<<" buffer["<<n+i<<"] is "<<buffer[ch]<<" "<<endl;
		//	printf("data[%d][ %d ] is %d \n",id, ch, data1[id][ch]);
		//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);
			} else continue;
		} //loop one ADC data

	        n += nrwords;

          if(id==1){
                    for(int nr=0;nr<32;nr++)
                    Si_13_hits->Fill(Si_13_strip[nr],data1[2][nr],1);
          }

          if(id==2){
                    h217->Fill(data1[2][16]);
                    h218->Fill(data1[2][17]);
                    h219->Fill(data1[2][18]);
                    h220->Fill(data1[2][19]);
                    for(int nr=0;nr>32&&nr<48;nr++)
                    Si_13_hits->Fill(Si_13_strip[nr],data1[2][nr-32],1);
          }

          if(id==3){
                    for(int nr=0;nr<32;nr++)
                    Si_15_hits->Fill(Si_15_strip[nr],data1[3][nr],1);
          }
          if(id==4){
                    for(int nr=0;nr>32&&nr<64;nr++)
                    Si_15_hits->Fill(Si_15_strip[nr],data1[4][nr-32],1);

          }
          if(id==5){
                    for(int nr=0;nr<32;nr++)
                    Ge_5mm_hits->Fill(Ge_5mm_strip[nr],data1[5][nr],1);

          }
          if(id==6){
                    for(int nr=0;nr<32;nr++)
                    Ge_11mm_hits->Fill(Ge_11mm_strip[nr],data1[6][nr],1);
          }

		}
		else if(id==7){//Identify QDC data by module ID

		//id=id-25; //id number for the array
                for(int i=1;i<=nrwords;i++)
		{
			if((buffer[n+i]&0xf4E00000)==0x04000000)
			{
			int ch= (buffer[n+i]>>16) & 0x1F;    //  cout<<" ch1 = "<<ch<<endl;
			data1[id][ch] = (buffer[n+i]) & 0xFFF;   //  cout<<" buffer["<<n+i<<"] is "<<buffer[ch1]<<" "<<endl;
		//	printf("data[%d][ %d ] is %d \n",id1, ch1, data1[id1][ch1]);
		//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);
			} else continue;
		} //loop one QDC data

	        n += nrwords;

		}
		else if(id==8){
		//	id=id-40; //id number for the data array
		//cout<<"The TDC data"<<endl;

                for(int i=1;i<=nrwords;i++)
		{
			if((buffer[n+i]&0xf4C00000)==0x04000000)
			{
			int ch= (buffer[n+i]>>16) & 0x1F;     // cout<<" ch2 = "<<ch<<"the id is "<<id<<endl;
			data1[id][ch] = (buffer[n+i]) & 0xFFFF;  //   cout<<" buffer["<<n+i<<"] is "<<buffer[ch]<<" "<<endl;
		//	printf("data[%d][ %d ] is %d \n",id2, ch2, data1[id2][ch2]);
		//	printf("data[%d][ %d ] is 0x%08x \n",id, ch, data1[id][ch]);
			} else continue;
		} //loop one ADC data

	        n += nrwords;

		} else continue;



		} //ADC/QDC/TDC data, module with HW ID


/**************************************************************************************************
*Scaler data
***************************************************************************************************/


		if((buffer[n]==1)&&(buffer[n+1]==32)){
			//else if((buffer[n]&0xFFFFFFFF)==0x0000000a){
			//	 cout<<"scaler events!"<<endl;

				int scaler_length = buf[n+1]&0xFFFFFFFF;

				//for(int k=1;k<=scaler_length;k++)
					//{//scaler_temp[k-1]=buf[n+1+k]&0xFFFFFFFF;
					// scaler[k] = scaler_temp[k];
						//if(k<6)cout<<"The scaler_temp["<<k<<"] is "<<scaler_temp[k]<<endl;
					//}

				// n += scaler_length;
		} //loop scaler data

//	}while(n++<size);//test the cluster loop

	}//loop one cluster data with size

	//cout<<"Finishing data decoding before filling histograms"<<endl;

/***********************************************************************************************************************************/
/* Fill histogram for online display                                                                                               */
/***********************************************************************************************************************************/

				//if(temp_ID==48){

  						//        h217->Fill(data1[2][16]);
						  //        h218->Fill(data1[2][17]);
					   //         h219->Fill(data1[2][18]);
					   //         h220->Fill(data1[2][19]);
							//printf("data[1][ 16 ] is %d \n", data1[1][16]);
							//printf("data[1][ 17 ] is %d \n", data1[1][17]);
							//printf("data[1][ 18 ] is %d \n", data1[1][18]);
							//printf("data[1][ 19 ] is %d \n", data1[1][19]);

					//	h701->Fill(data1[7][1]);
					//	h702->Fill(data1[7][2]);
					//	h703->Fill(data1[7][3]);
					//	h705->Fill(data1[7][5]);
					//	h707->Fill(data1[7][7]);
					//	h709->Fill(data1[7][9]);

	/*					for(int i=0; i<33;i++)
						{
 							Scint_qhits->Fill(scint[i],data1[7][i],1);
							Scint_thits->Fill(scint[i],data1[8][i],1);
			 			}

						for(int nr=0; nr<64;nr++)
						{
						if(nr<32) {
						Ge_5mm_hits->Fill(Ge_5mm_strip[nr],data1[5][nr],1);
            Ge_11mm_hits->Fill(Ge_11mm_strip[nr],data1[6][nr],1);
						Si_13_hits->Fill(Si_13_strip[nr],data1[1][nr],1);
						Si_15_hits->Fill(Si_15_strip[nr],data1[3][nr],1);
						}
						else if(nr>31&&nr<48) {
						Si_13_hits->Fill(Si_13_strip[nr],data1[2][nr-32],1);
						Si_15_hits->Fill(Si_15_strip[nr],data1[4][nr-32],1);
						}
						else Si_15_hits->Fill(Si_15_strip[nr],data1[4][nr-32],1);

						}
			cout<<"Fill the histogram!"<<endl<<endl; //Filling only once per cluster data
*/
		//	}//loop scaler data which means one event complete
		//	}//data structure complete, fill histogram



		//



			if(!(ii%100))
			{
				mfile->Update(); // update all objects in  shared memroy
				if(!ii) mfile->ls();
			}
			ii++;
		//	gSystem->Sleep(10);

			if(h217->GetEntries()>30000||h218->GetEntries()>30000||h219->GetEntries()>30000||h220->GetEntries()>30000)
				{
				h217->Reset();
				h218->Reset();
				h219->Reset();
				h220->Reset();

				Ge_5mm_hits->Reset();
				Ge_11mm_hits->Reset();
				Si_13_hits->Reset();
				Si_15_hits->Reset();

				Scint_qhits->Reset();
				Scint_thits->Reset();

				//h701->Reset();
				//h702->Reset();
				//h703->Reset();
				//h705->Reset();
				//h707->Reset();
				//h709->Reset();

				}

      //  cout<<"the Entries of histogram is "<<h217->GetEntries()<< endl;

}
