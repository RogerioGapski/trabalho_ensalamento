#include "framework/FrameworkTestes.hpp"
#include "framework/FixturesTestes.hpp"
#include "ensalamento/Validador.hpp"

using namespace ensalamento;
using namespace testes;

TESTE(rb05_recurso_obrigatorio_ausente_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.recursosObrigatorios.insert("projetor");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB05));
}

TESTE(rb05_todos_recursos_presentes_nao_gera_violacao) {
    EstadoEnsalamento estado;
    Sala sala = criarSala("sala-1", "predio-1", "campus-1", 50);
    sala.recursos.insert("projetor");
    sala.recursos.insert("ar_condicionado");
    estado.salas.push_back(sala);

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.recursosObrigatorios.insert("projetor");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB05));
}

TESTE(rb05_multiplos_recursos_ausentes_gera_violacao_unica_com_todos_listados) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.recursosObrigatorios.insert("projetor");
    turma.necessidades.recursosObrigatorios.insert("microfone");
    turma.necessidades.recursosObrigatorios.insert("computadores");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouViolacaoComTresRecursos = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB05 && violacao.mensagem.find("projetor") != std::string::npos
            && violacao.mensagem.find("microfone") != std::string::npos
            && violacao.mensagem.find("computadores") != std::string::npos) {
            encontrouViolacaoComTresRecursos = true;
        }
    }

    ASSERT_VERDADEIRO(encontrouViolacaoComTresRecursos);
}

TESTE(rb05_tipo_de_sala_obrigatorio_incorreto_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));

    Turma turma = criarTurma("turma-1", 20);
    turma.necessidades.tipoSalaObrigatorio = TipoSala::Laboratorio;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_VERDADEIRO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB05));
}

TESTE(rb05_recursos_obrigatorios_vazios_nao_gera_violacao) {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 50));
    estado.turmas.push_back(criarTurma("turma-1", 20));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    ASSERT_FALSO(possuiViolacaoComCodigo(resultado.violacoes, CodigoRegra::RB05));
}