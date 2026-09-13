#ifndef ENSALAMENTO_API_CONVERSOR_JWK_PARA_PEM_HPP
#define ENSALAMENTO_API_CONVERSOR_JWK_PARA_PEM_HPP

#include <string>

namespace ensalamento_api {

class ConversorJwkParaPem {
public:
    static std::string converterParaPem(const std::string& moduloBase64Url, const std::string& expoenteBase64Url);
};

}

#endif