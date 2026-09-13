#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"
#include "ensalamento/Alocador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(cenario_sem_nenhuma_sala_cadastrada_turma_fica_pendente) {
    EstadoEnsalamento estado;
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_VERDADEIRO(resultado.alocacoesGeradas.empty());
    ASSERT_IGUAL(std::size_t(1), resultado.relatoriosTurmas.size());
    ASSERT_FALSO(resultado.relatoriosTurmas[0].totalmenteAlocada);
    ASSERT_FALSO(resultado.relatoriosTurmas[0].relatoriosEncontros[0].motivos.empty());
}

TESTE(cenario_todas_salas_violam_capacidade_turma_fica_pendente) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 10));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 15));
    estado.turmas.push_back(criarTurma("turma-1", 40));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_VERDADEIRO(resultado.alocacoesGeradas.empty());
    ASSERT_FALSO(resultado.relatoriosTurmas[0].totalmenteAlocada);

    for (const auto& avaliacao : resultado.relatoriosTurmas[0].relatoriosEncontros[0].salasAvaliadas) {
        ASSERT_FALSO(avaliacao.elegivel);
    }
}

TESTE(cenario_todas_salas_em_manutencao_turma_fica_pendente) {
    EstadoEnsalamento estado;
    Sala salaUm = criarSala("sala-1", "predio-1", "campus-1", 50);
    salaUm.status = StatusSala::EmManutencao;
    Sala salaDois = criarSala("sala-2", "predio-1", "campus-1", 50);
    salaDois.status = StatusSala::Desativada;
    estado.salas.push_back(salaUm);
    estado.salas.push_back(salaDois);

    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_VERDADEIRO(resultado.alocacoesGeradas.empty());
}

TESTE(cenario_sala_unica_viola_multiplas_regras_simultaneamente) {
    EstadoEnsalamento estado;
    Sala sala = criarSala("sala-1", "predio-1", "campus-1", 10);
    estado.salas.push_back(sala);

    Turma turma = criarTurma("turma-1", 100);
    turma.necessidades.acessibilidadeSolicitada = true;
    turma.necessidades.recursosObrigatorios.insert("projetor");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quinta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB03));
    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB05));
    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB06));
}