#ifndef ENSALAMENTO_API_ROTAS_ENSALAMENTO_HPP
#define ENSALAMENTO_API_ROTAS_ENSALAMENTO_HPP

#include <crow.h>
#include <crow/middlewares/cors.h>

namespace ensalamento_api {

using AppComCors = crow::App<crow::CORSHandler>;

class RotasEnsalamento {
public:
    static void registrar(AppComCors& aplicacao);
};

}

#endif