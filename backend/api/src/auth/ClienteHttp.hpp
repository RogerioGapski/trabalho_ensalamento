#ifndef ENSALAMENTO_API_CLIENTE_HTTP_HPP
#define ENSALAMENTO_API_CLIENTE_HTTP_HPP

#include <string>

namespace ensalamento_api {

class ClienteHttp {
public:
    static std::string obterTexto(const std::string& url);
};

}

#endif