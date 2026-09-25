cmake_minimum_required(VERSION 3.16)
project(ensalamento_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ensalamento_core
    src/Validador.cpp
    src/RastreadorAlteracoes.cpp
    src/IndiceAdequacao.cpp
    src/Alocador.cpp
)

target_include_directories(ensalamento_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(ensalamento_core PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)