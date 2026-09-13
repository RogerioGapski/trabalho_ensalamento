#include "auth/ValidadorTokenGoogle.hpp"
#include <jwt-cpp/jwt.h>
#include <cstdlib>
#include <stdexcept>

namespace ensalamento_api {

ValidadorTokenGoogle::ValidadorTokenGoogle()
    : provedorChaves("https://www.googleapis.com/oauth2/v3/certs") {
    const char* clientIdAmbiente = std::getenv("GOOGLE_CLIENT_ID");
    if (clientIdAmbiente == nullptr) {
        throw std::runtime_error("Variavel de ambiente GOOGLE_CLIENT_ID nao foi definida");
    }
    clientIdEsperado = std::string(clientIdAmbiente);
}

DadosTokenValidado ValidadorTokenGoogle::validar(const std::string& idToken) {
    auto tokenDecodificado = jwt::decode(idToken);

    if (!tokenDecodificado.has_key_id()) {
        throw std::runtime_error("Token do Google sem identificador de chave (kid)");
    }

    std::string kid = tokenDecodificado.get_key_id();
    std::string chavePem = provedorChaves.obterChavePemPorKid(kid);

    auto verificador = jwt::verify()
        .allow_algorithm(jwt::algorithm::rs256(chavePem, "", "", ""))
        .with_audience(clientIdEsperado);

    verificador.verify(tokenDecodificado);

    if (!tokenDecodificado.has_issuer()) {
        throw std::runtime_error("Token do Google sem emissor");
    }

    std::string emissor = tokenDecodificado.get_issuer();
    if (emissor != "https://accounts.google.com" && emissor != "accounts.google.com") {
        throw std::runtime_error("Emissor do token do Google nao e reconhecido");
    }

    DadosTokenValidado dados;
    dados.subject = tokenDecodificado.get_subject();

    if (!tokenDecodificado.has_payload_claim("email")) {
        throw std::runtime_error("Token do Google nao contem claim de email");
    }
    dados.email = tokenDecodificado.get_payload_claim("email").as_string();

    dados.emailVerificado = false;
    if (tokenDecodificado.has_payload_claim("email_verified")) {
        auto claim = tokenDecodificado.get_payload_claim("email_verified");
        try {
            dados.emailVerificado = claim.as_boolean();
        } catch (const std::exception&) {
            dados.emailVerificado = claim.as_string() == "true";
        }
    }

    if (!dados.emailVerificado) {
        throw std::runtime_error("Email do Google nao esta verificado");
    }

    dados.nome = tokenDecodificado.has_payload_claim("name")
        ? tokenDecodificado.get_payload_claim("name").as_string()
        : dados.email;

    return dados;
}

}