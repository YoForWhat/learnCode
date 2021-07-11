#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define UDP_PORT    10000
#define UDP_ADDR    "192.168.31.100"
#define BUF_SIZE    1024   

int createUdpClient()
{
    int ret;
    int sockfd;
    struct sockaddr_in serverAddr;
    socklen_t serverLen = sizeof(struct sockaddr);
    char sendBuf[BUF_SIZE] = "hello server!";
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket fail\n");
        return -1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(UDP_ADDR);
    
    while(1) {
        sendto(sockfd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr*)&serverAddr, serverLen);
        usleep(1000);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    (void)createUdpClient();

    return 0;
}