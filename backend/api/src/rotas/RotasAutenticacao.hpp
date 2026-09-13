#ifndef ENSALAMENTO_API_ROTAS_AUTENTICACAO_HPP
#define ENSALAMENTO_API_ROTAS_AUTENTICACAO_HPP

#include <crow.h>

namespace ensalamento_api {

class RotasAutenticacao {
public:
    static void registrar(crow::SimpleApp& aplicacao);
};

}

#endif