#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <sys/wait.h>
#include <spdlog/spdlog.h>

#define DEFAULT_PORT 6666

int main(int argc, char **argv) {
    int serverfd, acceptfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    unsigned int sin_size, myport = 6666, lisnum = 10;

    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        spdlog::error("socket");
        return -1;
    }
    spdlog::info("socket ok");
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
        spdlog::error("bind");
        return -2;
    }
    spdlog::info("bind ok");

    if (listen(serverfd, lisnum) == -1) {
        spdlog::error("listen");
        return -3;
    }
    spdlog::info("listen ok");

    fd_set client_fdset;
    int maxsock;
    struct timeval tv;
    int client_sockfd[5];
    bzero((void*)client_sockfd, sizeof(client_sockfd));

    int conn_amount = 0;
    maxsock = serverfd;
    char buffer[1024];
    int ret = 0;

    while (1) {
        FD_ZERO(&client_fdset);
        FD_SET(serverfd, &client_fdset);
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        for (int i = 0; i < 5; ++i) {
            if (client_sockfd[i] != 0) {
                FD_SET(client_sockfd[i], &client_fdset);
            }
        }

        ret = select(maxsock+1, &client_fdset, nullptr, nullptr, &tv);
        if (ret < 0) {
            spdlog::error("select error!\n");
            break;
        } else if (ret == 0) {
            spdlog::info("timeout");
            continue;
        }

        if (FD_ISSET(serverfd, &client_fdset)) {
            struct sockaddr_in client_addr;
            size_t size = sizeof(struct sockaddr_in);
            int sock_client = accept(serverfd, (struct sockaddr*)(&client_addr), (unsigned int*)(&size));
            if (sock_client < 0) {
                spdlog::error("accept error!");
                continue;
            }

            if (conn_amount < 5) {
                client_sockfd[conn_amount++] = sock_client;
                bzero(buffer, 0);
                strcpy(buffer, "this is server! welcome!\n");
                send(sock_client, buffer, 1024, 0);
                spdlog::info("new connection client[{}] {}:{}", conn_amount-1, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                bzero(buffer, sizeof(buffer));
                ret = recv(sock_client, buffer, 1024, 0);
                if (ret < 0) {
                    spdlog::error("recv error!");
                    close(sock_client);
                    return -1;
                }
                spdlog::info("recv: {}", buffer);
                if (sock_client > maxsock) {
                    maxsock = sock_client;
                }
            } else {
                spdlog::warn("max connections!!!quit!!");
                break;
            }
        }

        for (int i = 0; i < conn_amount; ++i) {
            if (FD_ISSET(client_sockfd[i], &client_fdset)) {
                spdlog::info("start recv from client[{}]\n", i);
                ret = recv(client_sockfd[i], buffer, 1024, 0);
                if (ret <= 0) {
                    spdlog::info("client[{}] close", i);
                    close(client_sockfd[i]);
                    FD_CLR(client_sockfd[i], &client_fdset);
                    client_sockfd[i] = 0;
                } else {
                    spdlog::info("recv from client[{}]: {}", i, buffer);
                }
            }
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (client_sockfd[i] != 0) {
            close(client_sockfd[i]);
        }
    }
    close(serverfd);
}
