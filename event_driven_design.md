# Event-Driven Architecture Design

## Architecture
```
                                ┌─────────────┐
                                │   Client    │
                                └──────┬──────┘
                                       │ HTTP
                                       v
                                ┌─────────────┐
                                │  myservice  │
                                │  (Producer) │
                                └──────┬──────┘
                                       │ Kafka message
                                       v
                                ┌─────────────┐
                                │    Kafka    │
                                │  (3 brokers)│
                                └──────┬──────┘
                                       │ Consumer reads
                                       v
                                ┌─────────────┐
                                │   Event     │
                                │   Consumer  │
                                └──────┬──────┘
                                       │ Writes to DB
                                       v
                                ┌─────────────┐
                                │   MongoDB   │
                                └─────────────┘
```

## Components

| Component | Technology | Role |
|-----------|------------|------|
| **myservice** | C++20 + userver | HTTP API + Kafka Producer |
| **Kafka Cluster** | Apache Kafka 7.5.0 | Message broker (3 brokers) |
| **Event Consumer** | Python 3.11 | Consumes events, writes to MongoDB |
| **PostgreSQL** | PostgreSQL 14 | Source of truth (Commands) |
| **MongoDB** | MongoDB 7 | Read model + Event storage |

## Data Flow
```
Client -> myservice (Producer) -> Kafka -> Event Consumer -> БД (MongoDB)
                                    |
                                    V
                              (хранит события)
```

## Поток событий в системе
#### 1. Регистрация пользователя
POST /api/register -> UserRegistered -> user-events topic -> Consumer -> MongoDB (user_events)
#### 2. Логин пользователя
POST /api/login -> UserLoggedIn -> user-events topic -> Consumer -> MongoDB (user_events)
#### 3. Создание проекта
POST /api/projects -> ProjectCreated -> project-events topic -> Consumer -> MongoDB (project_events)
#### 4. Создание задачи
POST /api/projects/{id}/tasks -> TaskCreated -> task-events topic -> Consumer -> MongoDB (task_events + task_analytics)
#### 5. Добавление комментария
POST /api/tasks/{id}/comments -> CommentAdded -> comment-events topic -> Consumer -> MongoDB (comment_events)

## Гарантии доставки сообщений
| Гарантия | Реализация |
|----------|------------|
| **At-least-once** | Producer: `acks=all`, `retries=3`; Consumer: ручной commit после обработки |
| **Идемпотентность** | Consumer использует `event_id` как unique index в MongoDB |
| **Replication** | replication_factor=3 на всех топиках |

Exactly-once требует двухфазного коммита между Kafka и MongoDB, что усложняет систему и снижает производительность.

## CQRS
### Разделение операций
| Тип | Операции | Хранилище |
|-----|----------|-----------|
| **Command (Write)** | CREATE/UPDATE проектов, задач, комментариев | PostgreSQL (source of truth) |
| **Query (Read)** | GET проектов, задач, комментариев | MongoDB + In-memory Cache |

### Синхронизация Read/Write моделей через события
| Command (Write) | Event | Read Model Update |
|-----|----------|-----------|
|POST /api/projects|ProjectCreated|Consumer обновляет project_events collection|
|POST /api/tasks|TaskCreated|Consumer обновляет task_analytics collection|
|POST /api/comments|CommentAdded|Consumer обновляет comment_events collection + инвалидирует кеш|

