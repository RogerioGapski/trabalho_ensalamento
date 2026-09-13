#ifndef ENSALAMENTO_API_REPOSITORIO_VERSAO_HPP
#define ENSALAMENTO_API_REPOSITORIO_VERSAO_HPP

#include "ensalamento/Alocador.hpp"
#include <pqxx/pqxx>
#include <string>
#include <optional>

namespace ensalamento_api {

struct RascunhoSalvo {
    std::string revisaoId;
    int versao;
};

class RepositorioVersao {
public:
    static RascunhoSalvo salvarRascunho(
        pqxx::connection& conexao,
        const std::string& periodoLetivoId,
        const ensalamento::ResultadoAlocacao& resultadoAlocacao,
        const std::string& dadosSerializados,
        const std::optional<std::string>& usuarioId
    );
};

}

#endif