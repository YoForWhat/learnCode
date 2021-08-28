#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //struct sockaddr_in
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

char *g_ip = "127.0.0.1";
unsigned int g_port = 11111;

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serverAddr;
    int sockLen = sizeof(serverAddr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("create socket fail\n");
        exit(0);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(g_ip);
    serverAddr.sin_port = htons(g_port);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sockLen) != 0) {
        printf("Failed to connect\n");
        close(sockfd);
        exit(0);
    }
 
    char sendBuf[128] = {0};
    char recvBuf[128] = {0};
    while (1) {
        putchar('>');
        // bzero(sendBuf, sizeof(sendBuf));
        fgets(sendBuf, sizeof(sendBuf), stdin);
        
        if (send(sockfd, sendBuf, sizeof(sendBuf), 0) <= 0) {
            printf("send length == 0\n");
        } else {
            // if (recv(sockfd, sendBuf, sizeof(sendBuf), 0) <= 0) {
            //     printf("recv length == 0\n");
            // } else {
            //     if (strncmp(sendBuf, "quit", 4) == 0) {
            //         break;
            //     } 
            //     printf("recv:%s\n", sendBuf);
            // }
        }
        if (strncmp(sendBuf, "quit", 4) == 0) {
            break;
        }
        // usleep(1000);
    }

    close(sockfd);
    
    return 0;
}