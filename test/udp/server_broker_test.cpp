#include "../../transport_client/udp_broker.hpp"
using namespace std;

int main() {
    UdpBroker udpBroker;
    udpBroker.bind(8000);
    while (1) {
        spdlog::info("server wait:");
        string read_data = udpBroker.block_read(100);
        spdlog::info("server receive data: {}", read_data);
        udpBroker.send("i am server");
    }
}
