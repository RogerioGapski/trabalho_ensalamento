#include "include/ensalamento/IndiceAdequacao.hpp"

namespace ensalamento {

ResultadoIndiceAdequacao CalculadoraIndiceAdequacao::calcular(
    const Sala& sala,
    const Turma& turma,
    const std::optional<std::string>& salaJaUsadaPelaTurma,
    const std::optional<std::string>& predioJaUsadoPelaTurma,
    const ConfiguracaoIndiceAdequacao& configuracao
) {
    ResultadoIndiceAdequacao resultado;

    int sobraDeEspaco = sala.capacidadeRegular - turma.tamanhoParaCalculo();
    if (sobraDeEspaco < 0) {
        sobraDeEspaco = 0;
    }

    resultado.componenteSobraDeEspaco = configuracao.pesoSobraDeEspaco * (1.0 / (1.0 + static_cast<double>(sobraDeEspaco)));

    resultado.atendeuPermanenciaMesmaSala = salaJaUsadaPelaTurma.has_value() && salaJaUsadaPelaTurma.value() == sala.id;
    resultado.componentePermanenciaMesmaSala = resultado.atendeuPermanenciaMesmaSala ? configuracao.pesoPermanenciaMesmaSala : 0.0;

    resultado.atendeuPermanenciaMesmoPredio = predioJaUsadoPelaTurma.has_value() && predioJaUsadoPelaTurma.value() == sala.predioId;
    resultado.componentePermanenciaMesmoPredio = resultado.atendeuPermanenciaMesmoPredio ? configuracao.pesoPermanenciaMesmoPredio : 0.0;

    resultado.atendeuCampusPreferido = turma.necessidades.campusPreferido.has_value() && turma.necessidades.campusPreferido.value() == sala.campusId;
    resultado.componenteCampusPreferido = resultado.atendeuCampusPreferido ? configuracao.pesoCampusPreferido : 0.0;

    resultado.atendeuPredioPreferido = turma.necessidades.predioPreferido.has_value() && turma.necessidades.predioPreferido.value() == sala.predioId;
    resultado.componentePredioPreferido = resultado.atendeuPredioPreferido ? configuracao.pesoPredioPreferido : 0.0;

    resultado.pontuacaoTotal = resultado.componenteSobraDeEspaco
        + resultado.componentePermanenciaMesmaSala
        + resultado.componentePermanenciaMesmoPredio
        + resultado.componenteCampusPreferido
        + resultado.componentePredioPreferido;

    return resultado;
}

}