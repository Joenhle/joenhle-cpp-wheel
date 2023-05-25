#ifndef UDP_BROKER_HPP
#define UDP_BROKER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <spdlog/spdlog.h>

const static int MAX_SIZE = 1024;

class UdpBroker {
public:
    UdpBroker() {
        sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd_ < 0) {
            spdlog::error("create udp socket error");
            exit(-1);
        }
    }

    void bind(int port) const {
        struct sockaddr_in addr_serv;
        memset(&addr_serv, 0, sizeof(struct sockaddr_in));
        addr_serv.sin_family = AF_INET;
        addr_serv.sin_port = htons(port);
        addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
        if (::bind(sock_fd_, (struct sockaddr*)&addr_serv, sizeof(addr_serv)) < 0) {
            spdlog::error("udp bind error");
            exit(-1);
        }
    }

    std::string block_read(int size) {
        size = std::min(size, MAX_SIZE);
        char recv_buf[size];
        int len = sizeof(addr_client_);
        int recv_num = recvfrom(sock_fd_, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&addr_client_, (socklen_t*)&len);
        if (recv_num < 0) {
            spdlog::error("recvfrom error");
            exit(-1);
        }
        recv_buf[recv_num] = '\0';
        std::string res = recv_buf;
        return res;
    }

    void send_to(std::string ip, int port, std::string data) {
        struct sockaddr_in addr_serv;
        int len;
        memset(&addr_serv, 0, sizeof(addr_serv));
        addr_serv.sin_family = AF_INET;
        addr_serv.sin_addr.s_addr = inet_addr(ip.c_str());
        addr_serv.sin_port = htons(port);
        len = sizeof(addr_serv);
        int send_num = sendto(sock_fd_, data.c_str(), data.size(), 0, (struct sockaddr*)&addr_serv, len);
        if (send_num < 0) {
            spdlog::error("sendto error");
            exit(-1);
        }
    }

    void send(std::string data) {
        int len = sizeof(addr_client_);
        int send_num = sendto(sock_fd_, data.c_str(), data.size(), 0, (struct sockaddr*)&addr_client_, len);
        if (send_num < 0) {
            spdlog::error("sendto error");
            exit(-1);
        }
    }

    void close() {
        ::close(sock_fd_);
    }

private:
    int sock_fd_{-1};
    struct sockaddr_in addr_client_;
};

#endif