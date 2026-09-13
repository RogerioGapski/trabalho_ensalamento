#ifndef ENSALAMENTO_API_VALIDADOR_TOKEN_GOOGLE_HPP
#define ENSALAMENTO_API_VALIDADOR_TOKEN_GOOGLE_HPP

#include "auth/DadosTokenValidado.hpp"
#include "auth/ProvedorChavesPublicas.hpp"
#include <string>

namespace ensalamento_api {

class ValidadorTokenGoogle {
public:
    ValidadorTokenGoogle();

    DadosTokenValidado validar(const std::string& idToken);

private:
    ProvedorChavesPublicas provedorChaves;
    std::string clientIdEsperado;
};

}

#endif