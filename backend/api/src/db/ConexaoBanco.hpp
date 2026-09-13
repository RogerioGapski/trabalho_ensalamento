#ifndef ENSALAMENTO_API_CONEXAO_BANCO_HPP
#define ENSALAMENTO_API_CONEXAO_BANCO_HPP

#include <pqxx/pqxx>
#include <memory>
#include <string>

namespace ensalamento_api {

class ConexaoBanco {
public:
    static std::string obterStringConexaoDoAmbiente();
    static std::unique_ptr<pqxx::connection> abrirConexao();
};

}

#endif