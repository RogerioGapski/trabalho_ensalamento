cmake_minimum_required(VERSION 3.16)
project(ensalamento_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(lib/ensalamento_core)
add_subdirectory(tests)
add_subdirectory(api)