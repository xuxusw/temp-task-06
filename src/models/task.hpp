#pragma once

#include <string>
#include <chrono>
#include <optional>

namespace models {

    enum class TaskStatus {
        ToDo,
        InProgress,
        Review,
        Done
    };

    inline std::string TaskStatusToString(TaskStatus status) {
        switch(status){
            case TaskStatus::ToDo: return "TODO";
            case TaskStatus::InProgress: return "IN_PROGRESS";
            case TaskStatus::Review: return "REVIEW";
            case TaskStatus::Done: return "DONE";
            default: return "UNKNOWN";
        }
    }

    inline TaskStatus TaskStatusFromString(const std::string& str) {
        if (str == "TODO") return TaskStatus::ToDo;
        if (str == "IN_PROGRESS") return TaskStatus::InProgress;
        if (str == "REVIEW") return TaskStatus::Review;
        if (str == "DONE") return TaskStatus::Done;
        return TaskStatus::ToDo;
    }

    struct Task {
        int id;
        std::string title;
        std::string description;
        TaskStatus status;
        int project_id;
        int creator_id;
        int priority;
        std::chrono::system_clock::time_point created_at;
        std::optional<std::chrono::system_clock::time_point> updated_at;
    };
}

// https://developer.atlassian.com/cloud/jira/platform/rest/v3/api-group-issues/#api-group-issues на будущее