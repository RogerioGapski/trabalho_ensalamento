#ifndef ENSALAMENTO_API_MIDDLEWARE_AUTENTICACAO_HPP
#define ENSALAMENTO_API_MIDDLEWARE_AUTENTICACAO_HPP

#include "auth/GerenciadorSessao.hpp"
#include "auth/CookieSessao.hpp"
#include "db/ConexaoBanco.hpp"
#include <crow.h>
#include <optional>

namespace ensalamento_api {

struct MiddlewareAutenticacao : crow::ILocalMiddleware {
    struct context {
        std::optional<UsuarioDaSessao> usuarioAutenticado;
    };

    void before_handle(crow::request& requisicao, crow::response&, context& contexto) {
        std::string cabecalhoCookie = requisicao.get_header_value("Cookie");
        std::string tokenBruto = CookieSessao::extrairTokenDoCabecalhoCookie(cabecalhoCookie);

        if (tokenBruto.empty()) {
            return;
        }

        try {
            std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
            contexto.usuarioAutenticado = GerenciadorSessao::validarSessao(*conexao, tokenBruto);
        } catch (const std::exception&) {
            contexto.usuarioAutenticado = std::nullopt;
        }
    }

    void after_handle(crow::request&, crow::response&, context&) {
    }
};

}

#endif