#ifndef TASK_HPP
#define TASK_HPP

#include <functional>

class Task {
public:
    explicit Task(std::function<void()>& func) : func_(func) {};
    explicit Task(std::function<void()>&& func) : func_(func) {};
    void run() {
        func_();
    }
private:
    std::function<void()> func_;
};

#endif