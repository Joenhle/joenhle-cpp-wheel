#ifndef THREAD_HPP
#define THREAD_HPP

#include <iostream>
#include <functional>
#include <thread>
#include <spdlog/spdlog.h>
#include <sstream>

class Thread {
public:
    enum State {
        RUNNING_FOREGROUND,
        RUNNING_BACKGROUND,
        IDLE,
    };

public:
    explicit Thread(std::function<void()>& task) : id_(0), state_(IDLE), task_(task), thread_(&Thread::self_task, this) {};
    explicit Thread(std::function<void()>&& task) : id_(0), state_(IDLE), task_(task), thread_(&Thread::self_task, this) {};

    State get_state() const {
        return state_;
    }

    std::thread::id get_tid() const {
        return id_;
    }

    void detach() {
        if (state_ == RUNNING_FOREGROUND) {
            thread_.detach();
            state_ = RUNNING_BACKGROUND;
        }
    }

private:
    std::thread::id id_;
    State state_;
    std::function<void()> task_;
    std::thread thread_;

    void self_task() {
        id_ = thread_.get_id();
        state_ = RUNNING_FOREGROUND;
        std::ostringstream oss;
        oss << id_;
        spdlog::info("thread {} is doing task...", oss.str());
        task_();
    }

};


#endif