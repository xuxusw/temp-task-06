#include "event_producer.hpp"
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <userver/logging/log.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace event {

EventProducer::EventProducer(const userver::components::ComponentContext& context)
    : producer_(context.FindComponent<userver::kafka::ProducerComponent>("kafka-producer").GetProducer()) {
    LOG_INFO() << "EventProducer initialized";
}

std::string EventProducer::GenerateEventId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex = "0123456789abcdef";
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (char& c : uuid) {
        if (c == 'x') c = hex[dis(gen)];
        else if (c == 'y') c = hex[4 + dis(gen) % 4];
    }
    return uuid;
}

std::string EventProducer::GetCurrentTimestampISO() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count() << "Z";
    return ss.str();
}

void EventProducer::Publish(const std::string& topic,
                             const std::string& key,
                             const std::string& event_type,
                             const userver::formats::json::Value& payload,
                             const std::string& trace_id) {
    userver::formats::json::ValueBuilder message_builder;
    message_builder["event_id"] = GenerateEventId();
    message_builder["event_type"] = event_type;
    message_builder["timestamp"] = GetCurrentTimestampISO();
    message_builder["version"] = "1.0";
    message_builder["payload"] = payload;
    
    userver::formats::json::ValueBuilder metadata_builder;
    metadata_builder["trace_id"] = trace_id.empty() ? GenerateEventId() : trace_id;
    metadata_builder["source_service"] = "myservice";
    message_builder["metadata"] = metadata_builder.ExtractValue();
    
    // get Value and convert to string
    userver::formats::json::Value msg_value = message_builder.ExtractValue();
    // std::string msg_string = userver::formats::json::ToString(msg_value);
    std::string msg_string = msg_value.As<std::string>();
    
    try {
        // userver::kafka::Producer::Send отправляет сообщение
        // и приостанавливает корутину до подтверждения доставки
        producer_.Send(topic, key, msg_string);
        LOG_INFO() << "Event published: " << event_type << " (topic: " << topic << ")";
    } catch (const std::exception& e) {
        LOG_ERROR() << "Failed to publish event " << event_type << ": " << e.what();
    }
}

void EventProducer::PublishUserRegistered(const UserRegisteredEvent& event,
                                           const std::string& trace_id) {
    Publish("user-events", 
            std::to_string(event.user_id),
            "UserRegistered",
            event.ToPayload(),
            trace_id);
}

void EventProducer::PublishUserLoggedIn(const UserLoggedInEvent& event,
                                         const std::string& trace_id) {
    Publish("user-events",
            std::to_string(event.user_id),
            "UserLoggedIn",
            event.ToPayload(),
            trace_id);
}

void EventProducer::PublishProjectCreated(const ProjectCreatedEvent& event,
                                           const std::string& trace_id) {
    Publish("project-events",
            std::to_string(event.project_id),
            "ProjectCreated",
            event.ToPayload(),
            trace_id);
}

void EventProducer::PublishTaskCreated(const TaskCreatedEvent& event,
                                        const std::string& trace_id) {
    Publish("task-events",
            std::to_string(event.task_id),
            "TaskCreated",
            event.ToPayload(),
            trace_id);
}

void EventProducer::PublishCommentAdded(const CommentAddedEvent& event,
                                         const std::string& trace_id) {
    Publish("comment-events",
            event.comment_id,
            "CommentAdded",
            event.ToPayload(),
            trace_id);
}

} // namespace event