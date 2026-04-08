#pragma once

#include <string>
// #include <chrono>

#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/user_types.hpp>


namespace models {
    using TimePoint = userver::storages::postgres::TimePointTz;
    struct User {
        int id;
        std::string login;
        std::string password_hash;
        std::string first_name;
        std::string last_name;
        std::string email;
        // std::chrono::system_clock::time_point created_at;
        TimePoint created_at;
        bool is_deleted = false;
    };
}