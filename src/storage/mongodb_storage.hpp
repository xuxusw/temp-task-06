#pragma once

#include <string>
#include <vector>
#include <optional>

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/storages/mongo/pool.hpp>
#include <userver/storages/mongo/collection.hpp>

#include "models/comment.hpp"

namespace myservice {
namespace storage {

class MongoStorage final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "mongo-storage";

    MongoStorage(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);

    void InsertComment(const models::Comment& comment);
    std::vector<models::Comment> GetCommentsByTask(int task_id);
    std::optional<models::Comment> GetCommentById(const std::string& id);
    void AddReply(const std::string& comment_id, const models::Reply& reply);
    void DeleteComment(const std::string& id);

private:
    userver::storages::mongo::PoolPtr pool_;
    userver::storages::mongo::Collection GetCommentsCollection() const;
};

}  // namespace storage
}  // namespace myservice