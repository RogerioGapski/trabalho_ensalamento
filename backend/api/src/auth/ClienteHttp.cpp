#include "auth/ClienteHttp.hpp"
#include <curl/curl.h>
#include <stdexcept>

namespace ensalamento_api {

static size_t escreverDados(void* dados, size_t tamanho, size_t quantidade, void* saida) {
    std::size_t tamanhoTotal = tamanho * quantidade;
    static_cast<std::string*>(saida)->append(static_cast<char*>(dados), tamanhoTotal);
    return tamanhoTotal;
}

std::string ClienteHttp::obterTexto(const std::string& url) {
    CURL* manipulador = curl_easy_init();
    if (manipulador == nullptr) {
        throw std::runtime_error("Falha ao inicializar cliente HTTP");
    }

    std::string corpoResposta;

    curl_easy_setopt(manipulador, CURLOPT_URL, url.c_str());
    curl_easy_setopt(manipulador, CURLOPT_WRITEFUNCTION, escreverDados);
    curl_easy_setopt(manipulador, CURLOPT_WRITEDATA, &corpoResposta);
    curl_easy_setopt(manipulador, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(manipulador, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(manipulador, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode codigoResultado = curl_easy_perform(manipulador);

    long codigoStatus = 0;
    curl_easy_getinfo(manipulador, CURLINFO_RESPONSE_CODE, &codigoStatus);

    curl_easy_cleanup(manipulador);

    if (codigoResultado != CURLE_OK) {
        throw std::runtime_error("Falha na requisicao HTTP: " + std::string(curl_easy_strerror(codigoResultado)));
    }

    if (codigoStatus < 200 || codigoStatus >= 300) {
        throw std::runtime_error("Requisicao HTTP retornou status " + std::to_string(codigoStatus));
    }

    return corpoResposta;
}

}