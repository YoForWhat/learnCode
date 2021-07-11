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

#define PORT        10000
#define BUF_SIZE    1024    

int createUdpServer()
{
    int ret;
    int flag = 0;
    int timeCount = 0;
    int sockfd = 0;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    fd_set recvfd;
    struct timeval timeout;
    char recvbuf[BUF_SIZE];
    struct sockaddr_in serverAddr;
    struct sockaddr clientAddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("create sockfd fail\n");
        return -1;
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    // serverAddr.sin_addr.s_addr = inet_addr("192.168.31.100");
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind fail!\n");
        return -1;
    }

    while (1) {
        FD_ZERO(&recvfd);
        FD_SET(sockfd, &recvfd);
        ret = select(10, &recvfd, NULL, NULL, &timeout);
        if (ret == -1) {
            perror("select fail\n");
            // return -1;
        } 
        if (ret == 0 && flag == 1) { // timeout
            timeCount++;
            if (timeCount > 10) {
                printf("beyond 60 second no connect, over!\n");
                break;
            }
            // printf("select timeout\n");
            // break;
        } 
        if (FD_ISSET(sockfd, &recvfd)) {
            flag = 1;
            memset(recvbuf, 0, BUF_SIZE);
            ret = recvfrom(sockfd, recvbuf, BUF_SIZE, 0, (struct sockaddr*)&clientAddr, &addrLen);
            if (ret == -1) {
                printf("recvform() fail\n");
            } else {
                printf("recv:%s\n", recvbuf);
                ret = sendto(sockfd, recvbuf, BUF_SIZE, 0, (struct sockaddr*)&clientAddr, sizeof(struct sockaddr));
                if (ret == -1) {
                    printf("sendto() fail\n");
                }
            }
        }
        
        usleep(50);
    }
}


int main(int argc, char* argv[])
{
    int ret;
    ret = createUdpServer();
    return ret;
}