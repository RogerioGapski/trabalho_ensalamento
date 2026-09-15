#include "db/RepositorioAgendaIcs.hpp"

namespace ensalamento_api {

std::vector<EventoRecorrente> RepositorioAgendaIcs::montarEventosAPartirDeLinhas(const pqxx::result& linhas) {
    std::vector<EventoRecorrente> eventos;

    for (const auto& linha : linhas) {
        EventoRecorrente evento;

        evento.identificador = linha[0].as<std::string>();
        evento.diaSemanaTexto = linha[1].as<std::string>();
        evento.horaInicio = DataUtil::interpretarHoraIso(linha[2].as<std::string>());
        evento.horaFim = DataUtil::interpretarHoraIso(linha[3].as<std::string>());

        std::string disciplina = linha[4].as<std::string>();
        std::string codigoTurma = linha[5].as<std::string>();
        evento.resumo = codigoTurma + " - " + disciplina;

        std::string salaNome = linha[6].is_null() ? "" : linha[6].as<std::string>();
        std::string predioNome = linha[7].is_null() ? "" : linha[7].as<std::string>();
        std::string campusNome = linha[8].is_null() ? "" : linha[8].as<std::string>();

        evento.localizacao = salaNome.empty()
            ? "Sala a definir"
            : (campusNome + ", " + predioNome + ", Sala " + salaNome);

        evento.dataInicioPeriodo = DataUtil::interpretarDataIso(linha[9].as<std::string>());
        evento.dataFimPeriodo = DataUtil::interpretarDataIso(linha[10].as<std::string>());

        eventos.push_back(evento);
    }

    return eventos;
}

std::vector<EventoRecorrente> RepositorioAgendaIcs::gerarEventosParaTurma(pqxx::connection& conexao, const std::string& turmaId) {
    pqxx::work transacao(conexao);

    pqxx::result linhas = transacao.exec_params(
        "SELECT e.id::text, e.dia_semana::text, e.hora_inicio::text, e.hora_fim::text, "
        "t.disciplina, t.codigo, "
        "s.nome, p.nome, c.nome, "
        "pl.data_inicio::text, pl.data_fim::text "
        "FROM encontros e "
        "JOIN turmas t ON t.id = e.turma_id "
        "JOIN periodos_letivos pl ON pl.id = t.periodo_letivo_id "
        "LEFT JOIN alocacoes a ON a.encontro_id = e.id AND a.status = 'ativa' "
        "LEFT JOIN salas s ON s.id = a.sala_id "
        "LEFT JOIN predios p ON p.id = s.predio_id "
        "LEFT JOIN campi c ON c.id = p.campus_id "
        "WHERE t.id = $1",
        turmaId
    );

    transacao.commit();

    return montarEventosAPartirDeLinhas(linhas);
}

std::vector<EventoRecorrente> RepositorioAgendaIcs::gerarEventosParaProfessor(pqxx::connection& conexao, const std::string& professorId) {
    pqxx::work transacao(conexao);

    pqxx::result linhas = transacao.exec_params(
        "SELECT e.id::text, e.dia_semana::text, e.hora_inicio::text, e.hora_fim::text, "
        "t.disciplina, t.codigo, "
        "s.nome, p.nome, c.nome, "
        "pl.data_inicio::text, pl.data_fim::text "
        "FROM encontros e "
        "JOIN turmas t ON t.id = e.turma_id "
        "JOIN periodos_letivos pl ON pl.id = t.periodo_letivo_id AND pl.ativo = true "
        "LEFT JOIN alocacoes a ON a.encontro_id = e.id AND a.status = 'ativa' "
        "LEFT JOIN salas s ON s.id = a.sala_id "
        "LEFT JOIN predios p ON p.id = s.predio_id "
        "LEFT JOIN campi c ON c.id = p.campus_id "
        "WHERE t.professor_id = $1",
        professorId
    );

    transacao.commit();

    return montarEventosAPartirDeLinhas(linhas);
}

}