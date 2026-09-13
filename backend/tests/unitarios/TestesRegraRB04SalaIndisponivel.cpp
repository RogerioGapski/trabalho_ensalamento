#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb04_sala_em_manutencao_gera_violacao) {
    EstadoEnsalamento estado;
    Sala sala = criarSala("sala-1", "predio-1", "campus-1", 50);
    sala.status = StatusSala::EmManutencao;
    estado.salas.push_back(sala);
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB04));
}

TESTE(rb04_sala_desativada_gera_violacao) {
    EstadoEnsalamento estado;
    Sala sala = criarSala("sala-1", "predio-1", "campus-1", 50);
    sala.status = StatusSala::Desativada;
    estado.salas.push_back(sala);
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB04));
}

TESTE(rb04_sala_ativa_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB04));
}