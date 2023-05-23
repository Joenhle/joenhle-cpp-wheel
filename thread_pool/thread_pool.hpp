#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "thread.hpp"
#include "task.hpp"
#include <list>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <spdlog/spdlog.h>

const static int MAX_TASK_NUM = 10000;
const static int THREAD_NUM = 4;

class ThreadPool {
public:
    explicit ThreadPool(int thread_num = THREAD_NUM, int max_task_num = MAX_TASK_NUM) : thread_num_(thread_num), max_task_num_(max_task_num), stop_(false) {
        for (int i = 0; i < thread_num_; ++i) {
            thread_queue_.push_back(std::make_shared<Thread>(std::function<void()>([this]{do_task();})));
        }
    }

    int add_task(const Task& task) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (task_queue_.size() >= max_task_num_) {
            spdlog::info("tasks too much...");
            conv_p_.wait(lock);
        }
        task_queue_.push_back(std::make_shared<Task>(task));
        conv_c_.notify_one();
        return 0;
    }

    void stop() {
        stop_ = true;
    }

    ~ThreadPool() {
        stop();
        conv_c_.notify_all();
        for (auto& thread : thread_queue_) {
            thread->detach();
        }
        spdlog::info("ThreadPool is over, total task accomplished: {}", total_);
    }

private:
    void do_task() {
        while (!stop_) {
            std::unique_lock<std::mutex> lock(mutex_);
            while (task_queue_.empty()) {
                conv_c_.wait(lock);
                if (stop_) goto loop_out;
            }
            std::shared_ptr<Task> task = task_queue_.front();
            task_queue_.pop_front();
            conv_p_.notify_one();
            total_++;
            lock.unlock();
            task->run();
        }
        loop_out:
        spdlog::info("thread loop out");
    }

private:
    std::vector<std::shared_ptr<Thread>> thread_queue_;
    std::list<std::shared_ptr<Task>> task_queue_;
    int thread_num_;
    int max_task_num_;
    std::mutex mutex_;
    std::condition_variable conv_p_, conv_c_;
    bool stop_;
    std::atomic_int total_{0};
};

#endif