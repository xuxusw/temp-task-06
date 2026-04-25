#include "postgres_storage.hpp"

#include <userver/formats/json.hpp>
#include <userver/logging/log.hpp>
// #include <userver/storages/postgres/io/chrono.hpp>
// #include <userver/storages/postgres/io/user_types.hpp>

namespace userver::storages::postgres {

template <>
struct io::CppToUserPg<models::User> {
    static constexpr DBTypeName postgres_name = "public.users";
};

template <>
struct io::CppToUserPg<models::Project> {
    static constexpr DBTypeName postgres_name = "public.projects";
};

template <>
struct io::CppToUserPg<models::Task> {
    static constexpr DBTypeName postgres_name = "public.tasks";
};

} // namespace userver::storages::postgres


namespace myservice {
namespace storage {

namespace {
    // поиск по маске (ILIKE с %...%)
    std::string MakeLikePattern(const std::string& mask) {
        if (mask.empty()) return "%";
        return "%" + mask + "%";
    }
} // namespace

PostgresStorage::PostgresStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}


std::optional<models::User> PostgresStorage::CreateUser(const models::User& user) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        R"(
            INSERT INTO users (login, password_hash, first_name, last_name, email, created_at, is_deleted)
            VALUES ($1, $2, $3, $4, $5, $6, FALSE)
            RETURNING id, login, password_hash, first_name, last_name, email, created_at
        )",
        user.login, user.password_hash, user.first_name, user.last_name, user.email, user.created_at
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

std::optional<models::User> PostgresStorage::GetUserByLogin(const std::string& login) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        R"(
            SELECT id, login, password_hash, first_name, last_name, email, created_at, is_deleted
            FROM users
            WHERE login = $1 AND is_deleted = FALSE
        )",
        login
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

std::vector<models::User> PostgresStorage::SearchUsersByLogin(const std::string& login_mask) {
    std::string pattern = MakeLikePattern(login_mask);
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        R"(
            SELECT id, login, password_hash, first_name, last_name, email, created_at, is_deleted
            FROM users
            WHERE login ILIKE $1 AND is_deleted = FALSE
            LIMIT 100
        )",
        pattern
    );

    return result.AsContainer<std::vector<models::User>>(userver::storages::postgres::kRowTag);
}

std::vector<models::User> PostgresStorage::SearchUsersByFirstNameLastName(
    const std::string& first_name_mask,
    const std::string& last_name_mask) {
    
    std::string first_pattern = MakeLikePattern(first_name_mask);
    std::string last_pattern = MakeLikePattern(last_name_mask);
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        R"(
            SELECT id, login, password_hash, first_name, last_name, email, created_at, is_deleted
            FROM users
            WHERE first_name ILIKE $1 
              AND last_name ILIKE $2 
              AND is_deleted = FALSE
            LIMIT 100
        )",
        first_pattern, last_pattern
    );

    return result.AsContainer<std::vector<models::User>>(userver::storages::postgres::kRowTag);
}


std::optional<models::Project> PostgresStorage::CreateProject(const models::Project& project) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        R"(
            INSERT INTO projects (name, description, key, owner_id, created_at)
            VALUES ($1, $2, $3, $4, $5)
            RETURNING id, name, description, key, owner_id, created_at
        )",
        project.name, project.description, project.key, project.owner_id, project.created_at
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    return result.AsSingleRow<models::Project>(userver::storages::postgres::kRowTag);
}

std::optional<models::Project> PostgresStorage::GetProjectById(int id) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        R"(
            SELECT id, name, description, key, owner_id, created_at
            FROM projects
            WHERE id = $1
        )",
        id
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    return result.AsSingleRow<models::Project>(userver::storages::postgres::kRowTag);
}

std::vector<models::Project> PostgresStorage::GetAllProjects() {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        R"(
            SELECT id, name, description, key, owner_id, created_at
            FROM projects
            ORDER BY created_at DESC
            LIMIT 100
        )"
    );

    return result.AsContainer<std::vector<models::Project>>(userver::storages::postgres::kRowTag);
}


std::optional<models::Task> PostgresStorage::CreateTask(const models::Task& task) {
    std::string status_str = models::TaskStatusToString(task.status);
    auto now = userver::storages::postgres::TimePointTz(std::chrono::system_clock::now());

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        R"(
            INSERT INTO tasks (title, description, status, project_id, creator_id, assignee_id, priority, created_at, updated_at)
            VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)
            RETURNING id, title, description, status, project_id, creator_id, assignee_id, priority, created_at, updated_at
        )",
        task.title, task.description, status_str, task.project_id, task.creator_id,
        task.assignee_id, task.priority, now, now  // updated_at = created_at initially
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    // models::Task
    // return result.AsSingleRow<models::Task>(userver::storages::postgres::kRowTag);
    const auto& row = result[0];
    models::Task new_task;
    new_task.id = row["id"].As<int>();
    new_task.title = row["title"].As<std::string>();
    new_task.description = row["description"].As<std::string>();
    new_task.status = models::TaskStatusFromString(row["status"].As<std::string>());
    new_task.project_id = row["project_id"].As<int>();
    new_task.creator_id = row["creator_id"].As<int>();
    new_task.assignee_id = row["assignee_id"].As<std::optional<int>>();
    new_task.priority = row["priority"].As<int>();
    new_task.created_at = row["created_at"].As<models::TimePoint>();
    new_task.updated_at = row["updated_at"].As<std::optional<models::TimePoint>>();
    
    return new_task;
}

} // namespace storage
} // namespace myservice

