#include "user_handlers.hpp"

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/logging/log.hpp> 

namespace myservice {
namespace handlers {

UserSearchHandler::UserSearchHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<storage::InMemoryStorage>()) {}

std::string UserSearchHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    std::string login_param = request.GetArg("login");
    std::string first_name_param = request.GetArg("first_name");
    std::string last_name_param = request.GetArg("last_name");

    LOG_INFO() << "Searching for login: " << login_param;
    LOG_INFO() << "Searching for first_name: " << first_name_param;
    LOG_INFO() << "Searching for last_name: " << last_name_param;
    
    std::vector<models::User> users;
    
    if (!login_param.empty()) {
        users = storage_.SearchUsersByLogin(login_param);
        LOG_INFO() << "Found users by login: " << users.size();
    } else if (!first_name_param.empty() || !last_name_param.empty()) {
        users = storage_.SearchUsersByFirstNameLastName(first_name_param, last_name_param);
        LOG_INFO() << "Found users by name: " << users.size();
    } else {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error": "No search parameters provided"})";
    }
    
    LOG_INFO() << "Creating JSON response...";

    // userver::formats::json::ValueBuilder result;
    // for (size_t i = 0; i < users.size(); ++i) {
    //     result[i]["id"] = users[i].id;
    //     result[i]["login"] = users[i].login;
    //     result[i]["first_name"] = users[i].first_name;
    //     result[i]["last_name"] = users[i].last_name;
    //     result[i]["email"] = users[i].email;
    // }

    userver::formats::json::ValueBuilder result = userver::formats::json::MakeArray();
    for (const auto& user : users) {
        // объект внутри массива
        userver::formats::json::ValueBuilder item;
        item["id"] = user.id;
        item["login"] = user.login;
        item["first_name"] = user.first_name;
        item["last_name"] = user.last_name;
        item["email"] = user.email;
        result.PushBack(std::move(item));
    }

    LOG_INFO() << "JSON response created, size: " << users.size();

    return userver::formats::json::ToString(result.ExtractValue());
}

} // namespace handlers
} // namespace myservice