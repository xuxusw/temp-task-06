#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

// #include "storage/in_memory_storage.hpp"
#include "storage/postgres_storage.hpp"

namespace myservice {
namespace handlers {

class RegisterHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-register";
    
    RegisterHandler(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);
    
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
    
private:
    // storage::InMemoryStorage& storage_;  // ссылка 
    storage::PostgresStorage& storage_; 
};

class LoginHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-login";
    
    LoginHandler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);
    
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
    
private:
    // storage::InMemoryStorage& storage_;  // ссылка
    storage::PostgresStorage& storage_;
};

} // namespace handlers
} // namespace myservice