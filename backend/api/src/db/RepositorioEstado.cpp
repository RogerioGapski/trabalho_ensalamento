#include "db/RepositorioEstado.hpp"
#include <crow.h>
#include <sstream>
#include <stdexcept>

namespace ensalamento_api {

ensalamento::TipoSala RepositorioEstado::textoParaTipoSala(const std::string& texto) {
    if (texto == "laboratorio") {
        return ensalamento::TipoSala::Laboratorio;
    }
    if (texto == "auditorio") {
        return ensalamento::TipoSala::Auditorio;
    }
    if (texto == "sala_aula") {
        return ensalamento::TipoSala::SalaComum;
    }
    return ensalamento::TipoSala::Outro;
}

ensalamento::DiaSemana RepositorioEstado::textoParaDiaSemana(const std::string& texto) {
    if (texto == "segunda") return ensalamento::DiaSemana::Segunda;
    if (texto == "terca") return ensalamento::DiaSemana::Terca;
    if (texto == "quarta") return ensalamento::DiaSemana::Quarta;
    if (texto == "quinta") return ensalamento::DiaSemana::Quinta;
    if (texto == "sexta") return ensalamento::DiaSemana::Sexta;
    if (texto == "sabado") return ensalamento::DiaSemana::Sabado;
    return ensalamento::DiaSemana::Domingo;
}

ensalamento::StatusAlocacao RepositorioEstado::textoParaStatusAlocacao(const std::string& texto) {
    if (texto == "ativa") return ensalamento::StatusAlocacao::Ativa;
    if (texto == "cancelada") return ensalamento::StatusAlocacao::Cancelada;
    if (texto == "conflito") return ensalamento::StatusAlocacao::Conflito;
    return ensalamento::StatusAlocacao::Pendente;
}

ensalamento::HoraDia RepositorioEstado::textoParaHoraDia(const std::string& texto) {
    ensalamento::HoraDia horaDia;
    std::istringstream fluxo(texto);
    std::string parteHoras;
    std::string parteMinutos;

    std::getline(fluxo, parteHoras, ':');
    std::getline(fluxo, parteMinutos, ':');

    horaDia.horas = std::stoi(parteHoras);
    horaDia.minutos = std::stoi(parteMinutos);

    return horaDia;
}

void RepositorioEstado::aplicarNecessidadesObrigatorias(const std::string& jsonTexto, ensalamento::NecessidadeTurma& necessidades) {
    crow::json::rvalue json = crow::json::load(jsonTexto);
    if (!json) {
        return;
    }

    for (const auto& item : json) {
        if (!item.has("tipo")) {
            continue;
        }

        std::string tipo = item["tipo"].s();

        if (tipo == "recurso" && item.has("valor")) {
            necessidades.recursosObrigatorios.insert(item["valor"].s());
        } else if (tipo == "tipo_sala" && item.has("valor")) {
            necessidades.tipoSalaObrigatorio = textoParaTipoSala(item["valor"].s());
        } else if (tipo == "acessibilidade") {
            necessidades.acessibilidadeSolicitada = true;
        } else if (tipo == "campus" && item.has("valor")) {
            necessidades.campusPreferido = std::string(item["valor"].s());
        } else if (tipo == "predio" && item.has("valor")) {
            necessidades.predioPreferido = std::string(item["valor"].s());
        }
    }
}

void RepositorioEstado::aplicarPreferencias(const std::string& jsonTexto, ensalamento::NecessidadeTurma& necessidades) {
    crow::json::rvalue json = crow::json::load(jsonTexto);
    if (!json) {
        return;
    }

    for (const auto& item : json) {
        if (!item.has("tipo")) {
            continue;
        }

        std::string tipo = item["tipo"].s();

        if (tipo == "acessibilidade") {
            necessidades.acessibilidadeSolicitada = true;
        } else if (tipo == "campus" && item.has("valor")) {
            necessidades.campusPreferido = std::string(item["valor"].s());
        } else if (tipo == "predio" && item.has("valor")) {
            necessidades.predioPreferido = std::string(item["valor"].s());
        }
    }
}

std::vector<ensalamento::Sala> RepositorioEstado::carregarSalas(pqxx::work& transacao) {
    std::vector<ensalamento::Sala> salas;

    pqxx::result linhas = transacao.exec(
        "SELECT s.id::text, s.nome, s.predio_id::text, p.campus_id::text, s.tipo::text, "
        "s.capacidade, s.acessivel, s.recursos::text, s.ativo "
        "FROM salas s JOIN predios p ON p.id = s.predio_id"
    );

    for (const auto& linha : linhas) {
        ensalamento::Sala sala;
        sala.id = linha[0].as<std::string>();
        sala.nome = linha[1].as<std::string>();
        sala.predioId = linha[2].as<std::string>();
        sala.campusId = linha[3].as<std::string>();
        sala.tipo = textoParaTipoSala(linha[4].as<std::string>());
        sala.capacidadeRegular = linha[5].as<int>();
        sala.acessivel = linha[6].as<bool>();
        sala.status = linha[8].as<bool>() ? ensalamento::StatusSala::Ativa : ensalamento::StatusSala::Desativada;

        crow::json::rvalue jsonRecursos = crow::json::load(linha[7].as<std::string>());
        if (jsonRecursos) {
            for (const auto& recurso : jsonRecursos) {
                sala.recursos.insert(recurso.s());
            }
        }

        salas.push_back(sala);
    }

    return salas;
}

std::vector<ensalamento::Turma> RepositorioEstado::carregarTurmas(pqxx::work& transacao, const std::string& periodoLetivoId) {
    std::vector<ensalamento::Turma> turmas;

    pqxx::result linhas = transacao.exec_params(
        "SELECT id::text, disciplina, professor_id::text, quantidade_alunos, "
        "necessidades_obrigatorias::text, preferencias::text "
        "FROM turmas WHERE periodo_letivo_id = $1 AND ativo = true",
        periodoLetivoId
    );

    for (const auto& linha : linhas) {
        ensalamento::Turma turma;
        turma.id = linha[0].as<std::string>();
        turma.disciplina = linha[1].as<std::string>();

        if (!linha[2].is_null()) {
            turma.professoresIds.insert(linha[2].as<std::string>());
        }

        turma.quantidadePrevista = linha[3].as<int>();

        aplicarNecessidadesObrigatorias(linha[4].as<std::string>(), turma.necessidades);
        aplicarPreferencias(linha[5].as<std::string>(), turma.necessidades);

        turmas.push_back(turma);
    }

    return turmas;
}

std::vector<ensalamento::Encontro> RepositorioEstado::carregarEncontros(pqxx::work& transacao, const std::string& periodoLetivoId) {
    std::vector<ensalamento::Encontro> encontros;

    pqxx::result linhas = transacao.exec_params(
        "SELECT e.id::text, e.turma_id::text, e.dia_semana::text, "
        "e.hora_inicio::text, e.hora_fim::text "
        "FROM encontros e JOIN turmas t ON t.id = e.turma_id "
        "WHERE t.periodo_letivo_id = $1",
        periodoLetivoId
    );

    for (const auto& linha : linhas) {
        ensalamento::Encontro encontro;
        encontro.id = linha[0].as<std::string>();
        encontro.turmaId = linha[1].as<std::string>();
        encontro.diaSemana = textoParaDiaSemana(linha[2].as<std::string>());
        encontro.horaInicio = textoParaHoraDia(linha[3].as<std::string>());
        encontro.horaFim = textoParaHoraDia(linha[4].as<std::string>());
        encontro.permiteDivisaoDeSala = false;

        encontros.push_back(encontro);
    }

    return encontros;
}

std::vector<ensalamento::Alocacao> RepositorioEstado::carregarAlocacoes(pqxx::work& transacao, const std::string& periodoLetivoId) {
    std::vector<ensalamento::Alocacao> alocacoes;

    pqxx::result linhas = transacao.exec_params(
        "SELECT a.id::text, a.encontro_id::text, a.sala_id::text, a.status::text "
        "FROM alocacoes a "
        "JOIN encontros e ON e.id = a.encontro_id "
        "JOIN turmas t ON t.id = e.turma_id "
        "WHERE t.periodo_letivo_id = $1",
        periodoLetivoId
    );

    for (const auto& linha : linhas) {
        ensalamento::Alocacao alocacao;
        alocacao.id = linha[0].as<std::string>();
        alocacao.encontroId = linha[1].as<std::string>();
        alocacao.salaId = linha[2].as<std::string>();
        alocacao.status = textoParaStatusAlocacao(linha[3].as<std::string>());

        alocacoes.push_back(alocacao);
    }

    return alocacoes;
}

ensalamento::EstadoEnsalamento RepositorioEstado::carregarEstadoPorPeriodo(
    pqxx::connection& conexao,
    const std::string& periodoLetivoId
) {
    pqxx::work transacao(conexao);

    ensalamento::EstadoEnsalamento estado;
    estado.salas = carregarSalas(transacao);
    estado.turmas = carregarTurmas(transacao, periodoLetivoId);
    estado.encontros = carregarEncontros(transacao, periodoLetivoId);
    estado.alocacoes = carregarAlocacoes(transacao, periodoLetivoId);

    transacao.commit();

    return estado;
}

}