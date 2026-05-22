#include "comment_handlers.hpp"
 
#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/logging/log.hpp>
 
#include "cache/cache_manager.hpp"
#include "event/event_producer.hpp"
 
namespace myservice {
namespace handlers {
 
// POST /api/tasks/{taskId}/comments
AddCommentHandler::AddCommentHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      mongo_storage_(context.FindComponent<storage::MongoStorage>()),
      cache_(context.FindComponent<cache::CacheManager>()),
      // event_producer_(std::make_shared<event::EventProducer>(context)) {}
      event_producer_(context.FindComponent<event::EventProducer>()) {}
 
std::string AddCommentHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& req_context) const {
    
    int user_id = req_context.GetData<int>("user_id"); // user_id from JWT token
    int task_id = std::stoi(request.GetPathArg("taskId"));
    
    auto json = userver::formats::json::FromString(request.RequestBody());
    
    models::Comment comment;
    comment.id = models::GenerateCommentId();
    comment.task_id = task_id;
    comment.author_id = user_id;
    comment.author = json["author"].As<std::string>();
    comment.text = json["text"].As<std::string>();
    comment.created_at = std::chrono::system_clock::now();
 
    mongo_storage_.InsertComment(comment);
 
    // Публикация события CommentAdded
    event::CommentAddedEvent event;
    event.comment_id = comment.id; // generated id
    event.task_id = task_id;
    event.author = comment.author;
    event.text = comment.text;
 
    std::string trace_id = request.GetHeader("X-Request-Id");
    event_producer_.PublishCommentAdded(event, trace_id);
 
    std::string cache_key = "comments:task:" + std::to_string(task_id);
    cache_.Invalidate(cache_key);
    LOG_INFO() << "Comment added, cache invalidated for task: " << task_id;
    
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return R"({"status": "ok"})";
}
 
// GET /api/tasks/{taskId}/comments
GetCommentsHandler::GetCommentsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      mongo_storage_(context.FindComponent<storage::MongoStorage>()),
      cache_(context.FindComponent<cache::CacheManager>()) {}
 
std::string GetCommentsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    int task_id = std::stoi(request.GetPathArg("taskId"));
    std::string cache_key = "comments:task:" + std::to_string(task_id);
    
    auto cached = cache_.Get(cache_key);
    if (cached.has_value()) {
        LOG_INFO() << "Comments returned from CACHE for task: " << task_id;
        return *cached;
    }
    
    LOG_INFO() << "Comments cache MISS for task: " << task_id << ", querying MongoDB";
    // if not in cache - then in mongodb
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
    
    std::string response = userver::formats::json::ToString(result.ExtractValue());
    
    // save to cache for 60 sec
    cache_.Set(cache_key, response, 60);
    
    return response;
}
 
// POST /api/comments/{commentId}/replies
AddReplyHandler::AddReplyHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      mongo_storage_(context.FindComponent<storage::MongoStorage>()),
      cache_(context.FindComponent<cache::CacheManager>()),
      // event_producer_(std::make_shared<event::EventProducer>(context)) {}
      event_producer_(context.FindComponent<event::EventProducer>()) {}
 
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
    
    int task_id = mongo_storage_.GetTaskIdByCommentId(comment_id);
    
    // в массив replies (MongoDB $push)
    mongo_storage_.AddReply(comment_id, reply);
 
    // публикация события CommentAdded (cause Reply is also a Comment)
    event::CommentAddedEvent event;
    event.comment_id = comment_id;  // родительский comment_id
    event.task_id = task_id;
    event.author = reply.author;
    event.text = reply.text;
    
    std::string trace_id = request.GetHeader("X-Request-Id");
    event_producer_.PublishCommentAdded(event, trace_id);
 
    // cache invalidation
    if (task_id > 0) {
        std::string cache_key = "comments:task:" + std::to_string(task_id);
        cache_.Invalidate(cache_key);
        LOG_INFO() << "Reply added, cache invalidated for task: " << task_id;
    } else {
        cache_.InvalidateByPrefix("comments:task:");
        LOG_INFO() << "Reply added, cache invalidated by prefix";
    }
 
    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kCreated);
    return R"({"status": "ok"})";
}
 
}  // namespace handlers
}  // namespace myservice