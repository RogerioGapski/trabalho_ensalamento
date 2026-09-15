#include "calendario/DataUtil.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <array>

namespace ensalamento_api {

bool DataUtil::ehAnoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int DataUtil::diasNoMes(int ano, int mes) {
    static const std::array<int, 12> diasPorMes = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (mes == 2 && ehAnoBissexto(ano)) {
        return 29;
    }

    return diasPorMes[static_cast<std::size_t>(mes - 1)];
}

int DataUtil::diaDaSemana(const DataSimples& data) {
    int ano = data.ano;
    int mes = data.mes;

    if (mes < 3) {
        mes += 12;
        ano -= 1;
    }

    int k = ano % 100;
    int j = ano / 100;

    int resultado = (data.dia + 13 * (mes + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

    return (resultado + 6) % 7;
}

DataSimples DataUtil::adicionarDias(const DataSimples& data, int quantidadeDias) {
    DataSimples resultado = data;

    for (int i = 0; i < quantidadeDias; ++i) {
        resultado.dia += 1;
        if (resultado.dia > diasNoMes(resultado.ano, resultado.mes)) {
            resultado.dia = 1;
            resultado.mes += 1;
            if (resultado.mes > 12) {
                resultado.mes = 1;
                resultado.ano += 1;
            }
        }
    }

    return resultado;
}

DataSimples DataUtil::primeiraOcorrenciaDoDiaDaSemana(const DataSimples& dataInicio, int diaSemanaAlvo) {
    DataSimples dataAtual = dataInicio;

    for (int i = 0; i < 7; ++i) {
        if (diaDaSemana(dataAtual) == diaSemanaAlvo) {
            return dataAtual;
        }
        dataAtual = adicionarDias(dataAtual, 1);
    }

    return dataAtual;
}

DataSimples DataUtil::interpretarDataIso(const std::string& textoIso) {
    DataSimples resultado;
    std::istringstream fluxo(textoIso);
    std::string parteAno, parteMes, parteDia;

    std::getline(fluxo, parteAno, '-');
    std::getline(fluxo, parteMes, '-');
    std::getline(fluxo, parteDia, '-');

    resultado.ano = std::stoi(parteAno);
    resultado.mes = std::stoi(parteMes);
    resultado.dia = std::stoi(parteDia.substr(0, 2));

    return resultado;
}

HoraSimples DataUtil::interpretarHoraIso(const std::string& textoIso) {
    HoraSimples resultado;
    std::istringstream fluxo(textoIso);
    std::string parteHoras, parteMinutos, parteSegundos;

    std::getline(fluxo, parteHoras, ':');
    std::getline(fluxo, parteMinutos, ':');
    std::getline(fluxo, parteSegundos, ':');

    resultado.horas = std::stoi(parteHoras);
    resultado.minutos = std::stoi(parteMinutos);
    resultado.segundos = parteSegundos.empty() ? 0 : std::stoi(parteSegundos.substr(0, 2));

    return resultado;
}

int DataUtil::indiceDiaSemanaPorTexto(const std::string& diaSemanaTexto) {
    if (diaSemanaTexto == "domingo") return 0;
    if (diaSemanaTexto == "segunda") return 1;
    if (diaSemanaTexto == "terca") return 2;
    if (diaSemanaTexto == "quarta") return 3;
    if (diaSemanaTexto == "quinta") return 4;
    if (diaSemanaTexto == "sexta") return 5;
    return 6;
}

std::string DataUtil::codigoBydayPorIndice(int indiceDiaSemana) {
    static const std::array<std::string, 7> codigos = { "SU", "MO", "TU", "WE", "TH", "FR", "SA" };
    return codigos[static_cast<std::size_t>(indiceDiaSemana)];
}

std::string DataUtil::formatarDataHoraLocalIcs(const DataSimples& data, const HoraSimples& hora) {
    std::ostringstream fluxo;
    fluxo << std::setfill('0')
          << std::setw(4) << data.ano << std::setw(2) << data.mes << std::setw(2) << data.dia
          << "T"
          << std::setw(2) << hora.horas << std::setw(2) << hora.minutos << std::setw(2) << hora.segundos;
    return fluxo.str();
}

std::string DataUtil::formatarDataHoraUtcIcs(const DataSimples& data, const HoraSimples& hora) {
    return formatarDataHoraLocalIcs(data, hora) + "Z";
}

std::string DataUtil::formatarMomentoAtualUtcIcs() {
    std::time_t agora = std::time(nullptr);
    std::tm tempoUtc{};

#if defined(_WIN32)
    gmtime_s(&tempoUtc, &agora);
#else
    gmtime_r(&agora, &tempoUtc);
#endif

    std::ostringstream fluxo;
    fluxo << std::setfill('0')
          << std::setw(4) << (tempoUtc.tm_year + 1900)
          << std::setw(2) << (tempoUtc.tm_mon + 1)
          << std::setw(2) << tempoUtc.tm_mday
          << "T"
          << std::setw(2) << tempoUtc.tm_hour
          << std::setw(2) << tempoUtc.tm_min
          << std::setw(2) << tempoUtc.tm_sec
          << "Z";
    return fluxo.str();
}

}