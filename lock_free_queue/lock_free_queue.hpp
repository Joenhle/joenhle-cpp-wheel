#include <atomic>
#include <memory>
using namespace std;

/**
    单读单写, 加内存顺序
**/
template <typename T, size_t Cap>
class SpscQueue : private allocator<T> {
    T* data;
    atomic<size_t> head{0}, tail{0};
private:
    template<typename ...Args>
    bool emplace(Args && ...args) {
        size_t t = tail.load(std::memory_order_relaxed);
        if ((t + 1) % Cap == head.load(std::memory_order_acquire))
            return false;
        allocator<T>::construct(data + t, std::forward<Args>(args)...);
        tail.store((t + 1) % Cap, std::memory_order_release);
        return true;
    }
public:
    SpscQueue() : data(allocator<T>::allocate(Cap)) {}
    SpscQueue(const SpscQueue&) = delete;
    SpscQueue& operator=(const SpscQueue&) = delete;
    SpscQueue& operator=(const SpscQueue&) volatile = delete;

    bool push(const T &val) {
        return emplace(val);
    }

    bool push(T && val) {
        return emplace(val);
    }
    bool pop(T &val) {
        size_t h  = head.load(std::memory_order_relaxed);
        if (h == tail.load(std::memory_order_acquire))
            return false;
        val = ::move(data[h]);
        allocator<T>::destroy(data + h);
        head.store((h + 1) % Cap, std::memory_order_release);
        return true;
    }
    bool empty() {
        return head.load(memory_order_acquire) == tail.load(memory_order_acquire);
    }
    bool full() {
        return (tail.load(memory_order_acquire) + 1) % Cap == head.load(memory_order_acquire);
    }
};


/**
    多读多写, 没加内存顺序
**/
template <typename T, size_t Cap>
class MpmsQueue {
    T data[Cap];
    atomic<size_t> head{0}, tail{0}, write{0};

public:
    MpmsQueue() = default;
    MpmsQueue(const MpmsQueue&) = delete;
    MpmsQueue &operator=(const MpmsQueue&) = delete;
    MpmsQueue &operator=(const MpmsQueue&) volatile = delete;

    bool push(const T &val) {
        size_t t, w;
        do {
            t = tail.load();
            if ((t + 1) % Cap == head.load()) return false;
        } while (!tail.compare_exchange_strong(t, (t + 1) % Cap));
        data[t] = val;
        do {
            w = t;
        } while (!write.compare_exchange_strong(w, (w + 1) % Cap));
        return true;
    }

    bool pop(T& val) {
        size_t h;
        do {
            h = head.load();
            if (h == write.load()) return false;
            val = data[h];
        } while (!head.compare_exchange_strong(h, (h + 1) % Cap));
        return true;
    }

    bool full() {
        return (tail.load() + 1) % Cap == head.load();
    }

    bool empty() {
        return head.load() == write.load();
    }
};

