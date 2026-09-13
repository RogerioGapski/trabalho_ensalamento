#ifndef ENSALAMENTO_CORE_ALOCADOR_HPP
#define ENSALAMENTO_CORE_ALOCADOR_HPP

#include "ensalamento/Tipos.hpp"
#include "ensalamento/IndiceAdequacao.hpp"
#include <vector>
#include <string>
#include <optional>

namespace ensalamento {

struct AvaliacaoSala {
    std::string salaId;
    bool elegivel;
    std::vector<Violacao> restricoesVioladas;
    double indiceAdequacao;
    bool atendeuCampusPreferido;
    bool atendeuPredioPreferido;
    bool atendeuPermanenciaMesmaSala;
    bool atendeuPermanenciaMesmoPredio;
};

struct RelatorioEncontro {
    std::string encontroId;
    std::string turmaId;
    bool alocado;
    std::optional<std::string> salaEscolhidaId;
    double indiceAdequacaoEscolhido;
    std::vector<AvaliacaoSala> salasAvaliadas;
    std::vector<std::string> motivos;
};

struct RelatorioTurma {
    std::string turmaId;
    bool totalmenteAlocada;
    std::vector<RelatorioEncontro> relatoriosEncontros;
};

struct ResultadoAlocacao {
    std::vector<Alocacao> alocacoesGeradas;
    std::vector<RelatorioTurma> relatoriosTurmas;
};

class Alocador {
public:
    static ResultadoAlocacao alocar(const EstadoEnsalamento& estado, const ConfiguracaoIndiceAdequacao& configuracao);

private:
    static int contarSalasElegiveis(const EstadoEnsalamento& estado, const Turma& turma);
    static std::vector<std::string> ordenarTurmasPorDificuldade(const EstadoEnsalamento& estado);
};

}

#endif