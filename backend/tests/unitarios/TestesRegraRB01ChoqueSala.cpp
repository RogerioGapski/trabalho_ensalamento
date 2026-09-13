#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb01_horarios_exatamente_iguais_mesma_sala_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.turmas.push_back(criarTurma("turma-2", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB01));
}

TESTE(rb01_horarios_parcialmente_sobrepostos_mesma_sala_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.turmas.push_back(criarTurma("turma-2", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{9, 30}, HoraDia{11, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB01));
}

TESTE(rb01_horarios_encostando_nas_bordas_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.turmas.push_back(criarTurma("turma-2", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{10, 0}, HoraDia{12, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB01));
}

TESTE(rb01_dias_diferentes_mesma_sala_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.turmas.push_back(criarTurma("turma-2", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB01));
}

TESTE(rb01_salas_diferentes_mesmo_horario_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.turmas.push_back(criarTurma("turma-2", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB01));
}

TESTE(rb01_tres_encontros_mesma_sala_mesmo_horario_gera_multiplas_violacoes) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 10));
    estado.turmas.push_back(criarTurma("turma-2", 10));
    estado.turmas.push_back(criarTurma("turma-3", 10));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-3", "turma-3", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-3", "encontro-3", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_IGUAL(3, contarViolacoesComCodigo(resultado.violacoes, CodigoRegra::RB01));
}

TESTE(rb01_alocacao_cancelada_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 10));
    estado.turmas.push_back(criarTurma("turma-2", 10));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1", StatusAlocacao::Cancelada));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB01));
}