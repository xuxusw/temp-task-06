#pragma once

#include <memory>
#include <string>
#include <userver/components/component_context.hpp>
#include <userver/kafka/producer_component.hpp>
#include "event_types.hpp"

namespace event {

class EventProducer {
public:
    EventProducer(const userver::components::ComponentContext& context);
    
    void PublishUserRegistered(const UserRegisteredEvent& event, 
                                const std::string& trace_id = "");
    void PublishUserLoggedIn(const UserLoggedInEvent& event,
                              const std::string& trace_id = "");
    void PublishProjectCreated(const ProjectCreatedEvent& event,
                                const std::string& trace_id = "");
    void PublishTaskCreated(const TaskCreatedEvent& event,
                             const std::string& trace_id = "");
    void PublishCommentAdded(const CommentAddedEvent& event,
                              const std::string& trace_id = "");

private:
    void Publish(const std::string& topic,
                 const std::string& key,
                 const std::string& event_type,
                 const userver::formats::json::Value& payload,
                 const std::string& trace_id);
    
    std::string GenerateEventId();
    std::string GetCurrentTimestampISO();
    
    const userver::kafka::Producer& producer_; // const
};

} // namespace event