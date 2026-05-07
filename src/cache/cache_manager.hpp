#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <userver/components/loggable_component_base.hpp> 
#include <userver/engine/mutex.hpp>

namespace myservice::cache {

class CacheManager final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "cache-manager"; // reg this in static_config

    CacheManager(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);
    
    ~CacheManager() override = default;

    std::optional<std::string> Get(const std::string& key);
    void Set(const std::string& key, std::string value, int ttl_seconds);
    void Invalidate(const std::string& key);
    void InvalidateByPrefix(const std::string& prefix);

private:
    struct CacheEntry {
        std::string value;
        std::chrono::steady_clock::time_point expires_at;
    };
    
    userver::engine::Mutex mutex_;
    std::unordered_map<std::string, CacheEntry> cache_;
};

}  // namespace myservice::cache