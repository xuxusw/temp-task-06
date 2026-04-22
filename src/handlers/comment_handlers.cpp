#include "comment_handlers.hpp"

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

namespace myservice {
namespace handlers {

// POST /api/tasks/{taskId}/comments
AddCommentHandler::AddCommentHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      mongo_storage_(context.FindComponent<storage::MongoStorage>()) {}

std::string AddCommentHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& req_context) const {
    
    int user_id = req_context.GetData<int>("user_id"); // user_id from JWT token
    int task_id = std::stoi(request.GetPathArg("taskId"));
    
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    models::Comment comment;
    comment.task_id = task_id;
    comment.author_id = user_id;
    comment.author = json["author"].As<std::string>();
    comment.text = json["text"].As<std::string>();
    comment.created_at = std::chrono::system_clock::now();
    
    mongo_storage_.InsertComment(comment);
    
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return R"({"status": "ok"})";
}

// GET /api/tasks/{taskId}/comments
GetCommentsHandler::GetCommentsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      mongo_storage_(context.FindComponent<storage::MongoStorage>()) {}

std::string GetCommentsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    int task_id = std::stoi(request.GetPathArg("taskId"));
    auto comments = mongo_storage_.GetCommentsByTask(task_id);
    
    // JSON ответ
    userver::formats::json::ValueBuilder result;
    for (size_t i = 0; i < comments.size(); ++i) {
        result[i]["id"] = comments[i].id;
        result[i]["author"] = comments[i].author;
        result[i]["text"] = comments[i].text;
        result[i]["created_at"] = std::chrono::system_clock::to_time_t(comments[i].created_at);
        
        for (size_t j = 0; j < comments[i].replies.size(); ++j) {
            result[i]["replies"][j]["author"] = comments[i].replies[j].author;
            result[i]["replies"][j]["text"] = comments[i].replies[j].text;
            result[i]["replies"][j]["created_at"] = std::chrono::system_clock::to_time_t(comments[i].replies[j].created_at);
        }
    }
    
    return userver::formats::json::ToString(result.ExtractValue());
}

// POST /api/comments/{commentId}/replies
AddReplyHandler::AddReplyHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      mongo_storage_(context.FindComponent<storage::MongoStorage>()) {}

std::string AddReplyHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    // commentId from URL
    std::string comment_id = request.GetPathArg("commentId");
    
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    models::Reply reply;
    reply.author = json["author"].As<std::string>();
    reply.text = json["text"].As<std::string>();
    reply.created_at = std::chrono::system_clock::now();
    
    // в массив replies (MongoDB $push)
    mongo_storage_.AddReply(comment_id, reply);
    
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return R"({"status": "ok"})";
}

}  // namespace handlers
}  // namespace myservice