#include "json/SerializadorRelatorio.hpp"

namespace ensalamento_api {

crow::json::wvalue SerializadorRelatorio::serializarViolacao(const ensalamento::Violacao& violacao) {
    crow::json::wvalue json;
    json["codigo"] = ensalamento::paraTexto(violacao.codigo);
    json["mensagem"] = violacao.mensagem;

    crow::json::wvalue::list idsRelacionados;
    for (const auto& id : violacao.idsRelacionados) {
        idsRelacionados.push_back(id);
    }
    json["ids_relacionados"] = std::move(idsRelacionados);

    return json;
}

crow::json::wvalue SerializadorRelatorio::serializarAvaliacaoSala(const ensalamento::AvaliacaoSala& avaliacao) {
    crow::json::wvalue json;
    json["sala_id"] = avaliacao.salaId;
    json["elegivel"] = avaliacao.elegivel;
    json["indice_adequacao"] = avaliacao.indiceAdequacao;
    json["atendeu_campus_preferido"] = avaliacao.atendeuCampusPreferido;
    json["atendeu_predio_preferido"] = avaliacao.atendeuPredioPreferido;
    json["atendeu_permanencia_mesma_sala"] = avaliacao.atendeuPermanenciaMesmaSala;
    json["atendeu_permanencia_mesmo_predio"] = avaliacao.atendeuPermanenciaMesmoPredio;

    crow::json::wvalue::list restricoesVioladas;
    for (const auto& violacao : avaliacao.restricoesVioladas) {
        restricoesVioladas.push_back(serializarViolacao(violacao));
    }
    json["restricoes_violadas"] = std::move(restricoesVioladas);

    return json;
}

crow::json::wvalue SerializadorRelatorio::serializarRelatorioEncontro(const ensalamento::RelatorioEncontro& relatorio) {
    crow::json::wvalue json;
    json["encontro_id"] = relatorio.encontroId;
    json["turma_id"] = relatorio.turmaId;
    json["alocado"] = relatorio.alocado;
    json["indice_adequacao_escolhido"] = relatorio.indiceAdequacaoEscolhido;

    if (relatorio.salaEscolhidaId.has_value()) {
        json["sala_escolhida_id"] = relatorio.salaEscolhidaId.value();
    } else {
        json["sala_escolhida_id"] = nullptr;
    }

    crow::json::wvalue::list motivos;
    for (const auto& motivo : relatorio.motivos) {
        motivos.push_back(motivo);
    }
    json["motivos"] = std::move(motivos);

    crow::json::wvalue::list salasAvaliadas;
    for (const auto& avaliacao : relatorio.salasAvaliadas) {
        salasAvaliadas.push_back(serializarAvaliacaoSala(avaliacao));
    }
    json["salas_avaliadas"] = std::move(salasAvaliadas);

    return json;
}

crow::json::wvalue SerializadorRelatorio::serializarRelatorioTurma(const ensalamento::RelatorioTurma& relatorio) {
    crow::json::wvalue json;
    json["turma_id"] = relatorio.turmaId;
    json["totalmente_alocada"] = relatorio.totalmenteAlocada;

    crow::json::wvalue::list relatoriosEncontros;
    for (const auto& relatorioEncontro : relatorio.relatoriosEncontros) {
        relatoriosEncontros.push_back(serializarRelatorioEncontro(relatorioEncontro));
    }
    json["encontros"] = std::move(relatoriosEncontros);

    return json;
}

crow::json::wvalue SerializadorRelatorio::serializarAlocacao(const ensalamento::Alocacao& alocacao) {
    crow::json::wvalue json;
    json["id"] = alocacao.id;
    json["encontro_id"] = alocacao.encontroId;
    json["sala_id"] = alocacao.salaId;
    json["status"] = ensalamento::paraTexto(alocacao.status);
    return json;
}

crow::json::wvalue SerializadorRelatorio::serializarResultadoAlocacao(const ensalamento::ResultadoAlocacao& resultado) {
    crow::json::wvalue json;

    crow::json::wvalue::list alocacoesGeradas;
    for (const auto& alocacao : resultado.alocacoesGeradas) {
        alocacoesGeradas.push_back(serializarAlocacao(alocacao));
    }
    json["alocacoes_geradas"] = std::move(alocacoesGeradas);

    crow::json::wvalue::list relatoriosTurmas;
    for (const auto& relatorioTurma : resultado.relatoriosTurmas) {
        relatoriosTurmas.push_back(serializarRelatorioTurma(relatorioTurma));
    }
    json["relatorios_turmas"] = std::move(relatoriosTurmas);

    return json;
}

crow::json::wvalue SerializadorRelatorio::serializarResultadoValidacao(const ensalamento::ResultadoValidacao& resultado) {
    crow::json::wvalue json;
    json["valido"] = resultado.valido();

    crow::json::wvalue::list violacoes;
    for (const auto& violacao : resultado.violacoes) {
        violacoes.push_back(serializarViolacao(violacao));
    }
    json["violacoes"] = std::move(violacoes);

    return json;
}

}