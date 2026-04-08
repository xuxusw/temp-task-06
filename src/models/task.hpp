#pragma once

#include <string>
// #include <chrono>
#include <optional>

#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/user_types.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>


namespace models {

    using TimePoint = userver::storages::postgres::TimePointTz;

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
        std::optional<int> assignee_id; // can be NULL 
        int creator_id; 
        int priority;
        TimePoint created_at;
        // std::optional<std::chrono::system_clock::time_point> updated_at;
        std::optional<TimePoint> updated_at;
    };

} // namespace models

// namespace userver::storages::postgres {

//     template <>
//     struct io::CppToUserPg<models::TaskStatus> : io::EnumMappingBase<models::TaskStatus> {
//         static constexpr DBTypeName postgres_name = "task_status";
        
//         static constexpr auto enumerators = [] (auto selector) {
//             return selector()
//                 .Case("TODO", models::TaskStatus::ToDo)
//                 .Case("IN_PROGRESS", models::TaskStatus::InProgress)
//                 .Case("DONE", models::TaskStatus::Review)
//                 .Case("CANCELLED", models::TaskStatus::Done);
//         };
//     };

// } // namespace userver::storages::postgresv


// https://developer.atlassian.com/cloud/jira/platform/rest/v3/api-group-issues/#api-group-issues на будущее