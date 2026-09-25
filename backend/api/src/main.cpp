#include "rotas/RotasEnsalamento.hpp"
#include "rotas/RotasAutenticacao.hpp"
#include "rotas/RotasCalendario.hpp"
#include <crow.h>
#include <cstdlib>
#include <string>

int main() {
    crow::SimpleApp aplicacao;

    CROW_ROUTE(aplicacao, "/api/<path>").methods(crow::HTTPMethod::Options)
    ([](const crow::request& req, crow::response& res, const std::string& /*path*/) {
        res.add_header("Access-Control-Allow-Origin", "https://rogeriogapski.github.io");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Cookie");
        res.add_header("Access-Control-Allow-Credentials", "true");
        res.code = 204;
        res.end();
    });

    ensalamento_api::RotasAutenticacao::registrar(aplicacao);
    ensalamento_api::RotasEnsalamento::registrar(aplicacao);
    ensalamento_api::RotasCalendario::registrar(aplicacao);

    int porta = 8080;

    const char* portaPlataforma = std::getenv("PORT");
    const char* portaConfigurada = std::getenv("BACKEND_PORT");

    if (portaPlataforma != nullptr) {
        porta = std::stoi(std::string(portaPlataforma));
    } else if (portaConfigurada != nullptr) {
        porta = std::stoi(std::string(portaConfigurada));
    }

    aplicacao.port(porta).multithreaded().run();

    return 0;
}