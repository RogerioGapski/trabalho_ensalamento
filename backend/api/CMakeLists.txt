cmake_minimum_required(VERSION 3.16)
project(ensalamento_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)
find_package(libpqxx REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include(FetchContent)
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

add_executable(ensalamento_api
    src/main.cpp
    src/db/ConexaoBanco.cpp
    src/db/RepositorioEstado.cpp
    src/db/RepositorioVersao.cpp
    src/json/SerializadorRelatorio.cpp
    src/rotas/RotasEnsalamento.cpp
)

target_include_directories(ensalamento_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ensalamento_api PRIVATE
    ensalamento_core
    Crow::Crow
    libpqxx::pqxx
    Threads::Threads
    Boost::system
)

target_compile_options(ensalamento_api PRIVATE
    -Wall
    -Wextra
)