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

static void aplicarCabecalhosCors(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "https://rogeriogapski.github.io");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Cookie");
    res.add_header("Access-Control-Allow-Credentials", "true");
}

static void tratarLoginComProvedor(DadosTokenValidado dadosToken, crow::response& res) {
    aplicarCabecalhosCors(res);
    std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
    std::optional<UsuarioAutenticavel> usuarioExistente = RepositorioUsuarios::buscarPorEmail(*conexao, dadosToken.email);

    UsuarioAutenticavel usuario;

    if (usuarioExistente.has_value()) {
        if (!usuarioExistente.value().ativo) {
            crow::json::wvalue resposta;
            resposta["erro"] = "Conta desativada";
            res.code = 403;
            res.write(resposta.dump());
            res.end();
            return;
        }
        usuario = usuarioExistente.value();
    } else if (RepositorioUsuarios::dominioPermitido(dadosToken.email)) {
        usuario = RepositorioUsuarios::provisionarComoAluno(*conexao, dadosToken.email, dadosToken.nome);
    } else {
        crow::json::wvalue resposta;
        resposta["erro"] = "Conta nao autorizada a acessar o sistema";
        res.code = 403;
        res.write(resposta.dump());
        res.end();
        return;
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

    res.code = 200;
    res.add_header("Set-Cookie", CookieSessao::construirCabecalhoDefinirCookie(sessaoCriada.tokenBruto, horasValidade));
    res.write(resposta.dump());
    res.end();
}

void RotasAutenticacao::registrar(crow::SimpleApp& aplicacao) {
    static ValidadorTokenGoogle validadorGoogle;
    static ValidadorTokenMicrosoft validadorMicrosoft;

    CROW_ROUTE(aplicacao, "/api/auth/google").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)(
        [](const crow::request& req, crow::response& res) {
            aplicarCabecalhosCors(res);

            if (req.method == crow::HTTPMethod::OPTIONS) {
                res.code = 204;
                res.end();
                return;
            }

            try {
                crow::json::rvalue corpo = crow::json::load(req.body);
                if (!corpo || !corpo.has("id_token")) {
                    res.code = 400;
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Campo id_token e obrigatorio";
                    res.write(resposta.dump());
                    res.end();
                    return;
                }

                std::string idToken = corpo["id_token"].s();
                DadosTokenValidado dadosToken = validadorGoogle.validar(idToken);

                tratarLoginComProvedor(dadosToken, res);
            } catch (const std::exception& excecao) {
                aplicarCabecalhosCors(res);
                res.code = 401;
                crow::json::wvalue resposta;
                resposta["erro"] = std::string("Falha ao validar token do Google: ") + excecao.what();
                res.write(resposta.dump());
                res.end();
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/auth/microsoft").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)(
        [](const crow::request& req, crow::response& res) {
            aplicarCabecalhosCors(res);

            if (req.method == crow::HTTPMethod::OPTIONS) {
                res.code = 204;
                res.end();
                return;
            }

            try {
                crow::json::rvalue corpo = crow::json::load(req.body);
                if (!corpo || !corpo.has("id_token")) {
                    res.code = 400;
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Campo id_token e obrigatorio";
                    res.write(resposta.dump());
                    res.end();
                    return;
                }

                std::string idToken = corpo["id_token"].s();
                DadosTokenValidado dadosToken = validadorMicrosoft.validar(idToken);

                tratarLoginComProvedor(dadosToken, res);
            } catch (const std::exception& excecao) {
                aplicarCabecalhosCors(res);
                res.code = 401;
                crow::json::wvalue resposta;
                resposta["erro"] = std::string("Falha ao validar token da Microsoft: ") + excecao.what();
                res.write(resposta.dump());
                res.end();
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/auth/logout").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)(
        [](const crow::request& req, crow::response& res) {
            aplicarCabecalhosCors(res);

            if (req.method == crow::HTTPMethod::OPTIONS) {
                res.code = 204;
                res.end();
                return;
            }

            std::string cabecalhoCookie = req.get_header_value("Cookie");
            std::string tokenBruto = CookieSessao::extrairTokenDoCabecalhoCookie(cabecalhoCookie);

            if (!tokenBruto.empty()) {
                try {
                    std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                    GerenciadorSessao::revogarSessao(*conexao, tokenBruto);
                } catch (const std::exception&) {}
            }

            crow::json::wvalue resposta;
            resposta["status"] = "sessao encerrada";
            res.code = 200;
            res.write(resposta.dump());
            res.add_header("Set-Cookie", CookieSessao::construirCabecalhoRemoverCookie());
            res.end();
        }
    );

    CROW_ROUTE(aplicacao, "/api/auth/me").methods(crow::HTTPMethod::GET, crow::HTTPMethod::OPTIONS)(
        [](const crow::request& req, crow::response& res) {
            aplicarCabecalhosCors(res);

            if (req.method == crow::HTTPMethod::OPTIONS) {
                res.code = 204;
                res.end();
                return;
            }

            std::string cabecalhoCookie = req.get_header_value("Cookie");
            std::string tokenBruto = CookieSessao::extrairTokenDoCabecalhoCookie(cabecalhoCookie);

            if (tokenBruto.empty()) {
                res.code = 401;
                crow::json::wvalue resposta;
                resposta["erro"] = "Nao autenticado";
                res.write(resposta.dump());
                res.end();
                return;
            }

            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                std::optional<UsuarioDaSessao> usuario = GerenciadorSessao::validarSessao(*conexao, tokenBruto);

                if (!usuario.has_value()) {
                    res.code = 401;
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Sessao invalida ou expirada";
                    res.write(resposta.dump());
                    res.end();
                    return;
                }

                crow::json::wvalue resposta;
                resposta["id"] = usuario.value().id;
                resposta["nome"] = usuario.value().nome;
                resposta["email"] = usuario.value().email;
                resposta["papel"] = usuario.value().papel;

                res.code = 200;
                res.write(resposta.dump());
                res.end();
            } catch (const std::exception& excecao) {
                res.code = 500;
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                res.write(resposta.dump());
                res.end();
            }
        }
    );
}

}