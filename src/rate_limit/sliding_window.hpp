#pragma once

#include <chrono>
#include <deque>
#include <string>
#include <unordered_map>
#include <userver/engine/mutex.hpp>

namespace myservice::rate_limit {

struct RateLimitInfo {
    int limit;
    int remaining;
    int reset_seconds;
};

class SlidingWindowLimiter {
public:
    SlidingWindowLimiter(int max_requests, int window_seconds);
    
    bool TryAcquire(const std::string& key);
    RateLimitInfo GetInfo(const std::string& key);

private:
    struct WindowState {
        std::deque<std::chrono::steady_clock::time_point> requests;
    };
    
    void CleanExpired(WindowState& state);
    
    const int max_requests_;
    const std::chrono::seconds window_size_;
    userver::engine::Mutex mutex_;
    std::unordered_map<std::string, WindowState> windows_;
};

}  // namespace myservice::rate_limit