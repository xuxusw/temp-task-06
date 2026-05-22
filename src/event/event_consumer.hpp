#pragma once

#include <memory>
#include <string>
#include <userver/components/loggable_component_base.hpp>
#include <userver/kafka/consumer_component.hpp>
#include <userver/kafka/message.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/utils/span.hpp>

namespace event {

class EventConsumer final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "event-consumer";
    
    EventConsumer(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context);
    
    ~EventConsumer() override;
    
private:
    void ProcessBatch(userver::utils::span<const userver::kafka::Message> messages);  // изм
    void ProcessUserRegistered(const userver::formats::json::Value& payload);
    void ProcessUserLoggedIn(const userver::formats::json::Value& payload);
    void ProcessProjectCreated(const userver::formats::json::Value& payload);
    void ProcessTaskCreated(const userver::formats::json::Value& payload);
    void ProcessCommentAdded(const userver::formats::json::Value& payload);
    void StoreEvent(const std::string& collection_name, 
                    const std::string& event_id,
                    const std::string& event_type,
                    const std::string& timestamp,
                    const userver::formats::json::Value& payload);
    
    std::string GetCurrentTimestampISO();
    
    userver::kafka::ConsumerScope consumer_scope_;
    userver::storages::mongo::PoolPtr mongo_pool_;
};

} // namespace event