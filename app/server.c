#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>

#include <udtc.h>
void* recvdata(void* usocket);

#define DATA_LEN 4096
int main() 
{
    struct addrinfo hints;
    struct addrinfo* res;

    memset(&hints, 0, sizeof(struct addrinfo));
   
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char *listen_port = "9000";

    if (udt_startup() != 0)
    {
        printf("udt : startup failed:%s\n", udt_getlasterror_desc());
        return -1;
    }
   
   
    if (0 != getaddrinfo(NULL, listen_port, &hints, &res))
    {
        printf("illegal port number or port is busy\n");
        return -1;
    }
    
    UDTSOCKET serv = udt_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (udt_bind(serv, res->ai_addr, res->ai_addrlen) != 0)
    {
        printf("failed to bind : %s", udt_getlasterror_desc());
        return -1;
    }

    freeaddrinfo(res);

    if (udt_listen(serv, 10) != 0)
    {
        printf("listen failed: %s\n", udt_getlasterror_desc());
        return -1;
    }
    
    printf("server is ready at port : %s\n", listen_port);

    struct sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);
    
    UDTSOCKET recver;

    //while(1)
    //{
        if (UDT_INVALID_SOCK == (recver = udt_accept(serv, (struct sockaddr*)&clientaddr, &addrlen)))
        {
            printf("accept:%s", udt_getlasterror_desc());
            return -1;
        }
        char clienthost[NI_MAXHOST];
        char clientservice[NI_MAXSERV];
        getnameinfo((struct sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
        printf("new connection: %s : %s\n", clienthost, clientservice);

        //pthread_t rcvthread;
        //pthread_create(&rcvthread, NULL, recvdata, &recver);
        //pthread_detach(rcvthread);
        recvdata(&recver);
    //}
    if (udt_cleanup() != 0) {
        printf("cleanup:%s", udt_getlasterror_desc());
    }
}

void* recvdata(void* usocket)
{
    UDTSOCKET recver = *(UDTSOCKET*)usocket;
    char* data = malloc(sizeof(char) * DATA_LEN);
    while (1)
    {
        //udt_getsockopt(recver, 0, UDT_UDT_RCVDATA, &rcv_size, &var_size);
        if (UDT_ERROR == udt_recv(recver, data, DATA_LEN, 0))
        {
            printf("recv:%s\n", udt_getlasterror_desc());
            break;
        }
    }
    free(data);
    udt_close(recver);
    return NULL;
}
