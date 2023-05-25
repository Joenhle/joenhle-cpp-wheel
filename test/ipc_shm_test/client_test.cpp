#include "../../ipc/shm_broker.hpp"
#include <memory>
#include <string>
using namespace std;

int main() {
    shared_ptr<IPCBroker> ipcBroker = std::make_shared<SHMBroker>(3, 1024);
    ipcBroker->attach();
    const string data{"hello shm\n"};
    ipcBroker->send(data);
    return 0;
}