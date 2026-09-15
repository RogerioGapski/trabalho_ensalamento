#ifndef ENSALAMENTO_API_REPOSITORIO_METRICAS_HPP
#define ENSALAMENTO_API_REPOSITORIO_METRICAS_HPP

#include <pqxx/pqxx>
#include <string>
#include <vector>

namespace ensalamento_api {

struct OcupacaoPorSala {
    std::string salaId;
    std::string salaNome;
    int capacidade;
    double horasOcupadas;
    double horasDisponiveis;
    double taxaOcupacao;
};

struct HorarioPico {
    std::string diaSemana;
    std::string horaInicio;
    int quantidadeEncontros;
};

struct DesperdicioAssento {
    std::string salaId;
    std::string salaNome;
    std::string turmaCodigo;
    int capacidade;
    int tamanhoTurma;
    int assentosOciosos;
};

struct MetricasOcupacao {
    double taxaOcupacaoGeral;
    double totalHorasOcupadas;
    double totalHorasDisponiveis;
    int totalAssentosOciosos;
    std::vector<OcupacaoPorSala> ocupacaoPorSala;
    std::vector<HorarioPico> horariosPico;
    std::vector<DesperdicioAssento> desperdicioAssentos;
};

class RepositorioMetricas {
public:
    static MetricasOcupacao calcularMetricas(pqxx::connection& conexao, const std::string& periodoLetivoId);

private:
    static double horasEntre(const std::string& horaInicio, const std::string& horaFim);
    static double horasDisponiveisPorSemana();
};

}

#endif