#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace event {

// UserRegistered
struct UserRegisteredEvent {
    int64_t user_id;
    std::string login;
    std::string email;
    std::string first_name;
    std::string last_name;
    
    userver::formats::json::Value ToPayload() const {
        userver::formats::json::ValueBuilder builder;
        builder["user_id"] = user_id;
        builder["login"] = login;
        builder["email"] = email;
        builder["first_name"] = first_name;
        builder["last_name"] = last_name;
        return builder.ExtractValue();
    }
};

// UserLoggedIn
struct UserLoggedInEvent {
    int64_t user_id;
    std::string login;
    std::string ip_address;
    
    userver::formats::json::Value ToPayload() const {
        userver::formats::json::ValueBuilder builder;
        builder["user_id"] = user_id;
        builder["login"] = login;
        builder["ip_address"] = ip_address;
        return builder.ExtractValue();
    }
};

// ProjectCreated
struct ProjectCreatedEvent {
    int64_t project_id;
    std::string name;
    std::string key;
    int64_t owner_id;
    
    userver::formats::json::Value ToPayload() const {
        userver::formats::json::ValueBuilder builder;
        builder["project_id"] = project_id;
        builder["name"] = name;
        builder["key"] = key;
        builder["owner_id"] = owner_id;
        return builder.ExtractValue();
    }
};

// TaskCreated
struct TaskCreatedEvent {
    int64_t task_id;
    int64_t project_id;
    std::string title;
    std::string description;
    int priority;
    int64_t creator_id;
    std::string status = "TODO";
    
    userver::formats::json::Value ToPayload() const {
        userver::formats::json::ValueBuilder builder;
        builder["task_id"] = task_id;
        builder["project_id"] = project_id;
        builder["title"] = title;
        builder["description"] = description;
        builder["priority"] = priority;
        builder["creator_id"] = creator_id;
        builder["status"] = status;
        return builder.ExtractValue();
    }
};

// CommentAdded
struct CommentAddedEvent {
    std::string comment_id;
    int64_t task_id;
    std::string author;
    std::string text;
    
    userver::formats::json::Value ToPayload() const {
        userver::formats::json::ValueBuilder builder;
        builder["comment_id"] = comment_id;
        builder["task_id"] = task_id;
        builder["author"] = author;
        builder["text"] = text;
        return builder.ExtractValue();
    }
};

} // namespace event