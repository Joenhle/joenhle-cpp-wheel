#include "../thread_pool/thread_pool.hpp"
#include "../lock_free_queue/lock_free_queue.hpp"
#include <gtest/gtest.h>
using namespace std;

TEST(thread_pool, producer_consumer) {
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
    ThreadPool threadPool(6);
    for (int i = 0; i < 4; ++i) {
        threadPool.add_task(Task(func1));
    }
    for (int i = 0; i < 2; ++i) {
        threadPool.add_task(Task(func2));
    }
    std::this_thread::sleep_for(chrono::seconds(5));
    EXPECT_EQ(num.load(), 400);
}

int main(int argc,char **argv){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}