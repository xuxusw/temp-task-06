#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include "storage/in_memory_storage.hpp"

namespace myservice {
namespace handlers {

class UserSearchHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-user-search";
    
    UserSearchHandler(const userver::components::ComponentConfig& config,
                      const userver::components::ComponentContext& context);
    
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
    
private:
    storage::InMemoryStorage& storage_;
};

} // namespace handlers
} // namespace myservice