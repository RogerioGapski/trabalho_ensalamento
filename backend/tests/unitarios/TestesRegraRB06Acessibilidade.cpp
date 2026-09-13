#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb06_acessibilidade_solicitada_sala_sem_acessibilidade_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.acessibilidadeSolicitada = true;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sabado, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB06));
}

TESTE(rb06_acessibilidade_solicitada_sala_acessivel_nao_gera_violacao) {
    EstadoEnsalamento estado;
    Sala sala = criarSala("sala-1", "predio-1", "campus-1", 50);
    sala.acessivel = true;
    estado.salas.push_back(sala);

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.acessibilidadeSolicitada = true;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sabado, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB06));
}

TESTE(rb06_acessibilidade_nao_solicitada_sala_sem_acessibilidade_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sabado, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB06));
}