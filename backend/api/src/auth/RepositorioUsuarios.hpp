#ifndef ENSALAMENTO_API_REPOSITORIO_USUARIOS_HPP
#define ENSALAMENTO_API_REPOSITORIO_USUARIOS_HPP

#include <pqxx/pqxx>
#include <string>
#include <optional>

namespace ensalamento_api {

struct UsuarioAutenticavel {
    std::string id;
    std::string nome;
    std::string email;
    std::string papel;
    bool ativo;
};

class RepositorioUsuarios {
public:
    static std::optional<UsuarioAutenticavel> buscarPorEmail(pqxx::connection& conexao, const std::string& email);
    static UsuarioAutenticavel provisionarComoAluno(pqxx::connection& conexao, const std::string& email, const std::string& nome);
    static bool dominioPermitido(const std::string& email);
};

}

#endif