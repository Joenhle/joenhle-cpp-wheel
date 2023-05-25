#ifndef SHM_BROKER_HPP
#define SHM_BROKER_HPP

#include "ipc_broker.hpp"
#include <sys/shm.h>
#include "sys/sem.h"


class SHMBroker : public IPCBroker {
public:
    SHMBroker(int key, size_t size) : key_(key), size_(size) {
        shm_id_ = shmget(key_, size_, IPC_CREAT | 0666);
        sem_id_ = semget(key_, 1, IPC_CREAT | 0666);
        semctl(sem_id_, 0 , SETVAL, 1);
    }

    std::string get_data() override {
        return data_;
    }

    void attach() override {
        ptr_ = shmat(shm_id_, nullptr, 0);
    }

    void recv() override {
        struct sembuf buf{};
        buf.sem_flg = 0;
        buf.sem_num = 0;
        buf.sem_op = -1;
        semop(sem_id_, &buf, 1);
        data_ = "";
        for (int i = 0; i < size_; ++i) {
            data_ += *((char*)(ptr_) + i);
        }
        buf.sem_op = 1;
        semop(sem_id_, &buf, 1);
    }

    void send(const std::string& data) override {
        struct sembuf buf{};
        buf.sem_flg = 0;
        buf.sem_num = 0;
        buf.sem_op = -1;
        semop(sem_id_, &buf, 1);
        data_ = "";
        for (int i = 0; i < size_; ++i) {
            *((char*)(ptr_) + i) = data[i];
        }
        buf.sem_op = 1;
        semop(sem_id_, &buf, 1);
    }

    void send(const std::string&& data) override {
        struct sembuf buf{};
        buf.sem_flg = 0;
        buf.sem_num = 0;
        buf.sem_op = -1;
        semop(sem_id_, &buf, 1);
        data_ = "";
        for (int i = 0; i < size_; ++i) {
            *((char*)(ptr_) + i) = data[i];
        }
        buf.sem_op = 1;
        semop(sem_id_, &buf, 1);
    }

    void set_socket(int option) override {
    }

    ~SHMBroker() override {
        shmdt(ptr_);
        shmctl(shm_id_, IPC_RMID, 0);
    }

private:
    int key_;
    size_t size_;
    int shm_id_{-1};
    int sem_id_{-1};
    void* ptr_{nullptr};
    std::string data_;
};

#endif