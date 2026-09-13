#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Alocador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(alocador_turma_gigante_sem_sala_que_comporte_fica_pendente) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("auditorio-1", "predio-1", "campus-1", 400));
    estado.turmas.push_back(criarTurma("turma-1", 10000));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_VERDADEIRO(resultado.alocacoesGeradas.empty());
    ASSERT_FALSO(resultado.relatoriosTurmas[0].totalmenteAlocada);
}

TESTE(alocador_preferencia_por_menor_sobra_de_espaco) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-grande", "predio-1", "campus-1", 100));
    estado.salas.push_back(criarSala("sala-justa", "predio-1", "campus-1", 32));

    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_IGUAL(std::size_t(1), resultado.alocacoesGeradas.size());
    ASSERT_IGUAL(std::string("sala-justa"), resultado.alocacoesGeradas[0].salaId);
}

TESTE(alocador_mantem_turma_na_mesma_sala_ao_longo_da_semana) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 40));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 40));

    estado.turmas.push_back(criarTurma("turma-1", 30));

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_IGUAL(std::size_t(2), resultado.alocacoesGeradas.size());
    ASSERT_IGUAL(resultado.alocacoesGeradas[0].salaId, resultado.alocacoesGeradas[1].salaId);
}

TESTE(alocador_prioriza_campus_preferido_da_coordenacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-campus-a", "predio-1", "campus-a", 32));
    estado.salas.push_back(criarSala("sala-campus-b", "predio-2", "campus-b", 32));

    Turma turma = criarTurma("turma-1", 30);
    turma.necessidades.campusPreferido = "campus-b";
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quinta, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_IGUAL(std::size_t(1), resultado.alocacoesGeradas.size());
    ASSERT_IGUAL(std::string("sala-campus-b"), resultado.alocacoesGeradas[0].salaId);
}

TESTE(alocador_desempate_por_id_da_sala_quando_indices_iguais) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-b", "predio-1", "campus-1", 30));
    estado.salas.push_back(criarSala("sala-a", "predio-1", "campus-1", 30));

    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sabado, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_IGUAL(std::size_t(1), resultado.alocacoesGeradas.size());
    ASSERT_IGUAL(std::string("sala-a"), resultado.alocacoesGeradas[0].salaId);
}

TESTE(alocador_preserva_alocacao_existente_sem_gerar_nova) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 40));
    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-existente", "encontro-1", "sala-1"));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_VERDADEIRO(resultado.alocacoesGeradas.empty());
    ASSERT_VERDADEIRO(resultado.relatoriosTurmas[0].totalmenteAlocada);
    ASSERT_VERDADEIRO(resultado.relatoriosTurmas[0].relatoriosEncontros[0].alocado);
}

TESTE(alocador_turma_com_acessibilidade_obrigatoria_sem_sala_acessivel_fica_pendente) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 40));

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.acessibilidadeSolicitada = true;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_VERDADEIRO(resultado.alocacoesGeradas.empty());

    bool encontrouMotivoRB06 = false;
    for (const auto& motivo : resultado.relatoriosTurmas[0].relatoriosEncontros[0].motivos) {
        if (motivo.find("RB-06") != std::string::npos) {
            encontrouMotivoRB06 = true;
        }
    }
    ASSERT_VERDADEIRO(encontrouMotivoRB06);
}

TESTE(alocador_duas_turmas_mesmo_horario_ocupam_salas_diferentes) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 40));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 40));

    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.turmas.push_back(criarTurma("turma-2", 30));

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, criarConfiguracaoIndicePadrao());

    ASSERT_IGUAL(std::size_t(2), resultado.alocacoesGeradas.size());
    ASSERT_DIFERENTE(resultado.alocacoesGeradas[0].salaId, resultado.alocacoesGeradas[1].salaId);
}