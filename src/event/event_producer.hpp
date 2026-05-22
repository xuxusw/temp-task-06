#pragma once

#include <memory>
#include <string>
#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>
#include <userver/kafka/producer_component.hpp>
#include "event_types.hpp"

namespace event {

class EventProducer final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "event-producer";
    
    EventProducer(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context);
    
    ~EventProducer() override = default;
    
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
    
    const userver::kafka::Producer& producer_;
};

} // namespace event