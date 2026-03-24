#pragma once

#include <string>
#include <optional>

namespace myservice {
namespace auth {

std::string GenerateToken(int user_id);
std::optional<int> ValidateToken(const std::string& token);

extern const std::string JwtSecret;

} // namespace auth
} // namespace myservice