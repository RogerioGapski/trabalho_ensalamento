#include "rotas/RotasCalendario.hpp"
#include "db/ConexaoBanco.hpp"
#include "db/RepositorioAgendaIcs.hpp"
#include "calendario/GerenciadorChaveCalendario.hpp"
#include "calendario/IcsBuilder.hpp"
#include "auth/GerenciadorSessao.hpp"
#include "auth/CookieSessao.hpp"

namespace ensalamento_api {

void RotasCalendario::registrar(crow::SimpleApp& aplicacao) {
    CROW_ROUTE(aplicacao, "/api/calendario/chave").methods(crow::HTTPMethod::GET)(
        [](const crow::request& requisicao) {
            std::string cabecalhoCookie = requisicao.get_header_value("Cookie");
            std::string tokenSessao = CookieSessao::extrairTokenDoCabecalhoCookie(cabecalhoCookie);

            if (tokenSessao.empty()) {
                crow::json::wvalue resposta;
                resposta["erro"] = "Nao autenticado";
                return crow::response(401, resposta);
            }

            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                std::optional<UsuarioDaSessao> usuario = GerenciadorSessao::validarSessao(*conexao, tokenSessao);

                if (!usuario.has_value()) {
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Sessao invalida ou expirada";
                    return crow::response(401, resposta);
                }

                std::string tokenCalendario = GerenciadorChaveCalendario::obterOuCriarToken(*conexao, usuario.value().id);

                crow::json::wvalue resposta;
                resposta["token"] = tokenCalendario;
                resposta["papel"] = usuario.value().papel;

                return crow::response(200, resposta);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                return crow::response(500, resposta);
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/calendario/aluno.ics").methods(crow::HTTPMethod::GET)(
        [](const crow::request& requisicao) {
            auto parametros = crow::query_string(requisicao.url_params);
            const char* tokenBruto = parametros.get("token");
            const char* turmaIdBruto = parametros.get("turma_id");

            if (tokenBruto == nullptr || turmaIdBruto == nullptr) {
                return crow::response(400, "Parametros token e turma_id sao obrigatorios");
            }

            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                std::optional<UsuarioPorChaveCalendario> usuario = GerenciadorChaveCalendario::buscarUsuarioPorToken(*conexao, std::string(tokenBruto));

                if (!usuario.has_value()) {
                    return crow::response(401, "Token invalido");
                }

                std::vector<EventoRecorrente> eventos = RepositorioAgendaIcs::gerarEventosParaTurma(*conexao, std::string(turmaIdBruto));
                std::string conteudoIcs = IcsBuilder::construirCalendario("Minha agenda academica", eventos);

                crow::response resposta(200, conteudoIcs);
                resposta.set_header("Content-Type", "text/calendar; charset=utf-8");
                resposta.set_header("Content-Disposition", "inline; filename=agenda-academica.ics");
                return resposta;
            } catch (const std::exception& excecao) {
                return crow::response(500, std::string(excecao.what()));
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/calendario/professor.ics").methods(crow::HTTPMethod::GET)(
        [](const crow::request& requisicao) {
            auto parametros = crow::query_string(requisicao.url_params);
            const char* tokenBruto = parametros.get("token");

            if (tokenBruto == nullptr) {
                return crow::response(400, "Parametro token e obrigatorio");
            }

            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                std::optional<UsuarioPorChaveCalendario> usuario = GerenciadorChaveCalendario::buscarUsuarioPorToken(*conexao, std::string(tokenBruto));

                if (!usuario.has_value() || usuario.value().papel != "prof") {
                    return crow::response(401, "Token invalido");
                }

                std::vector<EventoRecorrente> eventos = RepositorioAgendaIcs::gerarEventosParaProfessor(*conexao, usuario.value().id);
                std::string conteudoIcs = IcsBuilder::construirCalendario("Minha agenda de aulas", eventos);

                crow::response resposta(200, conteudoIcs);
                resposta.set_header("Content-Type", "text/calendar; charset=utf-8");
                resposta.set_header("Content-Disposition", "inline; filename=agenda-professor.ics");
                return resposta;
            } catch (const std::exception& excecao) {
                return crow::response(500, std::string(excecao.what()));
            }
        }
    );
}

}