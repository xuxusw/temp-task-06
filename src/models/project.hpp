#pragma once

#include <string>
// #include <chrono>

#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/user_types.hpp>

namespace models {
    using TimePoint = userver::storages::postgres::TimePointTz;
    struct Project {
        int id;
        std::string name;
        std::string description;
        std::string key;
        int owner_id;
        // std::chrono::system_clock::time_point created_at;
        TimePoint created_at;
    };
}