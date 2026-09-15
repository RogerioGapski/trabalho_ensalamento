#ifndef ENSALAMENTO_API_REPOSITORIO_AGENDA_ICS_HPP
#define ENSALAMENTO_API_REPOSITORIO_AGENDA_ICS_HPP

#include "calendario/IcsBuilder.hpp"
#include <pqxx/pqxx>
#include <string>
#include <vector>

namespace ensalamento_api {

class RepositorioAgendaIcs {
public:
    static std::vector<EventoRecorrente> gerarEventosParaTurma(pqxx::connection& conexao, const std::string& turmaId);
    static std::vector<EventoRecorrente> gerarEventosParaProfessor(pqxx::connection& conexao, const std::string& professorId);

private:
    static std::vector<EventoRecorrente> montarEventosAPartirDeLinhas(const pqxx::result& linhas);
};

}

#endif