#include "db/RepositorioVersao.hpp"

namespace ensalamento_api {

RascunhoSalvo RepositorioVersao::salvarRascunho(
    pqxx::connection& conexao,
    const std::string& periodoLetivoId,
    const ensalamento::ResultadoAlocacao& resultadoAlocacao,
    const std::string& dadosSerializados,
    const std::optional<std::string>& usuarioId
) {
    pqxx::work transacao(conexao);

    pqxx::result resultadoVersaoAtual = transacao.exec_params(
        "SELECT COALESCE(MAX(versao), 0) + 1 FROM revisoes_versao "
        "WHERE entidade = 'ensalamento_periodo' AND entidade_id = $1",
        periodoLetivoId
    );

    int proximaVersao = resultadoVersaoAtual[0][0].as<int>();

    pqxx::result resultadoInsercaoRevisao = transacao.exec_params(
        "INSERT INTO revisoes_versao (entidade, entidade_id, versao, dados, usuario_id) "
        "VALUES ('ensalamento_periodo', $1, $2, $3::jsonb, $4) "
        "RETURNING id::text",
        periodoLetivoId,
        proximaVersao,
        dadosSerializados,
        usuarioId.has_value() ? pqxx::to_string(usuarioId.value()) : std::string("NULL")
    );

    std::string revisaoId = resultadoInsercaoRevisao[0][0].as<std::string>();

    for (const auto& alocacaoGerada : resultadoAlocacao.alocacoesGeradas) {
        transacao.exec_params(
            "INSERT INTO alocacoes (id, encontro_id, sala_id, status, criado_por) "
            "VALUES ($1, $2, $3, 'pendente', $4) "
            "ON CONFLICT (encontro_id, sala_id) DO UPDATE SET status = 'pendente'",
            alocacaoGerada.id,
            alocacaoGerada.encontroId,
            alocacaoGerada.salaId,
            usuarioId.has_value() ? pqxx::to_string(usuarioId.value()) : std::string("NULL")
        );
    }

    transacao.commit();

    RascunhoSalvo rascunho;
    rascunho.revisaoId = revisaoId;
    rascunho.versao = proximaVersao;

    return rascunho;
}

}