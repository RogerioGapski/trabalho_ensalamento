#ifndef ENSALAMENTO_API_REPOSITORIO_ESTADO_HPP
#define ENSALAMENTO_API_REPOSITORIO_ESTADO_HPP

#include "ensalamento/Tipos.hpp"
#include <pqxx/pqxx>
#include <string>

namespace ensalamento_api {

class RepositorioEstado {
public:
    static ensalamento::EstadoEnsalamento carregarEstadoPorPeriodo(
        pqxx::connection& conexao,
        const std::string& periodoLetivoId
    );

private:
    static ensalamento::TipoSala textoParaTipoSala(const std::string& texto);
    static ensalamento::DiaSemana textoParaDiaSemana(const std::string& texto);
    static ensalamento::StatusAlocacao textoParaStatusAlocacao(const std::string& texto);
    static ensalamento::HoraDia textoParaHoraDia(const std::string& texto);

    static std::vector<ensalamento::Sala> carregarSalas(pqxx::work& transacao);
    static std::vector<ensalamento::Turma> carregarTurmas(pqxx::work& transacao, const std::string& periodoLetivoId);
    static std::vector<ensalamento::Encontro> carregarEncontros(pqxx::work& transacao, const std::string& periodoLetivoId);
    static std::vector<ensalamento::Alocacao> carregarAlocacoes(pqxx::work& transacao, const std::string& periodoLetivoId);

    static void aplicarNecessidadesObrigatorias(const std::string& jsonTexto, ensalamento::NecessidadeTurma& necessidades);
    static void aplicarPreferencias(const std::string& jsonTexto, ensalamento::NecessidadeTurma& necessidades);
};

}

#endif