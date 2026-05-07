#include "cache_manager.hpp"
#include <userver/logging/log.hpp>

namespace myservice::cache {

CacheManager::CacheManager(const userver::components::ComponentConfig& config,
                           const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
    LOG_INFO() << "CacheManager initialized (in-memory cache)";
}

std::optional<std::string> CacheManager::Get(const std::string& key) {
    // userver::engine::MutexLock lock(mutex_);
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return std::nullopt;
    }
    
    if (std::chrono::steady_clock::now() > it->second.expires_at) {
        cache_.erase(it);
        return std::nullopt;
    }
    
    LOG_INFO() << "Cache HIT: " << key;
    return it->second.value;
}

void CacheManager::Set(const std::string& key, std::string value, int ttl_seconds) {
    // userver::engine::MutexLock lock(mutex_);
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    cache_[key] = CacheEntry{
        .value = std::move(value),
        .expires_at = std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds)
    };
    LOG_INFO() << "Cache SET: " << key << " TTL=" << ttl_seconds << "s";
}

void CacheManager::Invalidate(const std::string& key) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    cache_.erase(key);
    LOG_INFO() << "Cache INVALIDATE: " << key;
}

void CacheManager::InvalidateByPrefix(const std::string& prefix) {
   std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    for (auto it = cache_.begin(); it != cache_.end();) {
        if (it->first.rfind(prefix, 0) == 0) {
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
    LOG_INFO() << "Cache INVALIDATE by prefix: " << prefix;
}

}  // namespace myservice::cache