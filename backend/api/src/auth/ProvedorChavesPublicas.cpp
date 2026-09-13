#include "auth/ProvedorChavesPublicas.hpp"
#include "auth/ClienteHttp.hpp"
#include "auth/ConversorJwkParaPem.hpp"
#include <crow.h>
#include <stdexcept>

namespace ensalamento_api {

ProvedorChavesPublicas::ProvedorChavesPublicas(std::string urlJwksParametro)
    : urlJwks(std::move(urlJwksParametro)), ultimaAtualizacao(std::chrono::steady_clock::time_point()) {
}

void ProvedorChavesPublicas::atualizarChavesSeNecessario() {
    auto agora = std::chrono::steady_clock::now();
    auto tempoDesdeUltimaAtualizacao = std::chrono::duration_cast<std::chrono::minutes>(agora - ultimaAtualizacao);

    if (!chavesPorKid.empty() && tempoDesdeUltimaAtualizacao.count() < 60) {
        return;
    }

    std::string corpoJson = ClienteHttp::obterTexto(urlJwks);
    crow::json::rvalue json = crow::json::load(corpoJson);

    if (!json || !json.has("keys")) {
        throw std::runtime_error("Resposta JWKS invalida");
    }

    std::map<std::string, std::string> novasChaves;

    for (const auto& chave : json["keys"]) {
        if (!chave.has("kid") || !chave.has("n") || !chave.has("e")) {
            continue;
        }

        std::string kid = chave["kid"].s();
        std::string modulo = chave["n"].s();
        std::string expoente = chave["e"].s();

        try {
            std::string pem = ConversorJwkParaPem::converterParaPem(modulo, expoente);
            novasChaves[kid] = pem;
        } catch (const std::exception&) {
            continue;
        }
    }

    chavesPorKid = novasChaves;
    ultimaAtualizacao = agora;
}

std::string ProvedorChavesPublicas::obterChavePemPorKid(const std::string& kid) {
    std::lock_guard<std::mutex> trava(mutex);

    atualizarChavesSeNecessario();

    auto iterador = chavesPorKid.find(kid);
    if (iterador == chavesPorKid.end()) {
        chavesPorKid.clear();
        ultimaAtualizacao = std::chrono::steady_clock::time_point();
        atualizarChavesSeNecessario();

        iterador = chavesPorKid.find(kid);
        if (iterador == chavesPorKid.end()) {
            throw std::runtime_error("Chave publica com kid informado nao foi encontrada");
        }
    }

    return iterador->second;
}

}