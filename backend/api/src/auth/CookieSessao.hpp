#ifndef ENSALAMENTO_API_COOKIE_SESSAO_HPP
#define ENSALAMENTO_API_COOKIE_SESSAO_HPP

#include <string>

namespace ensalamento_api {

class CookieSessao {
public:
    static std::string nomeCookie();
    static std::string construirCabecalhoDefinirCookie(const std::string& tokenBruto, int horasValidade);
    static std::string construirCabecalhoRemoverCookie();
    static std::string extrairTokenDoCabecalhoCookie(const std::string& cabecalhoCookie);
};

}

#endif