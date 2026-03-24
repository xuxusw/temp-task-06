#pragma once

#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>

namespace myservice {
namespace handlers {

class AuthMiddleware {
public:
    static bool CheckAuth(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context);
};

} // namespace handlers
} // namespace myservice