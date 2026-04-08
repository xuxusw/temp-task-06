#include "task_handlers.hpp"

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include "models/task.hpp"

namespace myservice {
namespace handlers {

CreateTaskHandler::CreateTaskHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      // storage_(context.FindComponent<storage::InMemoryStorage>()) {}
      storage_(context.FindComponent<storage::PostgresStorage>()) {}

std::string CreateTaskHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    int user_id = context.GetData<int>("user_id");
    int project_id = std::stoi(request.GetPathArg("projectId"));
    
    auto project = storage_.GetProjectById(project_id); // check that the project exists! 
    if (!project.has_value()) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kNotFound);
        return R"({"error": "Project not found"})";
    }
    
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    if (!json.HasMember("title")) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error": "Missing title"})";
    }
    
    models::Task task;
    task.title = json["title"].As<std::string>();
    task.description = json["description"].As<std::string>("");
    task.status = models::TaskStatusFromString(json["status"].As<std::string>("TODO"));
    task.project_id = project_id;
    task.creator_id = user_id;
    task.priority = json["priority"].As<int>(3);
    // task.created_at = std::chrono::system_clock::now();
    task.created_at = userver::storages::postgres::TimePointTz(std::chrono::system_clock::now());
    
    auto created = storage_.CreateTask(task);
    
    userver::formats::json::ValueBuilder result;
    result["id"] = created->id;
    result["title"] = created->title;
    result["description"] = created->description;
    result["status"] = models::TaskStatusToString(created->status);
    result["project_id"] = created->project_id;
    result["creator_id"] = created->creator_id;
    result["priority"] = created->priority;
    
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(result.ExtractValue());
}

} // namespace handlers
} // namespace myservice