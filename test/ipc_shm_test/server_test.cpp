#include "../../ipc/shm_broker.hpp"
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
using namespace std;

int main() {
    shared_ptr<IPCBroker> ipcBroker = std::make_shared<SHMBroker>(3, 1024);
    ipcBroker->attach();
    sleep(5);
    ipcBroker->recv();
    spdlog::info(ipcBroker->get_data());
    return 0;
}