#include "event_consumer.hpp"
 
#include <userver/components/component_context.hpp>
#include <userver/formats/bson.hpp>
#include <userver/formats/bson/serialize.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/mongo/operations.hpp>
#include <userver/storages/mongo/pool.hpp>
#include <userver/utils/datetime.hpp>
 
#include <iomanip>
#include <sstream>
#include <chrono>
 
namespace event {
 
EventConsumer::EventConsumer(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context),
      consumer_scope_(
          context.FindComponent<userver::kafka::ConsumerComponent>("kafka-consumer").GetConsumer()
      ),
      mongo_pool_(
          context.FindComponent<userver::components::Mongo>("mongo-db-1").GetPool()
      ) {
    
    LOG_INFO() << "EventConsumer initializing...";
    
    consumer_scope_.Start(
        [this](userver::utils::span<const userver::kafka::Message> messages) {
            this->ProcessBatch(messages);
        }
    );
    
    LOG_INFO() << "EventConsumer started successfully, waiting for messages...";
}
 
EventConsumer::~EventConsumer() {
    LOG_INFO() << "EventConsumer shutting down...";
}
 
std::string EventConsumer::GetCurrentTimestampISO() {
    auto now = std::chrono::system_clock::now();
    return userver::utils::datetime::Timestring(now, "Y-m-dTH:M:SZ");
}
 
void EventConsumer::ProcessBatch(userver::utils::span<const userver::kafka::Message> messages) {
    LOG_INFO() << "Processing batch of " << messages.size() << " messages";
    
    for (const auto& message : messages) {
        try {
            auto json = userver::formats::json::FromString(message.GetPayload());
            
            std::string event_id = json["event_id"].As<std::string>();
            std::string event_type = json["event_type"].As<std::string>();
            std::string timestamp = json["timestamp"].As<std::string>();
            auto payload = json["payload"];
            
            LOG_DEBUG() << "Processing event: " << event_type 
                        << " (id: " << event_id 
                        << ", offset: " << message.GetOffset() << ")";
            
            if (event_type == "UserRegistered") {
                ProcessUserRegistered(payload);
            } 
            else if (event_type == "UserLoggedIn") {
                ProcessUserLoggedIn(payload);
            }
            else if (event_type == "ProjectCreated") {
                ProcessProjectCreated(payload);
            }
            else if (event_type == "TaskCreated") {
                ProcessTaskCreated(payload);
            }
            else if (event_type == "CommentAdded") {
                ProcessCommentAdded(payload);
            }
            else {
                LOG_WARNING() << "Unknown event type: " << event_type;
            }
            
            StoreEvent("raw_events", event_id, event_type, timestamp, payload);
            
        } catch (const std::exception& e) {
            LOG_ERROR() << "Failed to process message: " << e.what();
        }
    }
    
    LOG_DEBUG() << "Batch processed";
}
 
void EventConsumer::ProcessUserRegistered(const userver::formats::json::Value& payload) {
    int64_t user_id = payload["user_id"].As<int64_t>();
    std::string login = payload["login"].As<std::string>();
    std::string email = payload["email"].As<std::string>();
    std::string first_name = payload["first_name"].As<std::string>();
    std::string last_name = payload["last_name"].As<std::string>();
    
    LOG_INFO() << "User registered: " << login 
               << " (id: " << user_id 
               << ", email: " << email << ")";
}
 
void EventConsumer::ProcessUserLoggedIn(const userver::formats::json::Value& payload) {
    int64_t user_id = payload["user_id"].As<int64_t>();
    std::string login = payload["login"].As<std::string>();
    std::string ip_address = payload["ip_address"].As<std::string>();
    
    LOG_INFO() << "User logged in: " << login 
               << " (id: " << user_id 
               << ", IP: " << ip_address << ")";
}
 
void EventConsumer::ProcessProjectCreated(const userver::formats::json::Value& payload) {
    int64_t project_id = payload["project_id"].As<int64_t>();
    std::string name = payload["name"].As<std::string>();
    std::string key = payload["key"].As<std::string>();
    int64_t owner_id = payload["owner_id"].As<int64_t>();
    
    LOG_INFO() << "Project created: " << name 
               << " (id: " << project_id 
               << ", key: " << key 
               << ", owner: " << owner_id << ")";
}
 
void EventConsumer::ProcessTaskCreated(const userver::formats::json::Value& payload) {
    int64_t task_id = payload["task_id"].As<int64_t>();
    int64_t project_id = payload["project_id"].As<int64_t>();
    std::string title = payload["title"].As<std::string>();
    int priority = payload["priority"].As<int>();
    std::string status = payload["status"].As<std::string>();
    
    LOG_INFO() << "Task created: " << title 
               << " (id: " << task_id 
               << ", project: " << project_id 
               << ", priority: " << priority << ")";
}
 
void EventConsumer::ProcessCommentAdded(const userver::formats::json::Value& payload) {
    std::string comment_id = payload["comment_id"].As<std::string>();
    int64_t task_id = payload["task_id"].As<int64_t>();
    std::string author = payload["author"].As<std::string>();
    std::string text = payload["text"].As<std::string>();
    
    LOG_INFO() << "Comment added: " << comment_id 
               << " (task: " << task_id 
               << ", author: " << author << ")";
}
 
void EventConsumer::StoreEvent(const std::string& collection_name,
                                const std::string& event_id,
                                const std::string& event_type,
                                const std::string& timestamp,
                                const userver::formats::json::Value& payload) {
    try {
        auto collection = mongo_pool_->GetCollection(collection_name);
        
        // JSON payload -> string -> BSON
        std::string payload_str = userver::formats::json::ToString(payload);
        auto payload_bson = userver::formats::bson::FromJsonString(payload_str);
        
        auto bson_doc = userver::formats::bson::MakeDoc(
            "event_id", event_id,
            "event_type", event_type,
            "timestamp", timestamp,
            "payload", payload_bson,
            "processed_at", GetCurrentTimestampISO()
        );
        
        collection.InsertOne(bson_doc);
        
        LOG_DEBUG() << "Event stored in collection: " << collection_name;
        
    } catch (const std::exception& e) {
        LOG_ERROR() << "Failed to store raw event: " << e.what();
    }
}
 
} // namespace event