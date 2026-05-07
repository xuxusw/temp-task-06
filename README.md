# myservice

Оптимизация производительности через кеширование и rate limiting (вариант 8 "Управление проектами").

This work adds cache alrogithm and rate limiting to the REST API service with PostgreSQL database and partial Mongo database.

C++ service for project and task management that uses [userver framework](https://github.com/userver-framework/userver).

## Technology Stack:
* C++20 + userver framework
* JWT Authentication (jwt-cpp v0.7.2)
* PostgreSQL 14
* MongoDB 7
* OpenAPI 3.0
* Docker + Docker Compose

## Результаты оптимизации производительности (cache and rate limiting)

### Выполненные оптимизации
| Компонент | Реализация | Стратегия | TTL |
|-----------|-----------|-----------|-----|
| Кеш проектов | In-memory Cache | Cache-Aside | 300 сек |
| Кеш комментариев | In-memory Cache | Cache-Aside | 60 сек |
| Rate limit для /login | Sliding Window | 10 запросов/мин на IP | - |

### Сравнение производительности
| Endpoint | RPS (до) | RPS (после) | Latency avg (до) | Latency avg (после) |
|----------|----------|-------------|------------------|---------------------|
| GET /api/projects | 1079 | 1791 (+66%) | 9.34ms | 6.58ms (-30%) |
| GET /api/users/search | 713 | 877 (+23%) | 5.59ms | 4.57ms (-18%) |
| POST /api/login | 716 | контролируется | 5.02ms | 3.50ms (-30%) |

### Rate limiting параметры

- **Алгоритм:** Sliding Window
- **Лимит:** 5 запросов в минуту на IP
- **HTTP код:** 429 Too Many Requests
- **Заголовки:** X-RateLimit-Limit, X-RateLimit-Remaining, X-RateLimit-Reset, Retry-After

### Cache hit rate (по логам)

- GET /api/projects: множественные `Cache HIT` подтверждены
- GET /api/comments: множественные `Cache HIT` подтверждены

### Новые файлы:
```
myservice/
├── scripts/
│   ├── results/
│   │   ├── benchmark_before.txt (2)
│   │   ├── benchmark_after.txt (4)
│   │   ├── logs.txt
│   ├── after_opt_script.txt
│   ├── before_opt_script.txt
│   ├── benchmark_script.txt
│   ├── post_login.lua
├── src/
│   ├── cache/
│   │   ├── cache_manager.cpp
│   │   ├── cache_manager.hpp
│   ├── rate_limit/
│   │   ├── sliding_window.cpp
│   │   ├── sliding_window.hpp
├── perfomance_analysis.md
├── perfomance_design.md
```

plus some changes in project, comment and auth handlers and new method in mongodb_storage

Caching and rate limiting strategy described in `perfomance_design.md`. Results in `perfomance_analysis.md` and in `scripts/results/...`. 

## How to test:
Run containers:
```
docker compose down
docker compose build --no-cache myservice
docker compose up -d
# or docker compose up (to see logs)
```

Login and get a token:
```
curl -v -X POST http://localhost:8082/api/login   -H "Content-Type: application/json"   -d '{
    "login": "testuser",
    "password": "123456"
  }'
```
```
TOKEN="<your-token>"
```

Copy and paste commands from `scripts` dir.

## MongoDB

Описание содержится в `schema_design.md`.  
Новые файлы:
```
myservice/
├── mongodb/
│   ├── data.js
│   ├── docker_commands.md
│   ├── queries.js
│   ├── schema_design.md
│   ├── validation.js
├── src/
│   ├── handlers/
│   │   ├── comment_handlers.cpp
│   │   ├── comment_handlers.hpp
│   ├── models/
│   │   ├── comment.hpp
│   ├── storage/
│   │   ├── mongodb_storage.cpp
│   │   ├── mongodb_storage.hpp
```

Для основных сущностей (`users`, `projects`, `tasks`) в MongoDB созданы коллекции с тестовыми данными (`data.js`), валидацией (`validation.js`) и CRUD-примерами (`queries.js`). 

Добавлены новые сущности `comments`, `task_history` и `notifications` для демонстрации работы с неструктурированными данными. Для них тоже созданы коллекции с тестовыми данными (`data.js`), валидацией (`validation.js`) и CRUD-примерами (`queries.js`). Они прописаны для демонстрации сценариев, где документная модель более удобна:  
comments - иерархическая структура (вложенные replies);  
task_history - лог изменений (много записей, не нагружают основную БД);  
notifications - разные типы уведомлений с разными полями.  

`replies` вложены в документ комментария `comments` (embedded).  
`metadata` вложены в документ уведомления `notifications` (embedded).

## Docker run
Start the container:
```
docker compose up -d mongodb
```
Run the command to see collections made: 
```
docker compose exec mongodb mongosh myservice_mongo --eval "show collections"
```
Enter the container to test queries:
```
docker compose exec mongodb mongosh myservice_mongo
```
Now you can run commands from `queries.js`.  
After you are done, stop and delete the container:
```
docker compose down -v
```
## MongoDB API

There is API integration with Mongo for `comments` collection. Uses new `comment_handlers.hpp/cpp` with support for nested reply structure using embedded documents. Each comment document contains a `replies` array with nested reply objects (author, text, timestamp), avoiding separate JOIN operations.  

**API endpoints** (implemented in `comment_handlers.hpp/cpp`):
  - `POST /api/tasks/{taskId}/comments` - Add comment to task
  - `GET /api/tasks/{taskId}/comments` - Get all comments with nested replies
  - `POST /api/comments/{commentId}/replies` - Add reply to existing comment

API integration for `users`, `projects`, `tasks` remains in PostgeSQL. 

To get a token for login you need to run postgres container as well. These commands allow to run all the containers needed:
```
docker compose down
docker compose build --no-cache myservice
docker compose up -d
# or docker compose up (to see logs)
```
```
docker compose exec mongodb mongosh myservice_mongo --eval "show collections"
```
```
curl http://127.0.0.1:8082/hello?name=userver
```
```
docker compose exec postgres pg_isready -U myservice
```
```
docker compose exec postgres psql -U myservice -d myservice_db -c "\dt"
```
There is testuser already registered in postgres database, so you can just login:
```
curl -v -X POST http://localhost:8082/api/login   -H "Content-Type: application/json"   -d '{
    "login": "testuser",
    "password": "123456"
  }'
```
```
TOKEN="<your-token>"
```
#### POST /api/tasks/{taskId}/comments
```
curl -X POST "http://localhost:8082/api/tasks/1/comments" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "author": "Тестовый Пользователь",
    "text": "Это тестовый комментарий из curl"
  }'
```
#### see all comments
```
docker compose exec mongodb mongosh myservice_mongo --eval "db.comments.find().pretty()"
```
#### change <comment_id> to real comment ObjectId from Mongo
COMMENT_ID="<comment_id>"  
for example:  
COMMENT_ID="69ecc862cd332588e20220b5"
``` 
curl -X POST "http://localhost:8082/api/comments/$COMMENT_ID/replies" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "author": "Ответчик",
    "text": "Это ответ на комментарий"
  }'
```  
#### see all comments again (after changing one)
```
docker compose exec mongodb mongosh myservice_mongo --eval "db.comments.find().pretty()"
```

## PostgeSQL
### 1. Database Schema Design (`schema.sql`)
- **Tables:** `users`, `projects`, `tasks`
- **Primary Keys:** `id SERIAL PRIMARY KEY` for all tables
- **Foreign Keys:** `CASCADE`/`SET NULL`/`RESTRICT`
- **Constraints:** `NOT NULL`, `UNIQUE`, `CHECK` (status validation, priority range 1-5)
- **Soft Delete:** `is_deleted` flag for users (history preservation)
- **Cascade Rules:**
  - `tasks.project_id -> projects.id ON DELETE CASCADE` (tasks deleted when project is deleted)
  - `tasks.assignee_id -> users.id ON DELETE SET NULL` (tasks remain, assignee becomes NULL)
  - `tasks.creator_id -> users.id ON DELETE RESTRICT` (tasks remain, creator remains in history. SET NULL is not quite good cause it wont keep the info about who created the task. so RESTRICT)

### 2. Indexes for Query Optimization
| Index | Table | Columns | Purpose |
|-------|-------|---------|---------|
| `idx_users_login` | users | login (WHERE is_deleted = FALSE) | Fast login lookup |
| `idx_users_first_name_last_name` | users | first_name, last_name | Name search |
| `idx_projects_name` | projects | name | Project search by name |
| `idx_projects_owner_id` | projects | owner_id | JOIN with users |
| `idx_tasks_project_id` | tasks | project_id | Filter tasks by project |
| `idx_tasks_assignee_id` | tasks | assignee_id | Filter tasks by assignee |
| `idx_tasks_creator_id` | tasks | creator_id | Filter tasks by creator |
| `idx_tasks_status` | tasks | status | Filter by task status |
| `idx_tasks_project_priority` | tasks | project_id, priority DESC | Sorting by priority within project |
| `idx_users_name_trgm` | users | first_name, last_name (GIN) | Pattern matching with `LIKE '%...%'` |

### 3. Test Data 
`data.sql`:
- **Users:** 10 records 
- **Projects:** 10 records across different owners
- **Tasks:** 10 records with various statuses and priorities

`bigger_data.sql`:
- **Users:** 10000 records 
- **Projects:** 1000 records across different owners
- **Tasks:** 5000 records with various statuses and priorities

### 4. SQL Queries (`queries.sql`)
All API operations implemented as SQL
| Operation | SQL Query |
|-----------|-----------|
| Create user | `INSERT INTO users ... RETURNING id` |
| Find user by login | `SELECT ... WHERE login = $1 AND is_deleted = FALSE` |
| Search by name mask | `SELECT ... WHERE first_name ILIKE '%...%'` |
| Create project | `INSERT INTO projects ... RETURNING id` |
| Search project by name | `SELECT ... WHERE name ILIKE '%...%'` |
| Get all projects | `SELECT ... ORDER BY created_at DESC` |
| Create task | `INSERT INTO tasks ... RETURNING id` |
| Get tasks by project | `SELECT ... LEFT JOIN users ... WHERE project_id = $1` |
| Get task by ID | `SELECT ... JOIN projects, users ... WHERE task.id = $1` |
| Update task status | `UPDATE tasks SET status = $1, updated_at = NOW()` |
| Project statistics | `COUNT(CASE WHEN status = 'TODO' ...) GROUP BY project` |
| Soft delete user | `UPDATE users SET is_deleted = TRUE WHERE id = $1` |

### 5. Optimization Analysis (`optimization.md`)
EXPLAIN ANALYZE results before and after index creation
| Query | W/o index | With index | Query acceleration |
|-------|------|-------------|-------------|
| search by login | 0.101 ms (seq scan) | 0.033 ms (index scan) | ~3x |
| search by mask | 0.426 ms (seq scan) | 0.070 ms (bitmap heap scan) | ~6x |
| JOIN tasks->users | 2.501 ms (seq scan) | 0.075 ms (index scan) | ~33x |

### 6. API to PostgreSQL Integration
The C++ API (`postgres_storage.cpp`) now uses `userver::storages::postgres::Cluster` to execute SQL queries instead of in-memory storage. All handlers (`RegisterHandler`, `LoginHandler`, `UserSearchHandler`, etc.) work transparently with the database.

## How to Run
```
git clone -b task-03 --single-branch https://github.com/xuxusw/myservice.git
```
```
cd myservice
```
```
docker compose up -d
```
Check running containers:
```
docker compose ps
```
Exec SQL scripts:
```
docker compose exec -T postgres psql -U myservice -d myservice_db < postgresql/schemas/schema.sql
```
```
docker compose exec -T postgres psql -U myservice -d myservice_db < postgresql/schemas/data.sql
```
(you can also exec `bigger_data.sql`. but be prepaired that it'll make longer tables)  
Move into myservice_db for running SQL commands:
```
docker compose exec postgres psql -U myservice -d myservice_db
```
Check created tables:
```
SELECT * FROM users;
```
```
SELECT * FROM projects;
```
```
SELECT * FROM tasks;
```
Run any SQL commands you want.

## Basic info about the project:
## Data models  
### User
| Field | Data Type | Description |
|-------|------|-------------|
| `id` | int | Unique identifier |
| `login` | string | Unique login |
| `password_hash` | string | SHA256 hash |
| `first_name` | string | First name |
| `last_name` | string | Last name |
| `email` | string | Email address |
| `created_at` | datetime | Registration timestamp |

### Project
| Field | Data Type | Description |
|-------|------|-------------|
| `id` | int | Unique identifier |
| `name` | string | Project name |
| `description` | string | Project description |
| `key` | string | Short project code (e.g., "PRJ1") |
| `owner_id` | int | Creator user ID |
| `created_at` | datetime | Creation timestamp |

### Task
| Field | Data Type | Description |
|-------|------|-------------|
| `id` | int | Unique identifier |
| `title` | string | Task title |
| `description` | string | Task description |
| `status` | enum | `TODO` / `IN_PROGRESS` / `REVIEW` / `DONE` |
| `project_id` | int | Parent project ID |
| `assignee_id` | int? | Assigned user ID (optional) |
| `creator_id` | int | Creator user ID |
| `priority` | int | 1-5 priority level |
| `created_at` | datetime | Creation timestamp |
| `updated_at` | datetime? | Last update timestamp |


## API Endpoints
| Method | Endpoint | Description | Auth |
|--------|----------|-------------|------|
| POST | `/api/register` | User registration | No Auth |
| POST | `/api/login` | Login → JWT token | No Auth |
| GET | `/api/users/search` | Search users by login/name | Bearer Token |
| POST | `/api/projects` | Create project | Bearer Token |
| GET | `/api/projects` | List all projects | Bearer Token |
| POST | `/api/projects/{id}/tasks` | Create task in project | Bearer Token |

## JWT Implementation

JWT (HS256) with 24-hour expiration.

Based on: https://github.com/Yadroff/userver_jwt_checker.

**Files:**
- `src/auth/jwt.hpp/cpp` – token generation
- `src/auth/jwt_auth_checker.hpp/cpp` – token validation
- `src/auth/jwt_auth_factory.hpp/cpp` – factory registration

**JWT Token Structure:**
```json
{
  "iss": "project-manager",
  "user_id": "<user_id>",
  "iat": "<issued_at>",
  "exp": "<expires_at>"
}
```

**Usage:**
Authorization: Bearer <token>

# Architecture
## Storage Layer
* Custom in-memory storage with std::unordered_map
* Thread-safe via userver::engine::Mutex
* Supports partial matching (mask) for first name and last name
* Data is not persisted across service restarts

## Authentication Layer
* JwtChecker implements AuthCheckerBase (userver's auth framework)
* JwtAuthComponent manages secret key from config
* JwtAuthCheckerFactory registers jwt-auth type

## Project Structure
```
myservice/
├── postgresql/
│   └── schemas/
│       ├── schema.sql          # Database schema (tables, indexes, constraints)
│       ├── data.sql            # Test data (10+ records)
│       ├── bigger_data.sql     # Bigger test data (5000+ records)
├── queries.sql                 # SQL queries for all API operations
├── optimization.md             # EXPLAIN ANALYZE results and optimization analysis
├── optimization_bigger_data.md # EXPLAIN ANALYZE results for bigger data
├── src/                        # API source code
│   ├── models/                 # User, Project, Task models
│   ├── storage/                # PostgresStorage (replaces InMemoryStorage)
│   ├── auth/                   # JWT authentication
│   └── handlers/               # HTTP handlers
├── configs/                    # YAML configuration files
├── Dockerfile
├── docker-compose.yaml
├── openapi.yaml
└── README.md
```
**Note:** `auth_middleware.hpp`/`auth_middleware.cpp` in `/handlers` are not currently used. Authentication is implemented with userver's built-in AuthCheckerBase mechanism.  

# Docker run
```
# Build image
docker build -t myservice .

# Run container (without Swagger UI)
docker run -d -p 8082:8080 --name myservice_test myservice 

# Or using docker compose (with Swagger UI)
docker compose up -d
(docker-compose up -d)
```
* Service available at: http://localhost:8082 
* Swagger UI documentation available at: http://localhost:8083

**Note:** Swagger UI in browser may show CORS errors. For live testing, use Postman (import `openapi.yaml`) or curl commands.

## Examples
### Register
```
curl -X POST http://localhost:8082/api/register \
  -H "Content-Type: application/json" \
  -d '{
    "login": "user",
    "password": "123456",
    "first_name": "Ivan",
    "last_name": "Ivanov",
    "email": "ivan@example.com"
  }'
```
### Login 
```
curl -X POST http://localhost:8082/api/login \
  -H "Content-Type: application/json" \
  -d '{
    "login": "user",
    "password": "123456"
  }'
```
### Search Users (Authenticated)
```
TOKEN="<your-token>"

# login search
curl -X GET "http://localhost:8082/api/users/search?login=ivan" \
  -H "Authorization: Bearer $TOKEN"

# first_name and last_name masks search
curl -X GET "http://localhost:8082/api/users/search?first_name=Ivan&last_name=Ivanov" \
  -H "Authorization: Bearer $TOKEN"
```
### Create Project (Authenticated)
```
curl -X POST http://localhost:8082/api/projects \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "name": "First Project",
    "description": "This is a test project",
    "key": "PRJ1"
  }'
```
### Get Projects (Authenticated)
```
curl -X GET http://localhost:8082/api/projects \
  -H "Authorization: Bearer $TOKEN"
```
### Create Task (Authenticated)
The project with the using project id must exist.
(/api/projects/{id}/tasks)

```
curl -X POST http://localhost:8082/api/projects/1/tasks \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "title": "Implement REST API",
    "description": "Create all endpoints",
    "priority": 5
  }'
```

# Test results (Postman)

# Success Cases
## User register
![Postman register](./screenshots/Screenshot_1.png)

## User login
![Postman login](./screenshots/Screenshot_2.png)

## jwt.io jwt check
![Jwt check](./screenshots/jwtio_decoder.png)

## Get user by login
![User handlers check](./screenshots/Screenshot_7.png)

## Get user by first name and last name masks
![User handlers check](./screenshots/Screenshot_8.png)

## Create (post) a project
![Project handlers check](./screenshots/Screenshot_9.png)

## Get projects list
![Project handlers check](./screenshots/Screenshot_10.png)

## Create (post) a task
![Task handlers check](./screenshots/Screenshot_11.png)

# Error Cases
## User login wrong password
![Postman login](./screenshots/Screenshot_3.png)

## User login wrong login
![Postman login](./screenshots/Screenshot_4.png)

## User login missing password 
![Postman login](./screenshots/Screenshot_5.png)

## User register user already exists
![Postman register](./screenshots/Screenshot_6.png)

## Create (post) a task in non-existent project
![Task handlers check](./screenshots/Screenshot_12.png)


# OpenAPI Documentation
OpenAPI 3.0 specification: [`openapi.yaml`](openapi.yaml).
Created with VS Code OpenAPI extension and validated in Swagger Editor. 

View interactive documentation at `http://localhost:8083` Swagger UI (after `docker compose up -d`).
Or view without running service [Swagger Editor](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/xuxusw/myservice/main/openapi.yaml) (CORS errors. For live testing, use Postman (import `openapi.yaml`) or curl commands.)


## Makefile

`PRESET` is either `debug`, `release`, or if you've added custom presets in `CMakeUserPresets.json`, it
can also be `debug-custom`, `release-custom`.

* `make cmake-PRESET` - run cmake configure, update cmake options and source file lists
* `make build-PRESET` - build the service
* `make test-PRESET` - build the service and run all tests
* `make start-PRESET` - build the service, start it in testsuite environment and leave it running
* `make install-PRESET` - build the service and install it in directory set in environment `PREFIX`
* `make` or `make all` - build and run all tests in `debug` and `release` modes
* `make format` - reformat all C++ and Python sources
* `make dist-clean` - clean build files and cmake cache
* `make docker-COMMAND` - run `make COMMAND` in docker environment
* `make docker-clean-data` - stop docker containers


## License

Based on the template distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver/blob/develop/LICENSE)
and [CLA](https://github.com/userver-framework/userver/blob/develop/CONTRIBUTING.md). Services based on the template may change the license and CLA.