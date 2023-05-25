#include "../../transport_client/udp_broker.hpp"

using namespace std;

int main() {
    UdpBroker udpBroker;
    string send_data = "hey, who are you?";
    spdlog::info("client send: {}", send_data);
    udpBroker.send_to("127.0.0.1", 8000, send_data);
    string recv_data = udpBroker.block_read(100);
    spdlog::info("client receive: {}", recv_data);
    udpBroker.close();
}