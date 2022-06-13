/*
	TCP/IP client
*/


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>

#if defined _WIN32

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


#endif

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE 1048576

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("Could not create socket");
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
    if (rval <= 0)
    {
        printf("inet_pton() failed");
        return -1;
    }

    // Make a connection to the server with socket SendingSocket.

    if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("connect() failed");
    }

    printf("connected to server\n");

    // Sends some data to server

    char msg[SIZE];
    FILE *fptr;
    fptr = fopen("big_data.txt","r");

    if (fptr == NULL){
        printf("Error! opening file");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    socklen_t len=sizeof(msg);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, msg, &len) != 0) {
        perror("getsockopt");
        return -1;
    }

    printf("Current: %s\n", msg); //cubic

    //send the file
    for (int i = 0; i < 5; i++) {
        size_t bytes_sent;
        while(bytes_sent=fread(msg,1,SIZE,fptr)>0){
            if (send(sock,msg,sizeof(msg),0)==-1){
                perror("[-]Error in sending file.");
                exit(1);
            }
        }
        printf("[+]File data sent successfully.\n");
    }

    //changing CC to reno
    strcpy(msg, "reno");
    len = strlen(msg);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, msg, len) != 0) {
        perror("setsockopt");
        return -1;
    }
    len = sizeof(msg);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, msg, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    printf("New: %s\n", msg);//reno

    //send the file
    for (int i = 0; i < 5; i++) {
        size_t bytes_sent;
        while(bytes_sent=fread(msg,1,SIZE,fptr)>0){
            if (send(sock,msg,sizeof(msg),0)==-1){
                perror("[-]Error in sending file.");
                exit(1);
            }
        }
        printf("[+]File data sent successfully.\n");
    }
    fclose(fptr);
    close(sock);

    return 0;
}