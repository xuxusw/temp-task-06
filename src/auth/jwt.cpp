#include "jwt.hpp"
#include <jwt-cpp/jwt.h>
#include <chrono>

namespace myservice {
namespace auth {

// const std::string JwtSecret = std::getenv("JWT_SECRET") ???

const std::string JwtSecret = "my-secret-key-change-me-later";

std::string GenerateToken(int user_id) {
    auto token = jwt::create()
        .set_issuer("project-manager")
        .set_type("JWT")
        // .set_payload_claim("user_id", jwt::claim(user_id))
        .set_payload_claim("user_id", jwt::claim(std::to_string(user_id)))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
        .sign(jwt::algorithm::hs256{JwtSecret});
    return token;
}

std::optional<int> ValidateToken(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{JwtSecret})
            .with_issuer("project-manager");
        
        verifier.verify(decoded);
        
        auto user_id_claim = decoded.get_payload_claim("user_id");
        int user_id = std::stoi(user_id_claim.as_string());
        return user_id;
        
    } catch (...) {
        return std::nullopt;
    }
}

} // namespace auth
} // namespace myservice