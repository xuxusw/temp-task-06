#include "jwt_auth_checker.hpp"
 
#include <jwt-cpp/jwt.h>
 
#include <userver/http/common_headers.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
 
namespace myservice {
namespace auth {
 
namespace {
static constexpr std::string_view kBearerPrefix = "Bearer ";
static constexpr std::string_view kIssuer = "project-manager";
static const std::string kSecret = "my-secret-key-change-me-later";
}  // namespace
 
JwtChecker::JwtChecker(const std::string& secret) : secret_(secret) {}
 
JwtChecker::AuthCheckResult JwtChecker::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    const std::string_view auth_header =
        request.GetHeader(userver::http::headers::kAuthorization);

    if (auth_header.empty()) {
        return AuthCheckResult{AuthCheckResult::Status::kTokenNotFound,
                               "Missing 'Authorization' header"};
    }

    if (!auth_header.starts_with(kBearerPrefix)) {
        return AuthCheckResult{AuthCheckResult::Status::kInvalidToken,
                               "Invalid authorization type, expected 'Bearer'"};
    }
    
    std::string token = std::string(auth_header.substr(kBearerPrefix.length()));
    // const std::string_view token = auth_header.substr(kAlgorithm.length());

    try {
        // (https://github.com/Thalhammer/jwt-cpp/blob/master/example/jwks-verify.cpp) ? jwks и rsa256
        // пока пусть будет hs256 (https://github.com/Thalhammer/jwt-cpp/blob/master/README.md)
        auto decoded = jwt::decode(token);
        
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret_})
            .with_issuer(std::string(kIssuer));
        
        verifier.verify(decoded);
        
        auto user_id_claim = decoded.get_payload_claim("user_id");
        int user_id = std::stoi(user_id_claim.as_string());
        
        context.SetData<int>("user_id", user_id);
        
        return AuthCheckResult{AuthCheckResult::Status::kOk};
        
    } catch (const jwt::error::token_verification_exception& exc) {
        return AuthCheckResult{
            AuthCheckResult::Status::kInvalidToken,
            "Token verification failed: " + std::string{exc.what()}};
    } catch (const std::exception& exc) {
        return AuthCheckResult{
            AuthCheckResult::Status::kForbidden,
            "Token processing error: " + std::string{exc.what()}};
    }
}
 
JwtAuthComponent::JwtAuthComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
    std::string secret = config["secret"].As<std::string>(kSecret);
    authorizer_ = std::make_shared<JwtChecker>(secret);
    // authorizer_ = std::make_shared<JwtChecker>(config[kSecret].As<std::string>());
}
 
JwtCheckerPtr JwtAuthComponent::Get() const { return authorizer_; }
 
userver::yaml_config::Schema JwtAuthComponent::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<LoggableComponentBase>(R"(
type: object
description: JWT Auth Checker Component
additionalProperties: false
properties:
    secret:
        type: string
        description: secret key for JWT validation
        default: "my-secret-key-change-me-later"
)");
}
 
} // namespace auth
} // namespace myservice