#ifndef ENSALAMENTO_API_BASE64URL_HPP
#define ENSALAMENTO_API_BASE64URL_HPP

#include <string>
#include <vector>

namespace ensalamento_api {

class Base64Url {
public:
    static std::vector<unsigned char> decodificar(const std::string& textoBase64Url);
};

}

#endif