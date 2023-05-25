#ifndef IPC_BROKER_HPP
#define IPC_BROKER_HPP
#include <string>

class IPCBroker {
public:
    IPCBroker() = default;
    virtual std::string get_data() = 0;
    virtual void attach() = 0;
    virtual void recv() = 0;
    virtual void send(const std::string& data) = 0;
    virtual void send(const std::string&& data) = 0;
    virtual void set_socket(int option) = 0;
    virtual ~IPCBroker() = default;
};

#endif