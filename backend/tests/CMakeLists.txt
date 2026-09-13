cmake_minimum_required(VERSION 3.16)

add_executable(testes_unitarios
    PrincipalTestes.cpp
    unitarios/TestesRegraRB01ChoqueSala.cpp
    unitarios/TestesRegraRB02DivisaoSala.cpp
    unitarios/TestesRegraRB03Capacidade.cpp
    unitarios/TestesRegraRB04SalaIndisponivel.cpp
    unitarios/TestesRegraRB05Recursos.cpp
    unitarios/TestesRegraRB06Acessibilidade.cpp
    unitarios/TestesRegraRB07ChoqueProfessor.cpp
    unitarios/TestesRegraRB08Revalidacao.cpp
    unitarios/TestesCenariosSemSalaValida.cpp
    unitarios/TestesAlocadorHeuristica.cpp
)

target_include_directories(testes_unitarios PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(testes_unitarios PRIVATE ensalamento_core)

add_test(NAME testes_unitarios COMMAND testes_unitarios)