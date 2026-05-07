#include "project_handlers.hpp"

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include "cache/cache_manager.hpp"
#include <userver/logging/log.hpp> 

namespace myservice {
namespace handlers {

CreateProjectHandler::CreateProjectHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      // storage_(context.FindComponent<storage::InMemoryStorage>()) {}
      storage_(context.FindComponent<storage::PostgresStorage>()),
      cache_(context.FindComponent<cache::CacheManager>()) {}

std::string CreateProjectHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    int user_id = context.GetData<int>("user_id");
    
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    if (!json.HasMember("name")) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error": "Missing name"})";
    }
    
    models::Project project;
    project.name = json["name"].As<std::string>();
    project.description = json["description"].As<std::string>("");
    project.key = json["key"].As<std::string>("");
    project.owner_id = user_id;
    // project.created_at = std::chrono::system_clock::now();
    project.created_at = userver::storages::postgres::TimePointTz(std::chrono::system_clock::now());
    
    auto created = storage_.CreateProject(project);

    cache_.Invalidate("projects:all");
    LOG_INFO() << "Project created, cache invalidated";
    
    userver::formats::json::ValueBuilder result;
    result["id"] = created->id;
    result["name"] = created->name;
    result["description"] = created->description;
    result["key"] = created->key;
    result["owner_id"] = created->owner_id;
    
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(result.ExtractValue());
}

GetProjectsHandler::GetProjectsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      // storage_(context.FindComponent<storage::InMemoryStorage>()) {}
      storage_(context.FindComponent<storage::PostgresStorage>()),
      cache_(context.FindComponent<cache::CacheManager>()) {}

std::string GetProjectsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    const std::string cache_key = "projects:all";
    
    auto cached = cache_.Get(cache_key);
    if (cached.has_value()) {
        LOG_INFO() << "Projects returned from CACHE";
        return *cached;
    }
    
    LOG_INFO() << "Projects cache MISS, querying PostgreSQL";
    
    auto projects = storage_.GetAllProjects();
    
    // userver::formats::json::ValueBuilder result;
    // for (size_t i = 0; i < projects.size(); ++i) {
    //     result[i]["id"] = projects[i].id;
    //     result[i]["name"] = projects[i].name;
    //     result[i]["description"] = projects[i].description;
    //     result[i]["key"] = projects[i].key;
    //     result[i]["owner_id"] = projects[i].owner_id;
    // }
    
    userver::formats::json::ValueBuilder result = userver::formats::json::MakeArray();
    for (const auto& project : projects) {
        userver::formats::json::ValueBuilder item;
        item["id"] = project.id;
        item["name"] = project.name;
        item["description"] = project.description;
        item["key"] = project.key;
        item["owner_id"] = project.owner_id;
        result.PushBack(std::move(item));
    }
    
    // return userver::formats::json::ToString(result.ExtractValue());
    std::string response = userver::formats::json::ToString(result.ExtractValue());
    
    // save to cache for 5 mins
    cache_.Set(cache_key, response, 300);
    
    return response;
}

} // namespace handlers
} // namespace myservice