#ifndef ENSALAMENTO_CORE_INDICE_ADEQUACAO_HPP
#define ENSALAMENTO_CORE_INDICE_ADEQUACAO_HPP

#include "ensalamento/Tipos.hpp"
#include <optional>
#include <string>

namespace ensalamento {

struct ConfiguracaoIndiceAdequacao {
    double pesoSobraDeEspaco;
    double pesoPermanenciaMesmaSala;
    double pesoPermanenciaMesmoPredio;
    double pesoCampusPreferido;
    double pesoPredioPreferido;
};

struct ResultadoIndiceAdequacao {
    double pontuacaoTotal;
    double componenteSobraDeEspaco;
    double componentePermanenciaMesmaSala;
    double componentePermanenciaMesmoPredio;
    double componenteCampusPreferido;
    double componentePredioPreferido;
    bool atendeuPermanenciaMesmaSala;
    bool atendeuPermanenciaMesmoPredio;
    bool atendeuCampusPreferido;
    bool atendeuPredioPreferido;
};

class CalculadoraIndiceAdequacao {
public:
    static ResultadoIndiceAdequacao calcular(
        const Sala& sala,
        const Turma& turma,
        const std::optional<std::string>& salaJaUsadaPelaTurma,
        const std::optional<std::string>& predioJaUsadoPelaTurma,
        const ConfiguracaoIndiceAdequacao& configuracao
    );
};

}

#endif