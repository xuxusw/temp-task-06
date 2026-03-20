#ifndef MODELS_PROJECT_HPP
#define MODELS_PROJECT_HPP

#include <string>
#include <chrono>

namespace models {
    struct Project {
        int id;
        std::string name;
        std::string description;
        std::string key;
        int owner_id;
        std::chrono::system_clock::time_point created_at;
    };
}

#endif