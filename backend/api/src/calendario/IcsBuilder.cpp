#include "calendario/IcsBuilder.hpp"
#include <sstream>

namespace ensalamento_api {

const std::string TIMEZONE_INSTITUICAO = "America/Sao_Paulo";

std::string IcsBuilder::escaparTextoIcs(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.size());

    for (char caractere : texto) {
        if (caractere == ',' || caractere == ';') {
            resultado.push_back('\\');
        }
        resultado.push_back(caractere);
    }

    return resultado;
}

std::string IcsBuilder::construirEvento(const EventoRecorrente& evento) {
    int indiceDiaSemana = DataUtil::indiceDiaSemanaPorTexto(evento.diaSemanaTexto);
    std::string codigoByday = DataUtil::codigoBydayPorIndice(indiceDiaSemana);

    DataSimples primeiraData = DataUtil::primeiraOcorrenciaDoDiaDaSemana(evento.dataInicioPeriodo, indiceDiaSemana);

    HoraSimples horaFimUntil{ 23, 59, 59 };

    std::ostringstream fluxo;
    fluxo << "BEGIN:VEVENT\r\n";
    fluxo << "UID:" << evento.identificador << "@ensalamento\r\n";
    fluxo << "DTSTAMP:" << DataUtil::formatarMomentoAtualUtcIcs() << "\r\n";
    fluxo << "DTSTART;TZID=" << TIMEZONE_INSTITUICAO << ":" << DataUtil::formatarDataHoraLocalIcs(primeiraData, evento.horaInicio) << "\r\n";
    fluxo << "DTEND;TZID=" << TIMEZONE_INSTITUICAO << ":" << DataUtil::formatarDataHoraLocalIcs(primeiraData, evento.horaFim) << "\r\n";
    fluxo << "RRULE:FREQ=WEEKLY;BYDAY=" << codigoByday << ";UNTIL="
          << DataUtil::formatarDataHoraUtcIcs(evento.dataFimPeriodo, horaFimUntil) << "\r\n";
    fluxo << "SUMMARY:" << escaparTextoIcs(evento.resumo) << "\r\n";
    fluxo << "LOCATION:" << escaparTextoIcs(evento.localizacao) << "\r\n";
    fluxo << "END:VEVENT\r\n";

    return fluxo.str();
}

std::string IcsBuilder::construirCalendario(const std::string& nomeCalendario, const std::vector<EventoRecorrente>& eventos) {
    std::ostringstream fluxo;

    fluxo << "BEGIN:VCALENDAR\r\n";
    fluxo << "VERSION:2.0\r\n";
    fluxo << "PRODID:-//Ensalamento//Agenda Academica//PT-BR\r\n";
    fluxo << "CALSCALE:GREGORIAN\r\n";
    fluxo << "METHOD:PUBLISH\r\n";
    fluxo << "X-WR-CALNAME:" << escaparTextoIcs(nomeCalendario) << "\r\n";
    fluxo << "X-WR-TIMEZONE:" << TIMEZONE_INSTITUICAO << "\r\n";

    for (const auto& evento : eventos) {
        fluxo << construirEvento(evento);
    }

    fluxo << "END:VCALENDAR\r\n";

    return fluxo.str();
}

}