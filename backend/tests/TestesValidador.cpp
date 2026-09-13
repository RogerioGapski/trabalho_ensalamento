#include "include/ensalamento/Validador.hpp"
#include "ensalamento/RastreadorAlteracoes.hpp"
#include <cassert>
#include <iostream>

using namespace ensalamento;

static Sala criarSalaPadrao(const std::string& id, int capacidade) {
    Sala sala;
    sala.id = id;
    sala.nome = "Sala " + id;
    sala.predioId = "predio-1";
    sala.campusId = "campus-1";
    sala.tipo = TipoSala::SalaComum;
    sala.capacidadeRegular = capacidade;
    sala.acessivel = false;
    sala.status = StatusSala::Ativa;
    return sala;
}

static Turma criarTurmaPadrao(const std::string& id, int quantidadePrevista) {
    Turma turma;
    turma.id = id;
    turma.disciplina = "Disciplina " + id;
    turma.coordenacaoId = "coordenacao-1";
    turma.quantidadePrevista = quantidadePrevista;
    turma.necessidades.acessibilidadeSolicitada = false;
    return turma;
}

static Encontro criarEncontroPadrao(const std::string& id, const std::string& turmaId, DiaSemana dia, HoraDia inicio, HoraDia fim) {
    Encontro encontro;
    encontro.id = id;
    encontro.turmaId = turmaId;
    encontro.diaSemana = dia;
    encontro.horaInicio = inicio;
    encontro.horaFim = fim;
    encontro.permiteDivisaoDeSala = false;
    return encontro;
}

static Alocacao criarAlocacaoPadrao(const std::string& id, const std::string& encontroId, const std::string& salaId) {
    Alocacao alocacao;
    alocacao.id = id;
    alocacao.encontroId = encontroId;
    alocacao.salaId = salaId;
    alocacao.status = StatusAlocacao::Ativa;
    return alocacao;
}

static void testeRB01ChoqueDeSala() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSalaPadrao("sala-1", 50));
    estado.turmas.push_back(criarTurmaPadrao("turma-1", 30));
    estado.turmas.push_back(criarTurmaPadrao("turma-2", 30));
    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontroPadrao("encontro-2", "turma-2", DiaSemana::Segunda, HoraDia{9, 0}, HoraDia{11, 0}));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-2", "encontro-2", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB01 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB01) {
            encontrouRB01 = true;
        }
    }

    assert(encontrouRB01);
    std::cout << "testeRB01ChoqueDeSala: OK" << std::endl;
}

static void testeRB02DivisaoNaoCadastrada() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSalaPadrao("sala-1", 50));
    estado.salas.push_back(criarSalaPadrao("sala-2", 50));
    estado.turmas.push_back(criarTurmaPadrao("turma-1", 30));
    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Terca, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-2", "encontro-1", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB02 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB02) {
            encontrouRB02 = true;
        }
    }

    assert(encontrouRB02);
    std::cout << "testeRB02DivisaoNaoCadastrada: OK" << std::endl;
}

static void testeRB03Capacidade() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSalaPadrao("sala-1", 20));
    estado.turmas.push_back(criarTurmaPadrao("turma-1", 30));
    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Quarta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB03 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB03) {
            encontrouRB03 = true;
        }
    }

    assert(encontrouRB03);
    std::cout << "testeRB03Capacidade: OK" << std::endl;
}

static void testeRB04SalaIndisponivel() {
    EstadoEnsalamento estado;
    Sala sala = criarSalaPadrao("sala-1", 50);
    sala.status = StatusSala::Desativada;
    estado.salas.push_back(sala);
    estado.turmas.push_back(criarTurmaPadrao("turma-1", 30));
    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Quinta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB04 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB04) {
            encontrouRB04 = true;
        }
    }

    assert(encontrouRB04);
    std::cout << "testeRB04SalaIndisponivel: OK" << std::endl;
}

static void testeRB05RecursosObrigatorios() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSalaPadrao("sala-1", 50));

    Turma turma = criarTurmaPadrao("turma-1", 30);
    turma.necessidades.recursosObrigatorios.insert("projetor");
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Sexta, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB05 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB05) {
            encontrouRB05 = true;
        }
    }

    assert(encontrouRB05);
    std::cout << "testeRB05RecursosObrigatorios: OK" << std::endl;
}

