#ifndef ENSALAMENTO_TESTES_FIXTURES_HPP
#define ENSALAMENTO_TESTES_FIXTURES_HPP

#include "ensalamento/Tipos.hpp"
#include <string>
#include <IndiceAdequacao.hpp>

namespace testes {

inline ensalamento::Sala criarSala(
    const std::string& id,
    const std::string& predioId,
    const std::string& campusId,
    int capacidade
) {
    ensalamento::Sala sala;
    sala.id = id;
    sala.nome = "Sala " + id;
    sala.predioId = predioId;
    sala.campusId = campusId;
    sala.tipo = ensalamento::TipoSala::SalaComum;
    sala.capacidadeRegular = capacidade;
    sala.acessivel = false;
    sala.status = ensalamento::StatusSala::Ativa;
    return sala;
}

inline ensalamento::Turma criarTurma(const std::string& id, int quantidadePrevista) {
    ensalamento::Turma turma;
    turma.id = id;
    turma.disciplina = "Disciplina " + id;
    turma.coordenacaoId = "coordenacao-1";
    turma.quantidadePrevista = quantidadePrevista;
    turma.necessidades.acessibilidadeSolicitada = false;
    return turma;
}

inline ensalamento::Encontro criarEncontro(
    const std::string& id,
    const std::string& turmaId,
    ensalamento::DiaSemana dia,
    ensalamento::HoraDia inicio,
    ensalamento::HoraDia fim
) {
    ensalamento::Encontro encontro;
    encontro.id = id;
    encontro.turmaId = turmaId;
    encontro.diaSemana = dia;
    encontro.horaInicio = inicio;
    encontro.horaFim = fim;
    encontro.permiteDivisaoDeSala = false;
    return encontro;
}

inline ensalamento::Alocacao criarAlocacao(
    const std::string& id,
    const std::string& encontroId,
    const std::string& salaId,
    ensalamento::StatusAlocacao status = ensalamento::StatusAlocacao::Ativa
) {
    ensalamento::Alocacao alocacao;
    alocacao.id = id;
    alocacao.encontroId = encontroId;
    alocacao.salaId = salaId;
    alocacao.status = status;
    return alocacao;
}

inline ensalamento::ConfiguracaoIndiceAdequacao criarConfiguracaoIndicePadrao() {
    ensalamento::ConfiguracaoIndiceAdequacao configuracao;
    configuracao.pesoSobraDeEspaco = 10.0;
    configuracao.pesoPermanenciaMesmaSala = 5.0;
    configuracao.pesoPermanenciaMesmoPredio = 3.0;
    configuracao.pesoCampusPreferido = 4.0;
    configuracao.pesoPredioPreferido = 2.0;
    return configuracao;
}

inline bool possuiViolacaoComCodigo(const std::vector<ensalamento::Violacao>& violacoes, ensalamento::CodigoRegra codigo) {
    for (const auto& violacao : violacoes) {
        if (violacao.codigo == codigo) {
            return true;
        }
    }
    return false;
}

inline int contarViolacoesComCodigo(const std::vector<ensalamento::Violacao>& violacoes, ensalamento::CodigoRegra codigo) {
    int contador = 0;
    for (const auto& violacao : violacoes) {
        if (violacao.codigo == codigo) {
            ++contador;
        }
    }
    return contador;
}

}

#endif