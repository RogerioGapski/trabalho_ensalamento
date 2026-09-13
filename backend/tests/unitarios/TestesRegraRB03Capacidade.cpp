#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb03_capacidade_exatamente_igual_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 30));
    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB03));
}

TESTE(rb03_capacidade_um_a_menos_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 29));
    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB03));
}

TESTE(rb03_quantidade_confirmada_maior_que_prevista_usa_confirmada_para_calculo) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 35));

    Turma turma = criarTurma("turma-1", 20);
    turma.quantidadeConfirmada = 40;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB03));
}

TESTE(rb03_turma_gigante_sem_sala_suficiente_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("auditorio-1", "predio-1", "campus-1", 300));
    estado.turmas.push_back(criarTurma("turma-1", 5000));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quinta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "auditorio-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB03));
}

TESTE(rb03_sala_com_capacidade_zero_e_turma_minima_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 0));
    estado.turmas.push_back(criarTurma("turma-1", 1));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB03));
}