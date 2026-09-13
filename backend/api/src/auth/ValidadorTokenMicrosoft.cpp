#include "auth/ValidadorTokenMicrosoft.hpp"
#include <jwt-cpp/jwt.h>
#include <cstdlib>
#include <stdexcept>

namespace ensalamento_api {

ValidadorTokenMicrosoft::ValidadorTokenMicrosoft()
    : provedorChaves("https://login.microsoftonline.com/common/discovery/v2.0/keys") {
    const char* clientIdAmbiente = std::getenv("MICROSOFT_CLIENT_ID");
    if (clientIdAmbiente == nullptr) {
        throw std::runtime_error("Variavel de ambiente MICROSOFT_CLIENT_ID nao foi definida");
    }
    clientIdEsperado = std::string(clientIdAmbiente);
}

DadosTokenValidado ValidadorTokenMicrosoft::validar(const std::string& idToken) {
    auto tokenDecodificado = jwt::decode(idToken);

    if (!tokenDecodificado.has_key_id()) {
        throw std::runtime_error("Token da Microsoft sem identificador de chave (kid)");
    }

    std::string kid = tokenDecodificado.get_key_id();
    std::string chavePem = provedorChaves.obterChavePemPorKid(kid);

    auto verificador = jwt::verify()
        .allow_algorithm(jwt::algorithm::rs256(chavePem, "", "", ""))
        .with_audience(clientIdEsperado);

    verificador.verify(tokenDecodificado);

    if (!tokenDecodificado.has_issuer()) {
        throw std::runtime_error("Token da Microsoft sem emissor");
    }

    std::string emissor = tokenDecodificado.get_issuer();
    bool emissorValido = emissor.rfind("https://login.microsoftonline.com/", 0) == 0
        && emissor.size() > 5
        && emissor.compare(emissor.size() - 5, 5, "/v2.0") == 0;

    if (!emissorValido) {
        throw std::runtime_error("Emissor do token da Microsoft nao e reconhecido");
    }

    DadosTokenValidado dados;
    dados.subject = tokenDecodificado.get_subject();

    if (tokenDecodificado.has_payload_claim("email")) {
        dados.email = tokenDecodificado.get_payload_claim("email").as_string();
    } else if (tokenDecodificado.has_payload_claim("preferred_username")) {
        dados.email = tokenDecodificado.get_payload_claim("preferred_username").as_string();
    } else {
        throw std::runtime_error("Token da Microsoft nao contem claim de email nem preferred_username");
    }

    dados.emailVerificado = true;

    dados.nome = tokenDecodificado.has_payload_claim("name")
        ? tokenDecodificado.get_payload_claim("name").as_string()
        : dados.email;

    return dados;
}

}