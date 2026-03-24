#include "in_memory_storage.hpp"
// #include <userver/engine/mutex.hpp>

namespace myservice {
namespace storage {

InMemoryStorage::InMemoryStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {
    // через config можно читать параметры из конфига

    // models::User admin;
    // admin.login = "admin";
    // admin.password_hash = "1234";
    // CreateUser(admin);
}

bool InMemoryStorage::MatchesMask(const std::string& value, const std::string& mask) {
    if (mask.empty()) return true;
    return value.find(mask) != std::string::npos;
}

std::optional<models::User> InMemoryStorage::CreateUser(const models::User& user) {
    // std::scoped_lock lock(mutex_);
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    models::User new_user = user;
    new_user.id = next_user_id_++;
    users_[new_user.id] = new_user;
    
    return new_user;
}

std::optional<models::User> InMemoryStorage::GetUserByLogin(const std::string& login) {
    // std::scoped_lock lock(mutex_);
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    for (const auto& [id, user] : users_) {
        if (user.login == login) {
            return user;
        }
    }
    return std::nullopt;
}

// for later
std::vector<models::User> InMemoryStorage::SearchUsersByLogin(const std::string& login_mask) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    std::vector<models::User> result;
    for (const auto& [id, user] : users_) {
        if (MatchesMask(user.login, login_mask)) {
            result.push_back(user);
        }
    }
    return result;
}

std::vector<models::User> InMemoryStorage::SearchUsersByFirstNameLastName(
    const std::string& first_name_mask, const std::string& last_name_mask) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    std::vector<models::User> result;
    for (const auto& [id, user] : users_) {
        if (MatchesMask(user.first_name, first_name_mask) && 
            MatchesMask(user.last_name, last_name_mask)) {
            result.push_back(user);
        }
    }
    return result;
}

std::optional<models::Project> InMemoryStorage::CreateProject(const models::Project& project) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    models::Project new_project = project;
    new_project.id = next_project_id_++;
    projects_[new_project.id] = new_project;
    
    return new_project;
}

std::vector<models::Project> InMemoryStorage::GetAllProjects() {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    std::vector<models::Project> result;
    for (const auto& [id, project] : projects_) {
        result.push_back(project);
    }
    return result;
}

std::optional<models::Task> InMemoryStorage::CreateTask(const models::Task& task) {
    std::lock_guard<userver::engine::Mutex> lock(mutex_);
    
    models::Task new_task = task;
    new_task.id = next_task_id_++;
    tasks_[new_task.id] = new_task;
    
    return new_task;
}

} // namespace storage
} // namespace myservice