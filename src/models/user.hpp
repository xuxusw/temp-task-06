#pragma once

#include <string>
#include <chrono>

namespace models {
    struct User {
        int id;
        std::string login;
        std::string password_hash;
        std::string first_name;
        std::string last_name;
        std::string email;
        std::chrono::system_clock::time_point created_at;
    };
}