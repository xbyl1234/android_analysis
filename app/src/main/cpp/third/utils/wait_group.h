#pragma once

#include <atomic>
#include <mutex>
#include <memory>
#include <condition_variable>

using std::atomic;
using std::mutex;
using std::shared_ptr;
using std::condition_variable;

class _WaitGroup {
public:
    _WaitGroup() : count(0) {}

    void add(int n = 1) {
        count += n;
    }

    void done() {
        --count;
        if (count == 0) { cv.notify_all(); }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        while (count > 0) {
            cv.wait(lock);
        }
    }

private:
    std::atomic<int> count;
    std::mutex mtx;
    std::condition_variable cv;
};

using WaitGroup = shared_ptr<_WaitGroup>;