#include "auth_handlers.hpp"

#include <userver/formats/json.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/http/http_status.hpp>

#include "auth/jwt.hpp"
#include "models/user.hpp"

#include "rate_limit/sliding_window.hpp"
#include <userver/logging/log.hpp>

namespace myservice {
namespace handlers {

// 5 попыток в минуту на IP
static rate_limit::SlidingWindowLimiter login_limiter(5, 60);

// ссылка на компонент
RegisterHandler::RegisterHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      // storage_(context.FindComponent<storage::InMemoryStorage>()) {}
      storage_(context.FindComponent<storage::PostgresStorage>()) {}  
    
std::string RegisterHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    // auto json = boost::json::parse(request.RequestBody());
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    if (!json.HasMember("login") || !json.HasMember("password") ||
        !json.HasMember("first_name") || !json.HasMember("last_name") || !json.HasMember("email")) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error": "Missing required fields"})";
    }
    
    std::string login = json["login"].As<std::string>();
    
    // storage_ ссылка 
    if (storage_.GetUserByLogin(login).has_value()) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kConflict);
        return R"({"error": "User already exists"})";
    }
    
    models::User user;
    user.login = login;
    // user.password_hash = bcrypt::generate_hash(password, 10);
    user.password_hash = userver::crypto::hash::Sha256(json["password"].As<std::string>());
    user.first_name = json["first_name"].As<std::string>();
    user.last_name = json["last_name"].As<std::string>();
    user.email = json["email"].As<std::string>();
    // user.created_at = std::chrono::system_clock::now();
    user.created_at = userver::storages::postgres::TimePointTz(std::chrono::system_clock::now());
    
    auto created = storage_.CreateUser(user); 
    
    // ответ
    userver::formats::json::ValueBuilder result;
    result["id"] = created->id;
    result["login"] = created->login;
    result["first_name"] = created->first_name;
    result["last_name"] = created->last_name;
    result["email"] = created->email;
    
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(result.ExtractValue());
}


// снова ссылка на компонент
LoginHandler::LoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      // storage_(context.FindComponent<storage::InMemoryStorage>()) {}
      storage_(context.FindComponent<storage::PostgresStorage>()) {} 

std::string LoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    std::string client_ip = request.GetRemoteAddress().PrimaryAddressString();
    
    // check rate limit
    if (!login_limiter.TryAcquire(client_ip)) {
        auto info = login_limiter.GetInfo(client_ip);
        
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kTooManyRequests);
        request.GetHttpResponse().SetHeader(std::string("X-RateLimit-Limit"), std::to_string(info.limit));
        request.GetHttpResponse().SetHeader(std::string("X-RateLimit-Remaining"), std::to_string(info.remaining));
        request.GetHttpResponse().SetHeader(std::string("X-RateLimit-Reset"), std::to_string(info.reset_seconds));
        request.GetHttpResponse().SetHeader(std::string("Retry-After"), std::to_string(info.reset_seconds));
        
        LOG_WARNING() << "Rate limit exceeded for IP: " << client_ip;
        return R"({"error": "Too many login attempts. Please try again later."})";
    }
    
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    if (!json.HasMember("login") || !json.HasMember("password")) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error": "Missing login or password"})";
    }
    
    std::string login = json["login"].As<std::string>();
    std::string password = json["password"].As<std::string>();
    
    // // storage_ ссылка 
    auto user = storage_.GetUserByLogin(login);
    if (!user.has_value()) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kUnauthorized);
        return R"({"error": "Invalid credentials"})";
    }

    // std::string password_hash = bcrypt::generate_hash(password, 10);
    std::string password_hash = userver::crypto::hash::Sha256(password);
    if (user->password_hash != password_hash) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kUnauthorized);
        return R"({"error": "Invalid credentials"})";
    }
    
    std::string token = auth::GenerateToken(user->id);

    // when login success - add limit info
    auto info = login_limiter.GetInfo(client_ip);
    request.GetHttpResponse().SetHeader(std::string("X-RateLimit-Limit"), std::to_string(info.limit));
    request.GetHttpResponse().SetHeader(std::string("X-RateLimit-Remaining"), std::to_string(info.remaining));
    
    userver::formats::json::ValueBuilder result;
    result["token"] = token;
    // result["token_type"] = "Bearer";
    // result["expires_in"] 
    
    return userver::formats::json::ToString(result.ExtractValue());
}

} // namespace handlers
} // namespace myservice

