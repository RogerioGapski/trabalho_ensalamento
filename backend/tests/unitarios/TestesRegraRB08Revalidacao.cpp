#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/RastreadorAlteracoes.hpp"

using namespace ensalamento;
using namespace testes;

static EstadoEnsalamento criarEstadoBaseParaRb08() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));

    Turma turma = criarTurma("turma-1", 20);
    turma.professoresIds.insert("professor-1");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    return estado;
}

TESTE(rb08_nenhuma_alteracao_nao_marca_nada_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_VERDADEIRO(paraRevalidar.empty());
}

TESTE(rb08_alteracao_capacidade_sala_marca_alocacao_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.salas[0].capacidadeRegular = 10;

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_IGUAL(std::size_t(1), paraRevalidar.size());
    ASSERT_IGUAL(std::string("alocacao-1"), paraRevalidar[0]);
}

TESTE(rb08_alteracao_horario_encontro_marca_alocacao_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.encontros[0].horaInicio = HoraDia{9, 0};

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_IGUAL(std::size_t(1), paraRevalidar.size());
}

TESTE(rb08_alteracao_professor_da_turma_marca_alocacao_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.turmas[0].professoresIds.insert("professor-2");

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_IGUAL(std::size_t(1), paraRevalidar.size());
}

TESTE(rb08_troca_de_sala_na_alocacao_marca_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    estadoAnterior.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.alocacoes[0].salaId = "sala-2";

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_IGUAL(std::size_t(1), paraRevalidar.size());
}

TESTE(rb08_remocao_da_sala_marca_alocacao_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.salas.clear();

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_IGUAL(std::size_t(1), paraRevalidar.size());
}

TESTE(rb08_nova_alocacao_sem_correspondente_anterior_marca_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.encontros.push_back(criarEncontro("encontro-2", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));
    estadoAtual.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-1"));

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_IGUAL(std::size_t(1), paraRevalidar.size());
    ASSERT_IGUAL(std::string("alocacao-2"), paraRevalidar[0]);
}

TESTE(rb08_alocacao_cancelada_nao_e_marcada_para_revalidar) {
    EstadoEnsalamento estadoAnterior = criarEstadoBaseParaRb08();
    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.salas[0].capacidadeRegular = 5;
    estadoAtual.alocacoes[0].status = StatusAlocacao::Cancelada;

    std::vector<std::string> paraRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    ASSERT_VERDADEIRO(paraRevalidar.empty());
}