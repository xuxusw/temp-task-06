#include "sliding_window.hpp"
#include <algorithm>

namespace myservice::rate_limit {

SlidingWindowLimiter::SlidingWindowLimiter(int max_requests, int window_seconds)
    : max_requests_(max_requests), window_size_(window_seconds) {}

void SlidingWindowLimiter::CleanExpired(WindowState& state) {
    auto now = std::chrono::steady_clock::now();
    auto cutoff = now - window_size_;
    
    while (!state.requests.empty() && state.requests.front() < cutoff) {
        state.requests.pop_front();
    }
}

bool SlidingWindowLimiter::TryAcquire(const std::string& key) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    auto& state = windows_[key];
    CleanExpired(state);
    
    if (state.requests.size() >= static_cast<size_t>(max_requests_)) {
        return false;
    }
    
    state.requests.push_back(std::chrono::steady_clock::now());
    return true;
}

RateLimitInfo SlidingWindowLimiter::GetInfo(const std::string& key) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    auto it = windows_.find(key);
    if (it == windows_.end()) {
        return {max_requests_, max_requests_, static_cast<int>(window_size_.count())};
    }
    
    CleanExpired(it->second);
    
    int remaining = std::max(0, max_requests_ - static_cast<int>(it->second.requests.size()));
    
    return {max_requests_, remaining, static_cast<int>(window_size_.count())};
}

}  // namespace myservice::rate_limit