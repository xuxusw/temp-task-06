#include "auth_middleware.hpp"
#include <userver/http/common_headers.hpp>
#include "auth/jwt.hpp"

namespace myservice {
namespace handlers {
  
bool AuthMiddleware::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) {
    
    // auto auth_header = request.GetHeader("");
    const auto& auth_header = request.GetHeader(
        userver::http::headers::kAuthorization);
    
    if (auth_header.empty()) {
        return false;
    }
    
    const std::string kBearerPrefix = "Bearer ";
    if (auth_header.size() <= kBearerPrefix.size() ||
        auth_header.substr(0, kBearerPrefix.size()) != kBearerPrefix) {
        return false;
    }
    
    std::string token = auth_header.substr(kBearerPrefix.size());
    
    auto user_id = auth::ValidateToken(token);
    if (!user_id.has_value()) {
        return false;
    }
    
    context.SetData<int>("user_id", *user_id);
    // context.SetData<std::string>("user_id", std::to_string(*user_id));
    
    return true;
}

} // namespace handlers
} // namespace myservice