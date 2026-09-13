#include "rotas/RotasAutenticacao.hpp"
#include "auth/ValidadorTokenGoogle.hpp"
#include "auth/ValidadorTokenMicrosoft.hpp"
#include "auth/RepositorioUsuarios.hpp"
#include "auth/GerenciadorSessao.hpp"
#include "auth/CookieSessao.hpp"
#include "db/ConexaoBanco.hpp"
#include <cstdlib>
#include <stdexcept>

namespace ensalamento_api {

static crow::response tratarLoginComProvedor(DadosTokenValidado dadosToken) {
    std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();

    std::optional<UsuarioAutenticavel> usuarioExistente = RepositorioUsuarios::buscarPorEmail(*conexao, dadosToken.email);

    UsuarioAutenticavel usuario;

    if (usuarioExistente.has_value()) {
        if (!usuarioExistente.value().ativo) {
            crow::json::wvalue resposta;
            resposta["erro"] = "Conta desativada";
            return crow::response(403, resposta);
        }
        usuario = usuarioExistente.value();
    } else if (RepositorioUsuarios::dominioPermitido(dadosToken.email)) {
        usuario = RepositorioUsuarios::provisionarComoAluno(*conexao, dadosToken.email, dadosToken.nome);
    } else {
        crow::json::wvalue resposta;
        resposta["erro"] = "Conta nao autorizada a acessar o sistema";
        return crow::response(403, resposta);
    }

    SessaoCriada sessaoCriada = GerenciadorSessao::criarSessao(*conexao, usuario.id);

    int horasValidade = 12;
    const char* horasAmbiente = std::getenv("SESSION_TTL_HORAS");
    if (horasAmbiente != nullptr) {
        horasValidade = std::atoi(horasAmbiente);
    }

    crow::json::wvalue resposta;
    resposta["id"] = usuario.id;
    resposta["nome"] = usuario.nome;
    resposta["email"] = usuario.email;
    resposta["papel"] = usuario.papel;

    crow::response respostaHttp(200, resposta);
    respostaHttp.add_header("Set-Cookie", CookieSessao::construirCabecalhoDefinirCookie(sessaoCriada.tokenBruto, horasValidade));

    return respostaHttp;
}

void RotasAutenticacao::registrar(crow::SimpleApp& aplicacao) {
    static ValidadorTokenGoogle validadorGoogle;
    static ValidadorTokenMicrosoft validadorMicrosoft;

    CROW_ROUTE(aplicacao, "/api/auth/google").methods(crow::HTTPMethod::POST)(
        [](const crow::request& requisicao) {
            try {
                crow::json::rvalue corpo = crow::json::load(requisicao.body);
                if (!corpo || !corpo.has("id_token")) {
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Campo id_token e obrigatorio";
                    return crow::response(400, resposta);
                }

                std::string idToken = corpo["id_token"].s();
                DadosTokenValidado dadosToken = validadorGoogle.validar(idToken);

                return tratarLoginComProvedor(dadosToken);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string("Falha ao validar token do Google: ") + excecao.what();
                return crow::response(401, resposta);
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/auth/microsoft").methods(crow::HTTPMethod::POST)(
        [](const crow::request& requisicao) {
            try {
                crow::json::rvalue corpo = crow::json::load(requisicao.body);
                if (!corpo || !corpo.has("id_token")) {
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Campo id_token e obrigatorio";
                    return crow::response(400, resposta);
                }

                std::string idToken = corpo["id_token"].s();
                DadosTokenValidado dadosToken = validadorMicrosoft.validar(idToken);

                return tratarLoginComProvedor(dadosToken);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string("Falha ao validar token da Microsoft: ") + excecao.what();
                return crow::response(401, resposta);
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/auth/logout").methods(crow::HTTPMethod::POST)(
        [](const crow::request& requisicao) {
            std::string cabecalhoCookie = requisicao.get_header_value("Cookie");
            std::string tokenBruto = CookieSessao::extrairTokenDoCabecalhoCookie(cabecalhoCookie);

            if (!tokenBruto.empty()) {
                try {
                    std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                    GerenciadorSessao::revogarSessao(*conexao, tokenBruto);
                } catch (const std::exception&) {
                }
            }

            crow::json::wvalue resposta;
            resposta["status"] = "sessao encerrada";

            crow::response respostaHttp(200, resposta);
            respostaHttp.add_header("Set-Cookie", CookieSessao::construirCabecalhoRemoverCookie());

            return respostaHttp;
        }
    );

    CROW_ROUTE(aplicacao, "/api/auth/me").methods(crow::HTTPMethod::GET)(
        [](const crow::request& requisicao) {
            std::string cabecalhoCookie = requisicao.get_header_value("Cookie");
            std::string tokenBruto = CookieSessao::extrairTokenDoCabecalhoCookie(cabecalhoCookie);

            if (tokenBruto.empty()) {
                crow::json::wvalue resposta;
                resposta["erro"] = "Nao autenticado";
                return crow::response(401, resposta);
            }

            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                std::optional<UsuarioDaSessao> usuario = GerenciadorSessao::validarSessao(*conexao, tokenBruto);

                if (!usuario.has_value()) {
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Sessao invalida ou expirada";
                    return crow::response(401, resposta);
                }

                crow::json::wvalue resposta;
                resposta["id"] = usuario.value().id;
                resposta["nome"] = usuario.value().nome;
                resposta["email"] = usuario.value().email;
                resposta["papel"] = usuario.value().papel;

                return crow::response(200, resposta);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                return crow::response(500, resposta);
            }
        }
    );
}

}