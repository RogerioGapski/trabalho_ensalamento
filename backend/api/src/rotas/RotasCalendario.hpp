#ifndef ENSALAMENTO_API_ROTAS_CALENDARIO_HPP
#define ENSALAMENTO_API_ROTAS_CALENDARIO_HPP

#include <crow.h>
#include <crow/middlewares/cors.h>

namespace ensalamento_api {

using AppComCors = crow::App<crow::CORSHandler>;

class RotasCalendario {
public:
    static void registrar(AppComCors& aplicacao);
};

}

#endif