#pragma once
#include <chrono>
#include <thread>
#include <cstdint>
#include <algorithm>

class DSataThrottle {
public:
    explicit DSataThrottle(uint64_t bytesPerSecond)
        : limit_(bytesPerSecond), tokens_(bytesPerSecond),
          last_(std::chrono::high_resolution_clock::now()) {}

    void consume(size_t bytes) {
        double required = (double)bytes / limit_;
        auto deadline = std::chrono::high_resolution_clock::now() +
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::duration<double>(required));
        refill();
        while (tokens_ < bytes) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            refill();
        }
        tokens_ -= bytes;
        auto now = std::chrono::high_resolution_clock::now();
        if (now < deadline)
            std::this_thread::sleep_until(deadline);
    }

private:
    uint64_t limit_;
    uint64_t tokens_;
    std::chrono::high_resolution_clock::time_point last_;

    void refill() {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - last_).count();
        last_ = now;
        uint64_t add = static_cast<uint64_t>(elapsed * limit_);
        tokens_ = std::min(tokens_ + add, limit_);
    }
};
