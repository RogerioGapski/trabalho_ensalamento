#ifndef ENSALAMENTO_API_DATA_UTIL_HPP
#define ENSALAMENTO_API_DATA_UTIL_HPP

#include <string>

namespace ensalamento_api {

struct DataSimples {
    int ano;
    int mes;
    int dia;
};

struct HoraSimples {
    int horas;
    int minutos;
    int segundos;
};

class DataUtil {
public:
    static bool ehAnoBissexto(int ano);
    static int diasNoMes(int ano, int mes);
    static int diaDaSemana(const DataSimples& data);
    static DataSimples adicionarDias(const DataSimples& data, int quantidadeDias);
    static DataSimples primeiraOcorrenciaDoDiaDaSemana(const DataSimples& dataInicio, int diaSemanaAlvo);

    static DataSimples interpretarDataIso(const std::string& textoIso);
    static HoraSimples interpretarHoraIso(const std::string& textoIso);

    static int indiceDiaSemanaPorTexto(const std::string& diaSemanaTexto);
    static std::string codigoBydayPorIndice(int indiceDiaSemana);

    static std::string formatarDataHoraLocalIcs(const DataSimples& data, const HoraSimples& hora);
    static std::string formatarDataHoraUtcIcs(const DataSimples& data, const HoraSimples& hora);
    static std::string formatarMomentoAtualUtcIcs();
};

}

#endif