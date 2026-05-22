#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component_list.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/congestion_control/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>

#include <userver/storages/mongo/component.hpp>    
#include <userver/storages/postgres/component.hpp> 
#include <userver/ugrpc/server/component_list.hpp> 

#include <userver/utils/daemon_run.hpp>

// #include <userver/server/handlers/server_monitor.hpp>

#include <hello.hpp>
#include <hello_grpc.hpp>     
#include <hello_mongo.hpp>    
#include <hello_postgres.hpp> 

#include "auth/jwt_auth_factory.hpp"
#include "auth/jwt_auth_checker.hpp"
#include "handlers/auth_handlers.hpp"
#include "handlers/user_handlers.hpp"
#include "handlers/project_handlers.hpp"
#include "handlers/task_handlers.hpp"
#include "handlers/comment_handlers.hpp"
// #include "handlers/mongo_user_handlers.hpp"
// #include "storage/in_memory_storage.hpp"
#include "storage/postgres_storage.hpp"
#include "storage/mongodb_storage.hpp"
#include "cache/cache_manager.hpp"

#include <userver/kafka/producer_component.hpp>
#include <userver/kafka/consumer_component.hpp>
// #include <userver/kafka/component.hpp>
#include "event/event_producer.hpp"
#include "event/event_consumer.hpp"
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

int main(int argc, char* argv[]) {

    userver::server::handlers::auth::RegisterAuthCheckerFactory<myservice::auth::JwtAuthCheckerFactory>();

    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<userver::server::handlers::Ping>()
            .Append<userver::components::TestsuiteSupport>()
            .AppendComponentList(userver::clients::http::ComponentList())
            .Append<userver::clients::dns::Component>()
            .Append<userver::server::handlers::TestsControl>()
            .Append<userver::congestion_control::Component>()
            .Append<myservice::Hello>()
            .Append<userver::components::Postgres>("postgres-db-1")
            .Append<myservice::HelloPostgres>()
            .Append<userver::components::Mongo>("mongo-db-1")
            .Append<myservice::HelloMongo>()
            .AppendComponentList(userver::ugrpc::server::MinimalComponentList())
            .Append<myservice::HelloGrpc>()
            .Append<myservice::auth::JwtAuthComponent>()
            // .Append<myservice::storage::InMemoryStorage>()
            .Append<myservice::storage::PostgresStorage>()
            .Append<myservice::storage::MongoStorage>()
            .Append<myservice::cache::CacheManager>()
            .Append<myservice::handlers::RegisterHandler>()
            .Append<myservice::handlers::LoginHandler>()
            .Append<myservice::handlers::UserSearchHandler>()
            .Append<myservice::handlers::CreateProjectHandler>()
            .Append<myservice::handlers::GetProjectsHandler>()
            .Append<myservice::handlers::CreateTaskHandler>()
            .Append<myservice::handlers::AddCommentHandler>()
            .Append<myservice::handlers::GetCommentsHandler>()
            .Append<myservice::handlers::AddReplyHandler>()
            // .Append<userver::server::handlers::ServerMonitor>()
            // .Append<userver::kafka::KafkaComponent>()
            .Append<userver::components::Secdist>()
            .Append<userver::components::DefaultSecdistProvider>()
            .Append<userver::kafka::ProducerComponent>()
            .Append<userver::kafka::ConsumerComponent>()
            .Append<event::EventProducer>("event-producer")
            .Append<event::EventConsumer>("event-consumer") 
        ;

    return userver::utils::DaemonMain(argc, argv, component_list);
}