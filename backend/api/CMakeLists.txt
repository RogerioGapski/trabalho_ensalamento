cmake_minimum_required(VERSION 3.16)
project(ensalamento_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)
find_package(pqxx REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG v0.7.0
)
FetchContent_MakeAvailable(jwt-cpp)

add_executable(ensalamento_api
    src/main.cpp
    src/db/ConexaoBanco.cpp
    src/db/RepositorioEstado.cpp
    src/db/RepositorioVersao.cpp
    src/db/RepositorioMetricas.cpp
    src/db/RepositorioMapa.cpp
    src/db/RepositorioAgendaIcs.cpp
    src/json/SerializadorRelatorio.cpp
    src/rotas/RotasEnsalamento.cpp
    src/rotas/RotasAutenticacao.cpp
    src/rotas/RotasCalendario.cpp
    src/auth/Base64Url.cpp
    src/auth/ConversorJwkParaPem.cpp
    src/auth/ClienteHttp.cpp
    src/auth/ProvedorChavesPublicas.cpp
    src/auth/ValidadorTokenGoogle.cpp
    src/auth/ValidadorTokenMicrosoft.cpp
    src/auth/RepositorioUsuarios.cpp
    src/auth/GerenciadorSessao.cpp
    src/auth/CookieSessao.cpp
    src/calendario/DataUtil.cpp
    src/calendario/IcsBuilder.cpp
    src/calendario/GerenciadorChaveCalendario.cpp
)

target_include_directories(ensalamento_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ensalamento_api PRIVATE
    ensalamento_core
    Crow::Crow
    pqxx::pqxx
    Threads::Threads
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    CURL::libcurl
    jwt-cpp::jwt-cpp
)

target_compile_options(ensalamento_api PRIVATE
    -Wall
    -Wextra
)