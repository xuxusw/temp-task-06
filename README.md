# myservice

Work in progress.

C++ service that uses [userver framework](https://github.com/userver-framework/userver).

# JWT Implementation
Based on: https://github.com/Yadroff/userver_jwt_checker

# Models: 
* User
* Project
* Task

# Current Endpoints:
* User /api/register
* User /api/login

# Technology Stack:
* C++20 + userver
* jwt-cpp library v0.7.2 
* OpenAPI 3.0
* Docker 

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