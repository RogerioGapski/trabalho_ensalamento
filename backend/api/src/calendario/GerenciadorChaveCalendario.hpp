#ifndef ENSALAMENTO_API_GERENCIADOR_CHAVE_CALENDARIO_HPP
#define ENSALAMENTO_API_GERENCIADOR_CHAVE_CALENDARIO_HPP

#include <pqxx/pqxx>
#include <string>
#include <optional>

namespace ensalamento_api {

struct UsuarioPorChaveCalendario {
    std::string id;
    std::string papel;
};

class GerenciadorChaveCalendario {
public:
    static std::string obterOuCriarToken(pqxx::connection& conexao, const std::string& usuarioId);
    static std::optional<UsuarioPorChaveCalendario> buscarUsuarioPorToken(pqxx::connection& conexao, const std::string& token);

private:
    static std::string gerarTokenAleatorio();
};

}

#endif