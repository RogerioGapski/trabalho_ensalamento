#include "rotas/RotasEnsalamento.hpp"
#include "rotas/RotasAutenticacao.hpp"
#include "rotas/RotasCalendario.hpp"
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <cstdlib>
#include <string>

using AppComCors = crow::App<crow::CORSHandler>;

int main() {
    AppComCors aplicacao;

    auto& cors = aplicacao.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("https://rogeriogapski.github.io")
        .methods(crow::HTTPMethod::Post, crow::HTTPMethod::Get, crow::HTTPMethod::Put, crow::HTTPMethod::Delete, crow::HTTPMethod::Options)
        .headers("Content-Type", "Authorization", "Cookie")
        .allow_credentials();

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