#include "auth/ConversorJwkParaPem.hpp"
#include "auth/Base64Url.hpp"
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <stdexcept>

namespace ensalamento_api {

std::string ConversorJwkParaPem::converterParaPem(const std::string& moduloBase64Url, const std::string& expoenteBase64Url) {
    std::vector<unsigned char> moduloBytes = Base64Url::decodificar(moduloBase64Url);
    std::vector<unsigned char> expoenteBytes = Base64Url::decodificar(expoenteBase64Url);

    BIGNUM* modulo = BN_bin2bn(moduloBytes.data(), static_cast<int>(moduloBytes.size()), nullptr);
    BIGNUM* expoente = BN_bin2bn(expoenteBytes.data(), static_cast<int>(expoenteBytes.size()), nullptr);

    if (modulo == nullptr || expoente == nullptr) {
        if (modulo != nullptr) BN_free(modulo);
        if (expoente != nullptr) BN_free(expoente);
        throw std::runtime_error("Falha ao converter modulo/expoente da chave JWK");
    }

    OSSL_PARAM_BLD* construtorParametros = OSSL_PARAM_BLD_new();
    OSSL_PARAM_BLD_push_BN(construtorParametros, OSSL_PKEY_PARAM_RSA_N, modulo);
    OSSL_PARAM_BLD_push_BN(construtorParametros, OSSL_PKEY_PARAM_RSA_E, expoente);
    OSSL_PARAM* parametros = OSSL_PARAM_BLD_to_param(construtorParametros);

    EVP_PKEY_CTX* contexto = EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr);
    EVP_PKEY* chavePublica = nullptr;

    bool falhou = contexto == nullptr
        || EVP_PKEY_fromdata_init(contexto) <= 0
        || EVP_PKEY_fromdata(contexto, &chavePublica, EVP_PKEY_PUBLIC_KEY, parametros) <= 0;

    if (falhou) {
        OSSL_PARAM_BLD_free(construtorParametros);
        OSSL_PARAM_free(parametros);
        if (contexto != nullptr) EVP_PKEY_CTX_free(contexto);
        BN_free(modulo);
        BN_free(expoente);
        throw std::runtime_error("Falha ao construir chave publica RSA a partir do JWK");
    }

    BIO* bufferSaida = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bufferSaida, chavePublica);

    char* dadosPem = nullptr;
    long tamanhoPem = BIO_get_mem_data(bufferSaida, &dadosPem);
    std::string resultadoPem(dadosPem, static_cast<std::size_t>(tamanhoPem));

    BIO_free(bufferSaida);
    EVP_PKEY_free(chavePublica);
    EVP_PKEY_CTX_free(contexto);
    OSSL_PARAM_BLD_free(construtorParametros);
    OSSL_PARAM_free(parametros);
    BN_free(modulo);
    BN_free(expoente);

    return resultadoPem;
}

}