#include "db/ConexaoBanco.hpp"
#include <cstdlib>
#include <stdexcept>

namespace ensalamento_api {

std::string ConexaoBanco::obterStringConexaoDoAmbiente() {
    const char* valorVariavel = std::getenv("DATABASE_URL");
    if (valorVariavel == nullptr) {
        throw std::runtime_error("Variavel de ambiente DATABASE_URL nao foi definida");
    }
    return std::string(valorVariavel);
}

std::unique_ptr<pqxx::connection> ConexaoBanco::abrirConexao() {
    std::string stringConexao = obterStringConexaoDoAmbiente();
    return std::make_unique<pqxx::connection>(stringConexao);
}

}