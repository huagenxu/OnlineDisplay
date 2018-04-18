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


int main(int argc, char *argv[])
{
    int insock_l, outsock_l;
    char *p;

    if (readargs(argc, argv)) return 1;

    printf("inname=%s; outname=%s\n", inname, outname);

    if (strcmp(inname, "-")==0) {
        intype=intype_stdin;
    } else if ((p=index(inname, ':'))) {
        struct hostent *host;
        char hostname[1024];
        int idx=p-inname;
        intype=intype_connect;
        strncpy(hostname, inname, idx);
        hostname[idx]=0;
        if ((inport=portname2portnum(p+1))<0) return 1;
        host=gethostbyname(hostname);
        if (host!=0) {
            inhostaddr=*(unsigned int*)(host->h_addr_list[0]);
        } else {
            inhostaddr=inet_addr(hostname);
            if (inhostaddr==-1) {
                printf("unknown host: %s\n", hostname);
                return 1;
            }
        }
    } else {
        intype=intype_accept;
        if ((inport=portname2portnum(inname))<0) return 1;
    }

    if ((outport=portname2portnum(outname))<0) return 1;

    if (!quiet) {
        printf("using ");
        switch (intype) {
        case intype_stdin:
            printf("stdin");
            break;
        case intype_connect:
            printf("%d.%d.%d.%d:%d",
                    inhostaddr&0xff, (inhostaddr>>8)&0xff,
                    (inhostaddr>>16)&0xff, (inhostaddr>>24)&0xff, inport);
            break;
        case intype_accept:
            printf("port %d", inport);
            break;
        }
        printf(" for input and port %d for output\n", outport);
    }

    /*signal (SIGINT, *sigact);*/
    /*signal (SIGPIPE, SIG_IGN);*/
    signal (SIGPIPE, sigpipe);

    if (intype==intype_accept) {
        if ((insock_l=create_listening_socket(inport))<0) {
            printf("create_listening_socket input: %s\n", strerror(errno));
            return insock_l;
        }
    } else {
        insock_l=-1;
    }
    if ((outsock_l = create_listening_socket(outport))<0) {
        printf("create_listening_socket output: %s\n", strerror(errno));
        return outsock_l;
    }

    main_loop (insock_l, outsock_l);
    return 0;
}
/******************************************************************************/
/******************************************************************************/
