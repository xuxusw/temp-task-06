# myservice

C++ service that uses [userver framework](https://github.com/userver-framework/userver).

## JWT Implementation
Based on: https://github.com/Yadroff/userver_jwt_checker

## Models: 
* User
* Project
* Task

## API Endpoints:
| Метод | Endpoint | Описание | Аутентификация |
|-------|----------|----------|----------------|
| POST | `/api/register` | Регистрация пользователя | Не требуется |
| POST | `/api/login` | Вход, получение JWT токена | Не требуется |
| GET | `/api/users/search` | Поиск пользователей по логину или имени/фамилии | Требуется |
| POST | `/api/projects` | Создание нового проекта | Требуется |
| GET | `/api/projects` | Получение списка всех проектов | Требуется |
| POST | `/api/projects/{id}/tasks` | Создание задачи в проекте | Требуется |

## Technology Stack:
* C++20 + userver
* jwt-cpp library v0.7.2 
* OpenAPI 3.0
* Docker 

# Testing 

## User register
![Postman register](./screenshots/Screenshot_1.png)

## User login
![Postman login](./screenshots/Screenshot_2.png)

## jwt.io jwt check
![Jwt check](./screenshots/jwtio_decoder.png)

## User login wrong password
![Postman login](./screenshots/Screenshot_3.png)

## User login wrong login
![Postman login](./screenshots/Screenshot_4.png)

## User login missing password 
![Postman login](./screenshots/Screenshot_5.png)

## User register user already exists
![Postman register](./screenshots/Screenshot_6.png)

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

## Create (post) a task in non-existent project
![Task handlers check](./screenshots/Screenshot_12.png)

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