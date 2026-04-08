#pragma once

#include <optional>
#include <vector>
#include <string>

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/user.hpp"
#include "models/project.hpp"
#include "models/task.hpp"

namespace myservice {
namespace storage {

class PostgresStorage final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "postgres-storage";

    PostgresStorage(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

    std::optional<models::User> CreateUser(const models::User& user);
    std::optional<models::User> GetUserByLogin(const std::string& login);
    std::vector<models::User> SearchUsersByLogin(const std::string& login_mask);
    std::vector<models::User> SearchUsersByFirstNameLastName(
        const std::string& first_name_mask,
        const std::string& last_name_mask);

    std::optional<models::Project> CreateProject(const models::Project& project);
    std::optional<models::Project> GetProjectById(int id);
    std::vector<models::Project> GetAllProjects();

    std::optional<models::Task> CreateTask(const models::Task& task);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace storage
} // namespace myservice