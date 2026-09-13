#ifndef ENSALAMENTO_API_DADOS_TOKEN_VALIDADO_HPP
#define ENSALAMENTO_API_DADOS_TOKEN_VALIDADO_HPP

#include <string>

namespace ensalamento_api {

struct DadosTokenValidado {
    std::string subject;
    std::string email;
    bool emailVerificado;
    std::string nome;
};

}

#endif