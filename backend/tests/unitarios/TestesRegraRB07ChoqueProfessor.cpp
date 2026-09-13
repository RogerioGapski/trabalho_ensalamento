#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb07_mesmo_professor_horarios_sobrepostos_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    Turma turmaA = criarTurma("turma-1", 20);
    turmaA.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaA);

    Turma turmaB = criarTurma("turma-2", 20);
    turmaB.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaB);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Domingo, HoraDia{9, 0}, HoraDia{11, 0}));

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB07));
}

TESTE(rb07_mesmo_professor_horarios_nao_sobrepostos_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    Turma turmaA = criarTurma("turma-1", 20);
    turmaA.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaA);

    Turma turmaB = criarTurma("turma-2", 20);
    turmaB.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaB);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Domingo, HoraDia{10, 0}, HoraDia{12, 0}));

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB07));
}

TESTE(rb07_turmas_sem_professor_nao_gera_falso_positivo) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.turmas.push_back(criarTurma("turma-2", 20));

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Domingo, HoraDia{9, 0}, HoraDia{11, 0}));

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB07));
}

TESTE(rb07_professor_em_comum_parcial_gera_violacao_apenas_para_professor_compartilhado) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    Turma turmaA = criarTurma("turma-1", 20);
    turmaA.professoresIds.insert("professor-1");
    turmaA.professoresIds.insert("professor-2");
    estado.turmas.push_back(turmaA);

    Turma turmaB = criarTurma("turma-2", 20);
    turmaB.professoresIds.insert("professor-2");
    turmaB.professoresIds.insert("professor-3");
    estado.turmas.push_back(turmaB);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Domingo, HoraDia{9, 0}, HoraDia{11, 0}));

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouViolacaoProfessor2 = false;
    bool encontrouViolacaoProfessor1Ou3 = false;

    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo != CodigoRegra::RB07) {
            continue;
        }
        if (violacao.mensagem.find("professor-2") != std::string::npos) {
            encontrouViolacaoProfessor2 = true;
        }
        if (violacao.mensagem.find("professor-1") != std::string::npos || violacao.mensagem.find("professor-3") != std::string::npos) {
            encontrouViolacaoProfessor1Ou3 = true;
        }
    }

    ASSERT_VERDADEIRO(encontrouViolacaoProfessor2);
    ASSERT_FALSO(encontrouViolacaoProfessor1Ou3);
}

TESTE(rb07_dois_encontros_da_mesma_turma_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    Turma turma = criarTurma("turma-1", 20);
    turma.professoresIds.insert("professor-1");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-1", DiaSemana::Domingo, HoraDia{9, 0}, HoraDia{11, 0}));

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB07));
}

TESTE(rb07_dias_diferentes_mesmo_professor_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));

    Turma turmaA = criarTurma("turma-1", 20);
    turmaA.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaA);

    Turma turmaB = criarTurma("turma-2", 20);
    turmaB.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaB);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB07));
}