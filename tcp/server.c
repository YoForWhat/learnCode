#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_RECV_LENGTH 1024
#define MAX_CLIENT_NUM  32
#define LISTEN_NUMBER   32
#define PORT            11111

int clientFd[MAX_CLIENT_NUM] = {0};

int main(int argc, char **argv)
{
    int i;
    int ret;
    int sockFd;
    
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(serverAddr);

    int recvLen;
    char recvBuffer[MAX_RECV_LENGTH] = {0};
    char sendBuffer[MAX_RECV_LENGTH] = {0};

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("Failed to socket\n");
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    // 设置端口可复用
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind fail\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sockFd, MAX_CLIENT_NUM) < 0) {
        perror("listen fail\n");
        exit(EXIT_FAILURE);
    }

    printf("server listening...\n");

    int maxFd = sockFd;
    int newFd;
    fd_set readFdSet;
    struct timeval tv;
    while (1) {
        // 超时时间必须每次重新赋值，因为用过后会清除
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        // 接收设置位必须每次重新绑定
        FD_ZERO(&readFdSet);
        FD_SET(sockFd, &readFdSet);
        for (int i = 0; i < MAX_CLIENT_NUM; i++) {
            FD_SET(clientFd[i], &readFdSet);
        }

        ret = select(maxFd + 1, &readFdSet, NULL, NULL, &tv);
        if (ret < 0) { // fail
            printf("select fail ret = %d, errno = %s\n", ret, strerror(errno));
            continue; 
        } else if (ret == 0) { // time out
            printf("timeout\n");
            continue;
        } else { // 有数据过来了，则开始接收 
            if (FD_ISSET(sockFd, &readFdSet)) {
                newFd = accept(sockFd, (struct sockaddr *)&clientAddr, &addrLen);
                if (newFd < 0) {
                    perror("fail to accept socket");
                } else {
                    printf("********************************************************\n");
                    printf("connect FD:   %d\n", newFd);
                    printf("Client  IP:   %s\n", inet_ntoa(clientAddr.sin_addr));
                    printf("Client  PORT: %d\n", ntohs(clientAddr.sin_port));
                    printf("********************************************************\n");
                    
                     // 找到空闲的客户端描述符位
                    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
                        if (clientFd[i] == 0) {
                            clientFd[i] = newFd;
                            break;
                        }
                    }
                    // 如果得到的描述符值超过最大值，则赋值
                    if (maxFd < newFd) {
                        maxFd = newFd;
                    }
                }
            }
        }
        // 遍历所有描述符，接受数据
        for (int i = 0; i < MAX_CLIENT_NUM; i++) {
            if (clientFd[i] > 0) {
                if (FD_ISSET(clientFd[i], &readFdSet)) {
                    bzero(recvBuffer, sizeof(recvBuffer));
                    // 如果没有接收到数据，则clear该描述符
                    if (recv(clientFd[i], recvBuffer, sizeof(recvBuffer), 0) < 0) {
                        printf("recv error\n");
                        FD_CLR(clientFd[i], &readFdSet);
                        clientFd[i] = 0;
                        close(clientFd[i]);
                    } else {
                        if (strncmp(recvBuffer, "quit", 4) == 0) {
                            clientFd[i] = 0;
                            continue;
                        }
                        
                        printf("server recv:%s\n", recvBuffer);
                    }
                }
            }
        }
    }
}