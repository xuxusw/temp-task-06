#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

// #include "storage/in_memory_storage.hpp"
#include "storage/postgres_storage.hpp"
#include <memory>

namespace myservice::cache {
class CacheManager;
}

namespace event {
class EventProducer;
}

namespace myservice {
namespace handlers {

class CreateProjectHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-create-project";
    
    CreateProjectHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);
    
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
    
private:
    // storage::InMemoryStorage& storage_;
    storage::PostgresStorage& storage_;
    cache::CacheManager& cache_; 
    // std::shared_ptr<event::EventProducer> event_producer_;
    event::EventProducer& event_producer_;
};

class GetProjectsHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-projects";
    
    GetProjectsHandler(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context);
    
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
    
private:
    // storage::InMemoryStorage& storage_;
    storage::PostgresStorage& storage_;
    cache::CacheManager& cache_; 
    // std::shared_ptr<event::EventProducer> event_producer_;
    event::EventProducer& event_producer_;
};

} // namespace handlers
} // namespace myservice