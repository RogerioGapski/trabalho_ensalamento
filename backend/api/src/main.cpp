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
    const char* portaAmbiente = std::getenv("BACKEND_PORT");
    if (portaAmbiente != nullptr) {
        porta = std::stoi(std::string(portaAmbiente));
    }

    aplicacao.port(porta).multithreaded().run();

    return 0;
}