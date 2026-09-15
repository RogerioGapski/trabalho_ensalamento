#include "db/RepositorioMetricas.hpp"
#include <sstream>
#include <map>
#include <cmath>

namespace ensalamento_api {

double RepositorioMetricas::horasEntre(const std::string& horaInicio, const std::string& horaFim) {
    auto paraMinutos = [](const std::string& texto) {
        std::istringstream fluxo(texto);
        std::string parteHoras;
        std::string parteMinutos;
        std::getline(fluxo, parteHoras, ':');
        std::getline(fluxo, parteMinutos, ':');
        return std::stoi(parteHoras) * 60 + std::stoi(parteMinutos);
    };

    int minutosInicio = paraMinutos(horaInicio);
    int minutosFim = paraMinutos(horaFim);

    return static_cast<double>(minutosFim - minutosInicio) / 60.0;
}

double RepositorioMetricas::horasDisponiveisPorSemana() {
    const double horasPorDiaUtil = 14.0;
    const int diasUteisPorSemana = 6;
    return horasPorDiaUtil * diasUteisPorSemana;
}

MetricasOcupacao RepositorioMetricas::calcularMetricas(pqxx::connection& conexao, const std::string& periodoLetivoId) {
    pqxx::work transacao(conexao);

    pqxx::result linhasSalas = transacao.exec(
        "SELECT id::text, nome, capacidade FROM salas WHERE ativo = true"
    );

    std::map<std::string, OcupacaoPorSala> ocupacaoPorSalaId;
    for (const auto& linha : linhasSalas) {
        OcupacaoPorSala ocupacao;
        ocupacao.salaId = linha[0].as<std::string>();
        ocupacao.salaNome = linha[1].as<std::string>();
        ocupacao.capacidade = linha[2].as<int>();
        ocupacao.horasOcupadas = 0.0;
        ocupacao.horasDisponiveis = horasDisponiveisPorSemana();
        ocupacao.taxaOcupacao = 0.0;
        ocupacaoPorSalaId[ocupacao.salaId] = ocupacao;
    }

    pqxx::result linhasEncontrosAlocados = transacao.exec_params(
        "SELECT s.id::text, s.nome, s.capacidade, e.dia_semana::text, e.hora_inicio::text, e.hora_fim::text, "
        "t.codigo, t.quantidade_alunos "
        "FROM alocacoes a "
        "JOIN encontros e ON e.id = a.encontro_id "
        "JOIN turmas t ON t.id = e.turma_id "
        "JOIN salas s ON s.id = a.sala_id "
        "WHERE t.periodo_letivo_id = $1 AND a.status = 'ativa'",
        periodoLetivoId
    );

    std::map<std::string, int> contagemPorDiaHora;
    std::vector<DesperdicioAssento> desperdicios;

    double totalHorasOcupadas = 0.0;

    for (const auto& linha : linhasEncontrosAlocados) {
        std::string salaId = linha[0].as<std::string>();
        std::string salaNome = linha[1].as<std::string>();
        int capacidadeSala = linha[2].as<int>();
        std::string diaSemana = linha[3].as<std::string>();
        std::string horaInicio = linha[4].as<std::string>().substr(0, 5);
        std::string horaFim = linha[5].as<std::string>().substr(0, 5);
        std::string turmaCodigo = linha[6].as<std::string>();
        int quantidadeAlunos = linha[7].as<int>();

        double duracaoHoras = horasEntre(horaInicio, horaFim);
        totalHorasOcupadas += duracaoHoras;

        auto iteradorOcupacao = ocupacaoPorSalaId.find(salaId);
        if (iteradorOcupacao != ocupacaoPorSalaId.end()) {
            iteradorOcupacao->second.horasOcupadas += duracaoHoras;
        }

        std::string chaveDiaHora = diaSemana + "|" + horaInicio;
        contagemPorDiaHora[chaveDiaHora] += 1;

        int assentosOciosos = capacidadeSala - quantidadeAlunos;
        if (assentosOciosos > 0) {
            DesperdicioAssento desperdicio;
            desperdicio.salaId = salaId;
            desperdicio.salaNome = salaNome;
            desperdicio.turmaCodigo = turmaCodigo;
            desperdicio.capacidade = capacidadeSala;
            desperdicio.tamanhoTurma = quantidadeAlunos;
            desperdicio.assentosOciosos = assentosOciosos;
            desperdicios.push_back(desperdicio);
        }
    }

    transacao.commit();

    MetricasOcupacao metricas;
    metricas.totalHorasOcupadas = totalHorasOcupadas;

    double totalHorasDisponiveis = 0.0;
    int totalAssentosOciosos = 0;

    for (auto& parOcupacao : ocupacaoPorSalaId) {
        OcupacaoPorSala& ocupacao = parOcupacao.second;
        ocupacao.taxaOcupacao = ocupacao.horasDisponiveis > 0.0
            ? ocupacao.horasOcupadas / ocupacao.horasDisponiveis
            : 0.0;

        totalHorasDisponiveis += ocupacao.horasDisponiveis;
        metricas.ocupacaoPorSala.push_back(ocupacao);
    }

    metricas.totalHorasDisponiveis = totalHorasDisponiveis;
    metricas.taxaOcupacaoGeral = totalHorasDisponiveis > 0.0
        ? totalHorasOcupadas / totalHorasDisponiveis
        : 0.0;

    for (const auto& desperdicio : desperdicios) {
        totalAssentosOciosos += desperdicio.assentosOciosos;
    }
    metricas.totalAssentosOciosos = totalAssentosOciosos;

    for (const auto& parContagem : contagemPorDiaHora) {
        std::string chave = parContagem.first;
        std::size_t posicaoSeparador = chave.find('|');

        HorarioPico horarioPico;
        horarioPico.diaSemana = chave.substr(0, posicaoSeparador);
        horarioPico.horaInicio = chave.substr(posicaoSeparador + 1);
        horarioPico.quantidadeEncontros = parContagem.second;

        metricas.horariosPico.push_back(horarioPico);
    }

    metricas.desperdicioAssentos = desperdicios;

    return metricas;
}

}