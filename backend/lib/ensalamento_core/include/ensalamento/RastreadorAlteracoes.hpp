#ifndef ENSALAMENTO_CORE_RASTREADOR_ALTERACOES_HPP
#define ENSALAMENTO_CORE_RASTREADOR_ALTERACOES_HPP

#include "ensalamento/Tipos.hpp"
#include <vector>
#include <string>
#include <map>
#include <cstddef>

namespace ensalamento {

class RastreadorAlteracoes {
public:
    static std::vector<std::string> identificarAlocacoesParaRevalidar(
        const EstadoEnsalamento& estadoAnterior,
        const EstadoEnsalamento& estadoAtual
    );

private:
    static std::size_t calcularHashSala(const Sala& sala);
    static std::size_t calcularHashTurma(const Turma& turma);
    static std::size_t calcularHashEncontro(const Encontro& encontro);

    static std::map<std::string, std::size_t> mapaHashSalas(const EstadoEnsalamento& estado);
    static std::map<std::string, std::size_t> mapaHashTurmas(const EstadoEnsalamento& estado);
    static std::map<std::string, std::size_t> mapaHashEncontros(const EstadoEnsalamento& estado);
    static std::map<std::string, Alocacao> mapaAlocacoesPorId(const EstadoEnsalamento& estado);
    static std::map<std::string, Encontro> mapaEncontrosPorId(const EstadoEnsalamento& estado);

    static void combinarHash(std::size_t& baseHash, std::size_t novoValor);
};

}

#endif