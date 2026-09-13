#include "auth/CookieSessao.hpp"
#include <cstdlib>
#include <sstream>

namespace ensalamento_api {

std::string CookieSessao::nomeCookie() {
    const char* nomeAmbiente = std::getenv("SESSION_COOKIE_NAME");
    return nomeAmbiente != nullptr ? std::string(nomeAmbiente) : std::string("ensalamento_sessao");
}

std::string CookieSessao::construirCabecalhoDefinirCookie(const std::string& tokenBruto, int horasValidade) {
    std::ostringstream cabecalho;
    int segundosValidade = horasValidade * 3600;

    bool cookieSeguro = true;
    const char* cookieSeguroAmbiente = std::getenv("COOKIE_SECURE");
    if (cookieSeguroAmbiente != nullptr && std::string(cookieSeguroAmbiente) == "false") {
        cookieSeguro = false;
    }

    cabecalho << nomeCookie() << "=" << tokenBruto
               << "; Path=/"
               << "; HttpOnly"
               << "; SameSite=Lax"
               << "; Max-Age=" << segundosValidade;

    if (cookieSeguro) {
        cabecalho << "; Secure";
    }

    return cabecalho.str();
}

std::string CookieSessao::construirCabecalhoRemoverCookie() {
    std::ostringstream cabecalho;
    cabecalho << nomeCookie() << "=deleted; Path=/; HttpOnly; SameSite=Lax; Max-Age=0";
    return cabecalho.str();
}

std::string CookieSessao::extrairTokenDoCabecalhoCookie(const std::string& cabecalhoCookie) {
    std::string nomeAlvo = nomeCookie() + "=";
    std::size_t posicaoInicio = cabecalhoCookie.find(nomeAlvo);

    if (posicaoInicio == std::string::npos) {
        return "";
    }

    posicaoInicio += nomeAlvo.size();
    std::size_t posicaoFim = cabecalhoCookie.find(';', posicaoInicio);

    if (posicaoFim == std::string::npos) {
        return cabecalhoCookie.substr(posicaoInicio);
    }

    return cabecalhoCookie.substr(posicaoInicio, posicaoFim - posicaoInicio);
}

}