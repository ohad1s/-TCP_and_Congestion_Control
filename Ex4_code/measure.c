/*
    TCP/IP-server
*/

#include<stdio.h>
#include <sys/time.h>

#if defined _WIN32
#include<winsock2.h>   //winsock2 should be before windows
#pragma comment(lib,"ws2_32.lib")
#else
// Linux and other UNIXes
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#endif
#define SIZE 1048576
#define SERVER_PORT 5060  //The port that the server listens

int main()
{
    char server_reply[SIZE];
    int size_recv;
    int total_size;

    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket

    // Open the listening (server) socket
    int listeningSocket = -1;

    if((listeningSocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d", errno);
    }

    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.

    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse,sizeof(int)) < 0)
    {
        printf("setsockopt() failed with error code : %d" , errno);
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order

    // Bind the socket to the port with any IP at this port
    if (bind(listeningSocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" ,errno);
        // TODO: close the socket
        close(listeningSocket);

        return -1;
    }

    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    if (listen(listeningSocket, 500) == -1) //500 is a Maximum size of queue connection requests
        //number of concurrent connections
    {
        printf("listen() failed with error code : %d",errno);
        // TODO: close the socket
        close(listeningSocket);

        return -1;
    }


    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");

    struct sockaddr_in clientAddress;  //
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1)
    {
        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (clientSocket == -1)
        {
            printf("listen failed with error code : %d",errno);
            // TODO: close the sockets
            close(listeningSocket);
            return -1;
        }
        printf("A new client connection accepted\n");

        memset(server_reply, 0, SIZE);
        total_size = 0;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        long int rt = 0;
        int counter = 0;
        while (1){

            size_recv=recv(clientSocket, server_reply, SIZE, 0);
            if (size_recv < 0) {
                printf("error number %d", errno);
                break;
            }
            total_size += size_recv;

            if(total_size > 1024 && total_size % (1024*1024 * 5) == SIZE){
                total_size = total_size % (1024*1024 * 5);
                printf("recieved 5 files \n");
                gettimeofday(&end,NULL);
                rt = (end.tv_sec * 1000000+end.tv_usec) - (start.tv_sec*1000000 +start.tv_usec);
                counter ++;
                printf("average: %f milliseconds\n",(double)(rt)/5000);
                sleep(1);
                gettimeofday(&start, NULL);
            }
            if (counter == 2 || size_recv == 0) {
                printf("connection closed\n");
                break;
            }
            memset(server_reply, 0, SIZE);
        }
        close(listeningSocket);
        return 0;
    }
}


