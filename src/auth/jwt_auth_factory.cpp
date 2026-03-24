#include "jwt_auth_factory.hpp"

namespace myservice {
namespace auth {

JwtAuthCheckerFactory::JwtAuthCheckerFactory(
    const userver::components::ComponentContext& context)
    : auth_(context.FindComponent<JwtAuthComponent>()) {}

userver::server::handlers::auth::AuthCheckerBasePtr
JwtAuthCheckerFactory::MakeAuthChecker(
    const userver::server::handlers::auth::HandlerAuthConfig&) const {
    return auth_.Get();
}

} // namespace auth
} // namespace myservice