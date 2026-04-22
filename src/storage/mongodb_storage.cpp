#include "mongodb_storage.hpp"

#include <userver/formats/bson.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/mongo/operations.hpp>

namespace myservice {
namespace storage {

MongoStorage::MongoStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context),
      pool_(context.FindComponent<userver::components::Mongo>("mongo-db-1").GetPool()) {}

userver::storages::mongo::Collection MongoStorage::GetCommentsCollection() const {
    return pool_->GetCollection("comments");
}

void MongoStorage::InsertComment(const models::Comment& comment) {
    GetCommentsCollection().InsertOne(models::CommentToBson(comment));
}

std::vector<models::Comment> MongoStorage::GetCommentsByTask(int task_id) {
    std::vector<models::Comment> result;
    auto filter = userver::formats::bson::MakeDoc("task_id", task_id);
    for (const auto& doc : GetCommentsCollection().Find(filter)) {
        result.push_back(models::CommentFromBson(doc));
    }
    return result;
}

std::optional<models::Comment> MongoStorage::GetCommentById(const std::string& id) {
    auto filter = userver::formats::bson::MakeDoc("_id", userver::formats::bson::Oid(id));
    auto doc = GetCommentsCollection().FindOne(filter);
    if (!doc) return std::nullopt;
    return models::CommentFromBson(*doc);
}

void MongoStorage::AddReply(const std::string& comment_id, const models::Reply& reply) {
    auto collection = GetCommentsCollection();
    auto filter = userver::formats::bson::MakeDoc("_id", userver::formats::bson::Oid(comment_id));
    auto replyDoc = models::ReplyToBson(reply);
    collection.UpdateOne(
        filter,
        userver::formats::bson::MakeDoc(
            "$push", userver::formats::bson::MakeDoc("replies", replyDoc),
            "$set", userver::formats::bson::MakeDoc("updated_at", std::chrono::system_clock::now())
        )
    );
}

void MongoStorage::DeleteComment(const std::string& id) {
    auto oid = userver::formats::bson::Oid(id);
    GetCommentsCollection().DeleteOne(userver::formats::bson::MakeDoc("_id", oid));
}

}  // namespace storage
}  // namespace myservice