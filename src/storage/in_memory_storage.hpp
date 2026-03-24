#pragma once

#include <unordered_map>
#include <optional>
#include <vector>
#include <string>
#include <string_view>

#include <userver/engine/mutex.hpp>
#include <userver/components/component_base.hpp>

#include "models/user.hpp"
#include "models/project.hpp"
#include "models/task.hpp"

namespace myservice {
namespace storage {
class InMemoryStorage final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "in-memory-storage";
    
    InMemoryStorage(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);
    
    std::optional<models::User> CreateUser(const models::User& user);
    std::optional<models::User> GetUserByLogin(const std::string& login);
    std::vector<models::User> SearchUsersByLogin(const std::string& login_mask);
    std::vector<models::User> SearchUsersByFirstNameLastName(const std::string& first_name_mask, 
                                                       const std::string& last_name_mask);
    
    std::optional<models::Project> CreateProject(const models::Project& project);
    std::vector<models::Project> GetAllProjects();
    
    std::optional<models::Task> CreateTask(const models::Task& task);
    
private:
    std::unordered_map<int, models::User> users_;
    std::unordered_map<int, models::Project> projects_;
    std::unordered_map<int, models::Task> tasks_;
    
    int next_user_id_{1};
    int next_project_id_{1};
    int next_task_id_{1};
    
    mutable userver::engine::Mutex mutex_; // userver mutex
    
    bool MatchesMask(const std::string& value, const std::string& mask); // поиск по маске
};

} // namespace storage
} // namespace myservice