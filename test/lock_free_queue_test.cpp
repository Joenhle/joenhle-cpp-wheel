#include "../lock_free_queue/lock_free_queue.hpp"
#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include <atomic>
using namespace std;


TEST(lock_free_queue, single_producer_single_consumer) {
    SpscQueue<int, 10> queue;
    auto func1 = [&](){
        int i = 0;
        while (i != 100) {
            if (!queue.full() && queue.push(i)) {
                ++i;
            }
        }
    };

    auto func2 = [&](){
        int i = 0;
        while (i != 100) {
            int val;
            if (!queue.empty() && queue.pop(val)) {
                EXPECT_EQ(val, i);
                ++i;
            }
        }
    };
    thread t1(func1), t2(func2);
    t1.join();
    t2.join();
}

TEST(lock_free_queue, multi_producer_multi_consumer) {
    MpmsQueue<int, 10> queue;
    atomic<int> done{0};

    auto func1 = [&](){
        int i = 0;
        while (i != 100) {
            if (!queue.full() && queue.push(i)) {
                ++i;
            }
        }
        done++;
    };

    atomic<int> num{0};
    auto func2 = [&]() {
        while (done != 4 || !queue.empty()) {
            int val;
            if (!queue.empty() && queue.pop(val)) {
                num++;
            }
        }
    };

    thread t1(func1), t2(func1), t3(func1), t4(func1);
    thread c1(func2), c2(func2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    c1.join();
    c2.join();

    EXPECT_EQ(num.load(), 400);
}


int main(int argc,char **argv){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
