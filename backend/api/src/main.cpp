#include "rotas/RotasEnsalamento.hpp"
#include "rotas/RotasAutenticacao.hpp"
#include <crow.h>
#include <cstdlib>
#include <string>

int main() {
    crow::SimpleApp aplicacao;

    ensalamento_api::RotasAutenticacao::registrar(aplicacao);
    ensalamento_api::RotasEnsalamento::registrar(aplicacao);

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