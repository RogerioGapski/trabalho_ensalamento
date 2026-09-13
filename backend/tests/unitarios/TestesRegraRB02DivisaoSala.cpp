#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb02_mesmo_encontro_duas_salas_sem_divisao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-1", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB02));
}

TESTE(rb02_mesmo_encontro_duas_salas_com_divisao_permitida_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));

    Encontro encontro = criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0});
    encontro.permiteDivisaoDeSala = true;
    estado.encontros.push_back(encontro);

    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-1", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB02));
}

TESTE(rb02_mesmo_encontro_uma_unica_sala_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB02));
}

TESTE(rb02_mesmo_encontro_tres_salas_sem_divisao_gera_violacao_agregada) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 50));
    estado.salas.push_back(criarSala("sala-3", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-2", "encontro-1", "sala-2"));
    estado.alocacoes.push_back(criarAlocacao("alocacao-3", "encontro-1", "sala-3"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_IGUAL(1, contarViolacoesComCodigo(resultado.violacoes, CodigoRegra::RB02));
}