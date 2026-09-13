#ifndef ENSALAMENTO_API_PROVEDOR_CHAVES_PUBLICAS_HPP
#define ENSALAMENTO_API_PROVEDOR_CHAVES_PUBLICAS_HPP

#include <string>
#include <map>
#include <mutex>
#include <chrono>

namespace ensalamento_api {

class ProvedorChavesPublicas {
public:
    explicit ProvedorChavesPublicas(std::string urlJwks);

    std::string obterChavePemPorKid(const std::string& kid);

private:
    void atualizarChavesSeNecessario();

    std::string urlJwks;
    std::map<std::string, std::string> chavesPorKid;
    std::chrono::steady_clock::time_point ultimaAtualizacao;
    std::mutex mutex;
};

}

#endif