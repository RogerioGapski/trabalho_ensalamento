#ifndef ENSALAMENTO_API_GERENCIADOR_SESSAO_HPP
#define ENSALAMENTO_API_GERENCIADOR_SESSAO_HPP

#include <pqxx/pqxx>
#include <string>
#include <optional>

namespace ensalamento_api {

struct SessaoCriada {
    std::string tokenBruto;
    std::string expiraEmIso8601;
};

struct UsuarioDaSessao {
    std::string id;
    std::string nome;
    std::string email;
    std::string papel;
};

class GerenciadorSessao {
public:
    static SessaoCriada criarSessao(pqxx::connection& conexao, const std::string& usuarioId);
    static std::optional<UsuarioDaSessao> validarSessao(pqxx::connection& conexao, const std::string& tokenBruto);
    static void revogarSessao(pqxx::connection& conexao, const std::string& tokenBruto);

private:
    static std::string gerarTokenAleatorio();
    static std::string calcularHashSha256(const std::string& texto);
};

}

#endif