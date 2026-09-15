#ifndef ENSALAMENTO_API_ROTAS_CALENDARIO_HPP
#define ENSALAMENTO_API_ROTAS_CALENDARIO_HPP

#include <crow.h>

namespace ensalamento_api {

class RotasCalendario {
public:
    static void registrar(crow::SimpleApp& aplicacao);
};

}

#endif