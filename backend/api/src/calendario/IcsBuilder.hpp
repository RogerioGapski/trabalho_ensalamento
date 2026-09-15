#ifndef ENSALAMENTO_API_ICS_BUILDER_HPP
#define ENSALAMENTO_API_ICS_BUILDER_HPP

#include "calendario/DataUtil.hpp"
#include <string>
#include <vector>

namespace ensalamento_api {

struct EventoRecorrente {
    std::string identificador;
    std::string resumo;
    std::string localizacao;
    std::string diaSemanaTexto;
    HoraSimples horaInicio;
    HoraSimples horaFim;
    DataSimples dataInicioPeriodo;
    DataSimples dataFimPeriodo;
};

class IcsBuilder {
public:
    static std::string construirCalendario(const std::string& nomeCalendario, const std::vector<EventoRecorrente>& eventos);

private:
    static std::string escaparTextoIcs(const std::string& texto);
    static std::string construirEvento(const EventoRecorrente& evento);
};

}

#endif