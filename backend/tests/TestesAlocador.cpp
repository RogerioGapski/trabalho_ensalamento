#include "ensalamento/Alocador.hpp"
#include <cassert>
#include <iostream>

using namespace ensalamento;

static Sala criarSala(const std::string& id, const std::string& predioId, const std::string& campusId, int capacidade) {
    Sala sala;
    sala.id = id;
    sala.nome = "Sala " + id;
    sala.predioId = predioId;
    sala.campusId = campusId;
    sala.tipo = TipoSala::SalaComum;
    sala.capacidadeRegular = capacidade;
    sala.acessivel = false;
    sala.status = StatusSala::Ativa;
    return sala;
}

static Turma criarTurma(const std::string& id, int quantidadePrevista) {
    Turma turma;
    turma.id = id;
    turma.disciplina = "Disciplina " + id;
    turma.coordenacaoId = "coordenacao-1";
    turma.quantidadePrevista = quantidadePrevista;
    turma.necessidades.acessibilidadeSolicitada = false;
    return turma;
}

static Encontro criarEncontro(const std::string& id, const std::string& turmaId, DiaSemana dia, HoraDia inicio, HoraDia fim) {
    Encontro encontro;
    encontro.id = id;
    encontro.turmaId = turmaId;
    encontro.diaSemana = dia;
    encontro.horaInicio = inicio;
    encontro.horaFim = fim;
    encontro.permiteDivisaoDeSala = false;
    return encontro;
}

static ConfiguracaoIndiceAdequacao configuracaoPadrao() {
    ConfiguracaoIndiceAdequacao configuracao;
    configuracao.pesoSobraDeEspaco = 10.0;
    configuracao.pesoPermanenciaMesmaSala = 5.0;
    configuracao.pesoPermanenciaMesmoPredio = 3.0;
    configuracao.pesoCampusPreferido = 4.0;
    configuracao.pesoPredioPreferido = 2.0;
    return configuracao;
}

static void testeAlocacaoBasicaSemConflito() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 40));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 40));

    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.turmas.push_back(criarTurma("turma-2", 30));

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, configuracaoPadrao());

    assert(resultado.alocacoesGeradas.size() == 2);
    assert(resultado.alocacoesGeradas[0].salaId != resultado.alocacoesGeradas[1].salaId);

    std::cout << "testeAlocacaoBasicaSemConflito: OK" << std::endl;
}

static void testePreferenciaPorMenorSobra() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-grande", "predio-1", "campus-1", 100));
    estado.salas.push_back(criarSala("sala-justa", "predio-1", "campus-1", 32));

    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, configuracaoPadrao());

    assert(resultado.alocacoesGeradas.size() == 1);
    assert(resultado.alocacoesGeradas[0].salaId == "sala-justa");

    std::cout << "testePreferenciaPorMenorSobra: OK" << std::endl;
}

static void testePermanenciaNaMesmaSala() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-1", "predio-1", "campus-1", 40));
    estado.salas.push_back(criarSala("sala-2", "predio-1", "campus-1", 40));

    estado.turmas.push_back(criarTurma("turma-1", 30));

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontro("encontro-2", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, configuracaoPadrao());

    assert(resultado.alocacoesGeradas.size() == 2);
    assert(resultado.alocacoesGeradas[0].salaId == resultado.alocacoesGeradas[1].salaId);

    std::cout << "testePermanenciaNaMesmaSala: OK" << std::endl;
}

static void testeCampusPreferidoInfluenciaEscolha() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-campus-a", "predio-1", "campus-a", 32));
    estado.salas.push_back(criarSala("sala-campus-b", "predio-2", "campus-b", 32));

    Turma turma = criarTurma("turma-1", 30);
    turma.necessidades.campusPreferido = "campus-b";
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Quinta, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, configuracaoPadrao());

    assert(resultado.alocacoesGeradas.size() == 1);
    assert(resultado.alocacoesGeradas[0].salaId == "sala-campus-b");

    std::cout << "testeCampusPreferidoInfluenciaEscolha: OK" << std::endl;
}

static void testeTurmaNaoAlocadaPorCapacidade() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSala("sala-pequena", "predio-1", "campus-1", 10));

    estado.turmas.push_back(criarTurma("turma-1", 30));
    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Sabado, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, configuracaoPadrao());

    assert(resultado.alocacoesGeradas.empty());
    assert(resultado.relatoriosTurmas.size() == 1);
    assert(resultado.relatoriosTurmas[0].totalmenteAlocada == false);
    assert(resultado.relatoriosTurmas[0].relatoriosEncontros[0].alocado == false);
    assert(!resultado.relatoriosTurmas[0].relatoriosEncontros[0].motivos.empty());

    std::cout << "testeTurmaNaoAlocadaPorCapacidade: OK" << std::endl;
}

static void testeAcessibilidadeObrigatoriaNoAlocador() {
    EstadoEnsalamento estado;
    Sala salaSemAcessibilidade = criarSala("sala-1", "predio-1", "campus-1", 40);
    estado.salas.push_back(salaSemAcessibilidade);

    Turma turma = criarTurma("turma-1", 30);
    turma.necessidades.acessibilidadeSolicitada = true;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontro("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));

    ResultadoAlocacao resultado = Alocador::alocar(estado, configuracaoPadrao());

    assert(resultado.alocacoesGeradas.empty());

    bool encontrouMotivoRB06 = false;
    for (const auto& motivo : resultado.relatoriosTurmas[0].relatoriosEncontros[0].motivos) {
        if (motivo.find("RB-06") != std::string::npos) {
            encontrouMotivoRB06 = true;
        }
    }
    assert(encontrouMotivoRB06);

    std::cout << "testeAcessibilidadeObrigatoriaNoAlocador: OK" << std::endl;
}

int main() {
    testeAlocacaoBasicaSemConflito();
    testePreferenciaPorMenorSobra();
    testePermanenciaNaMesmaSala();
    testeCampusPreferidoInfluenciaEscolha();
    testeTurmaNaoAlocadaPorCapacidade();
    testeAcessibilidadeObrigatoriaNoAlocador();

    std::cout << "Todos os testes do alocador passaram." << std::endl;
    return 0;
}