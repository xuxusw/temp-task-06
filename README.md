# myservice

REST API сервис для управления проектами и задачами (вариант 8 "Управление проектами").

C++ service for project and task management that uses [userver framework](https://github.com/userver-framework/userver).

## Technology Stack:
* C++20 + userver framework
* JWT Authentication (jwt-cpp v0.7.2)
* OpenAPI 3.0
* Docker 

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
├── src/
│   ├── models/          # User, Project, Task
│   ├── storage/         # In-memory storage
│   ├── auth/            # JWT implementation
│   ├── handlers/        # HTTP handlers
│   └── main.cpp
├── configs/             # YAML configs
├── screenshots/         # Postman test results
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
docker run -d -p 8080:8080 --name myservice_test myservice 

# Or using docker compose (with Swagger UI)
docker compose up -d
(docker-compose up -d)
```
* Service available at: http://localhost:8080 
* Swagger UI documentation available at: http://localhost:8081

**Note:** Swagger UI in browser may show CORS errors. For live testing, use Postman (import `openapi.yaml`) or curl commands.

## Examples
### Register
```
curl -X POST http://localhost:8080/api/register \
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
curl -X POST http://localhost:8080/api/login \
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
curl -X GET "http://localhost:8080/api/users/search?login=ivan" \
  -H "Authorization: Bearer $TOKEN"

# first_name and last_name masks search
curl -X GET "http://localhost:8080/api/users/search?first_name=Ivan&last_name=Ivanov" \
  -H "Authorization: Bearer $TOKEN"
```
### Create Project (Authenticated)
```
curl -X POST http://localhost:8080/api/projects \
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
curl -X GET http://localhost:8080/api/projects \
  -H "Authorization: Bearer $TOKEN"
```
### Create Task (Authenticated)
The project with the using project id must exist.
(/api/projects/{id}/tasks)

```
curl -X POST http://localhost:8080/api/projects/1/tasks \
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

View interactive documentation at `http://localhost:8081` Swagger UI (after `docker compose up -d`).
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