static void testeRB06AcessibilidadeObrigatoria() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSalaPadrao("sala-1", 50));

    Turma turma = criarTurmaPadrao("turma-1", 30);
    turma.necessidades.acessibilidadeSolicitada = true;
    estado.turmas.push_back(turma);

    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Sabado, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB06 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB06) {
            encontrouRB06 = true;
        }
    }

    assert(encontrouRB06);
    std::cout << "testeRB06AcessibilidadeObrigatoria: OK" << std::endl;
}

static void testeRB07ChoqueDeProfessor() {
    EstadoEnsalamento estado;
    estado.salas.push_back(criarSalaPadrao("sala-1", 50));
    estado.salas.push_back(criarSalaPadrao("sala-2", 50));

    Turma turmaA = criarTurmaPadrao("turma-1", 20);
    turmaA.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaA);

    Turma turmaB = criarTurmaPadrao("turma-2", 20);
    turmaB.professoresIds.insert("professor-1");
    estado.turmas.push_back(turmaB);

    estado.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Domingo, HoraDia{8, 0}, HoraDia{10, 0}));
    estado.encontros.push_back(criarEncontroPadrao("encontro-2", "turma-2", DiaSemana::Domingo, HoraDia{9, 0}, HoraDia{11, 0}));

    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));
    estado.alocacoes.push_back(criarAlocacaoPadrao("alocacao-2", "encontro-2", "sala-2"));

    ResultadoValidacao resultado = Validador::validar(estado);

    bool encontrouRB07 = false;
    for (const auto& violacao : resultado.violacoes) {
        if (violacao.codigo == CodigoRegra::RB07) {
            encontrouRB07 = true;
        }
    }

    assert(encontrouRB07);
    std::cout << "testeRB07ChoqueDeProfessor: OK" << std::endl;
}

static void testeRB08InvalidacaoPorMudancaDeSala() {
    EstadoEnsalamento estadoAnterior;
    estadoAnterior.salas.push_back(criarSalaPadrao("sala-1", 50));
    estadoAnterior.turmas.push_back(criarTurmaPadrao("turma-1", 30));
    estadoAnterior.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estadoAnterior.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));

    EstadoEnsalamento estadoAtual = estadoAnterior;
    estadoAtual.salas[0].capacidadeRegular = 10;

    std::vector<std::string> alocacoesParaRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    bool encontrouAlocacao1 = false;
    for (const auto& id : alocacoesParaRevalidar) {
        if (id == "alocacao-1") {
            encontrouAlocacao1 = true;
        }
    }

    assert(encontrouAlocacao1);
    std::cout << "testeRB08InvalidacaoPorMudancaDeSala: OK" << std::endl;
}

static void testeRB08SemMudancaNaoRevalida() {
    EstadoEnsalamento estadoAnterior;
    estadoAnterior.salas.push_back(criarSalaPadrao("sala-1", 50));
    estadoAnterior.turmas.push_back(criarTurmaPadrao("turma-1", 30));
    estadoAnterior.encontros.push_back(criarEncontroPadrao("encontro-1", "turma-1", DiaSemana::Segunda, HoraDia{8, 0}, HoraDia{10, 0}));
    estadoAnterior.alocacoes.push_back(criarAlocacaoPadrao("alocacao-1", "encontro-1", "sala-1"));

    EstadoEnsalamento estadoAtual = estadoAnterior;

    std::vector<std::string> alocacoesParaRevalidar = RastreadorAlteracoes::identificarAlocacoesParaRevalidar(estadoAnterior, estadoAtual);

    assert(alocacoesParaRevalidar.empty());
    std::cout << "testeRB08SemMudancaNaoRevalida: OK" << std::endl;
}

int main() {
    testeRB01ChoqueDeSala();
    testeRB02DivisaoNaoCadastrada();
    testeRB03Capacidade();
    testeRB04SalaIndisponivel();
    testeRB05RecursosObrigatorios();
    testeRB06AcessibilidadeObrigatoria();
    testeRB07ChoqueDeProfessor();
    testeRB08InvalidacaoPorMudancaDeSala();
    testeRB08SemMudancaNaoRevalida();

    std::cout << "Todos os testes passaram." << std::endl;
    return 0;
}