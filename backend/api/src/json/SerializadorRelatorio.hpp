#ifndef ENSALAMENTO_API_SERIALIZADOR_RELATORIO_HPP
#define ENSALAMENTO_API_SERIALIZADOR_RELATORIO_HPP

#include "ensalamento/Tipos.hpp"
#include "ensalamento/Alocador.hpp"
#include "db/RepositorioMetricas.hpp"
#include <crow.h>
#include "db/RepositorioMapa.hpp"

namespace ensalamento_api {

class SerializadorRelatorio {
public:
    static crow::json::wvalue serializarViolacao(const ensalamento::Violacao& violacao);
    static crow::json::wvalue serializarAvaliacaoSala(const ensalamento::AvaliacaoSala& avaliacao);
    static crow::json::wvalue serializarRelatorioEncontro(const ensalamento::RelatorioEncontro& relatorio);
    static crow::json::wvalue serializarRelatorioTurma(const ensalamento::RelatorioTurma& relatorio);
    static crow::json::wvalue serializarAlocacao(const ensalamento::Alocacao& alocacao);
    static crow::json::wvalue serializarResultadoAlocacao(const ensalamento::ResultadoAlocacao& resultado);
    static crow::json::wvalue serializarResultadoValidacao(const ensalamento::ResultadoValidacao& resultado);
    static crow::json::wvalue serializarMetricasOcupacao(const MetricasOcupacao& metricas);
    static crow::json::wvalue serializarMapaCompleto(const std::vector<CampusMapa>& campi);
};

}

#endif