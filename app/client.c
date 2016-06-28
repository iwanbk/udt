#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#include <udtc.h>

#define N_SEND      (1000 * 100)
#define DATA_LEN    4096
int main(int argc, char* argv[])
{
    if ((3 != argc) || (0 == atoi(argv[2])))
    {
        printf("usage: client server_ip server_port\n");
        return -1;
    }
   
   
    if (udt_startup() != 0)
    {
        printf("udt : startup failed:%s\n", udt_getlasterror_desc());
        return -1;
    }
   
   
    struct addrinfo hints, *local, *peer;

   
    memset(&hints, 0, sizeof(struct addrinfo));

   
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;
   
    if (0 != getaddrinfo(NULL, argv[2], &hints, &local))
    {
        printf("incorrect network address.\n");
        return -1;
    }

    
    UDTSOCKET client = udt_socket(local->ai_family, local->ai_socktype, local->ai_protocol);

   // UDT Options
   //UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
   //UDT::setsockopt(client, 0, UDT_MSS, new int(9000), sizeof(int));
   //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10000000), sizeof(int));
   //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(10000000), sizeof(int));
   //UDT::setsockopt(client, 0, UDT_MAXBW, new int64_t(12500000), sizeof(int));

   // for rendezvous connection, enable the code below
   /*
   UDT::setsockopt(client, 0, UDT_RENDEZVOUS, new bool(true), sizeof(bool));
   if (UDT::ERROR == UDT::bind(client, local->ai_addr, local->ai_addrlen))
   {
      cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }
   */


    freeaddrinfo(local);
   
    if (0 != getaddrinfo(argv[1], argv[2], &hints, &peer))
    {
        printf("incorrect server/peer address. %s:%s\n", argv[1], argv[2]);
        return -1;
   
    }

   
    // connect to the server, implict bind
    if (UDT_ERROR == udt_connect(client, peer->ai_addr, peer->ai_addrlen))
    {
        printf("connect: %s\n", udt_getlasterror_desc());
        return 0;
    }

 
    freeaddrinfo(peer);
   
    // using CC method
    //CUDPBlast* cchandle = NULL;
    //int temp;
   
    //UDT::getsockopt(client, 0, UDT_CC, &cchandle, &temp);
   
    //if (NULL != cchandle)
    //   cchandle->setRate(500);
    
    char* data = malloc(sizeof(char) * DATA_LEN);
    memset(data, 'a', DATA_LEN);

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    for (int i = 0; i < N_SEND; i ++)
    {
        //printf("i = %d\n", i);
        if (UDT_ERROR == udt_send(client, data, DATA_LEN, 0))
        {
            printf("send:%s\n", udt_getlasterror_desc());
            break;
        }
    }

    gettimeofday(&t2, NULL);
    int elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("finished to write data in %d milliseconds\n", elapsedTime);
    
    udt_close(client);
    free(data);

    if (udt_cleanup() != 0) {
        printf("cleanup:%s", udt_getlasterror_desc());
    }
    return 0;
}

