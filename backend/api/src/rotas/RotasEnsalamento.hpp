#ifndef ENSALAMENTO_API_ROTAS_ENSALAMENTO_HPP
#define ENSALAMENTO_API_ROTAS_ENSALAMENTO_HPP

#include <crow.h>

namespace ensalamento_api {

class RotasEnsalamento {
public:
    static void registrar(crow::SimpleApp& aplicacao);
};

}

#endif