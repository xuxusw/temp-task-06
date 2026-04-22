#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>

#include <userver/formats/bson.hpp>
#include <userver/formats/bson/document.hpp>
#include <userver/formats/bson/value.hpp>
#include <userver/formats/bson/value_builder.hpp>

namespace myservice {
namespace models {

struct Reply {
    std::string author;
    std::string text;
    std::chrono::system_clock::time_point created_at;
};

struct Comment {
    std::string id;
    int task_id;
    std::string author;
    int author_id;
    std::string text;
    std::chrono::system_clock::time_point created_at;
    std::optional<std::chrono::system_clock::time_point> updated_at;
    std::vector<Reply> replies;
};

inline Reply ReplyFromBson(const userver::formats::bson::Value& doc) {
    Reply r;
    r.author = doc["author"].As<std::string>();
    r.text = doc["text"].As<std::string>();
    r.created_at = doc["created_at"].As<std::chrono::system_clock::time_point>();
    return r;
}

inline userver::formats::bson::Value ReplyToBson(const Reply& r) {
    userver::formats::bson::ValueBuilder vb;
    vb["author"] = r.author;
    vb["text"] = r.text;
    vb["created_at"] = r.created_at;
    return vb.ExtractValue();
}

inline Comment CommentFromBson(const userver::formats::bson::Value& doc) {
    Comment c;
    c.id = doc["_id"].As<userver::formats::bson::Oid>().ToString();
    c.task_id = doc["task_id"].As<int>();
    c.author = doc["author"].As<std::string>();
    c.author_id = doc["author_id"].As<int>();
    c.text = doc["text"].As<std::string>();
    c.created_at = doc["created_at"].As<std::chrono::system_clock::time_point>();
    if (doc.HasMember("updated_at")) {
        c.updated_at = doc["updated_at"].As<std::chrono::system_clock::time_point>();
    }
    for (const auto& r : doc["replies"]) {
        c.replies.push_back(ReplyFromBson(r));
    }
    return c;
}

inline userver::formats::bson::Value CommentToBson(const Comment& c) {
    userver::formats::bson::ValueBuilder vb;
    vb["task_id"] = c.task_id;
    vb["author"] = c.author;
    vb["author_id"] = c.author_id;
    vb["text"] = c.text;
    vb["created_at"] = c.created_at;
    if (c.updated_at.has_value()) {
        vb["updated_at"] = *c.updated_at;
    }
    
    userver::formats::bson::ValueBuilder replies_builder(userver::formats::bson::ValueBuilder::Type::kArray);
    for (const auto& r : c.replies) {
        replies_builder.PushBack(ReplyToBson(r));
    }
    vb["replies"] = replies_builder.ExtractValue();
    
    return vb.ExtractValue();
}

}  // namespace models
}  // namespace myservice