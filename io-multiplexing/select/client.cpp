#include <iostream>
#include <cstring>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 6666

int main(int argc, char* argv[]) {
    int connfd = 0;
    int clen = 0;
    struct sockaddr_in client;
    if (argc < 2) {
        std::cout << "Usage: client [server IP address]" << std::endl;
        return -1;
    }

    client.sin_family = AF_INET;
    client.sin_port = htons(DEFAULT_PORT);
    client.sin_addr.s_addr = inet_addr(argv[1]);
    connfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connfd < 0) {
        std::cerr << "socket" << std::endl;
        return -2;
    }

    if (connect(connfd, (struct sockaddr*)&client, sizeof(client)) < 0) {
        std::cerr << "connect" << std::endl;
        return -3;
    }

    char buffer[1024];
    bzero(buffer, sizeof(buffer));
    recv(connfd, buffer, 1024, 0);
    std::cout << "recv: " << buffer << std::endl;
    strcpy(buffer, "this is client!\n");
    send(connfd, buffer, 1024, 0);
    while (1) {
        bzero(buffer, sizeof(buffer));
        scanf("%s", buffer);
        int p = strlen(buffer);
        buffer[p] = '\0';
        send(connfd, buffer, 1024, 0);
        std::cout << "I have send buffer\n";
    }
    close(connfd);
    return 0;
}