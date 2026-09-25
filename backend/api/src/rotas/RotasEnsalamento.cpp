#include "rotas/RotasEnsalamento.hpp"
#include "db/ConexaoBanco.hpp"
#include "db/RepositorioEstado.hpp"
#include "db/RepositorioVersao.hpp"
#include "db/RepositorioMetricas.hpp"
#include "db/RepositorioMapa.hpp"
#include "json/SerializadorRelatorio.hpp"
#include "ensalamento/Validador.hpp"
#include "ensalamento/Alocador.hpp"
#include <sstream>

namespace ensalamento_api {

static ensalamento::ConfiguracaoIndiceAdequacao construirConfiguracaoAPartirDoCorpo(const crow::json::rvalue& corpo) {
    ensalamento::ConfiguracaoIndiceAdequacao configuracao;
    configuracao.pesoSobraDeEspaco = 10.0;
    configuracao.pesoPermanenciaMesmaSala = 5.0;
    configuracao.pesoPermanenciaMesmoPredio = 3.0;
    configuracao.pesoCampusPreferido = 4.0;
    configuracao.pesoPredioPreferido = 2.0;

    if (!corpo) {
        return configuracao;
    }

    if (corpo.has("peso_sobra_de_espaco")) {
        configuracao.pesoSobraDeEspaco = corpo["peso_sobra_de_espaco"].d();
    }
    if (corpo.has("peso_permanencia_mesma_sala")) {
        configuracao.pesoPermanenciaMesmaSala = corpo["peso_permanencia_mesma_sala"].d();
    }
    if (corpo.has("peso_permanencia_mesmo_predio")) {
        configuracao.pesoPermanenciaMesmoPredio = corpo["peso_permanencia_mesmo_predio"].d();
    }
    if (corpo.has("peso_campus_preferido")) {
        configuracao.pesoCampusPreferido = corpo["peso_campus_preferido"].d();
    }
    if (corpo.has("peso_predio_preferido")) {
        configuracao.pesoPredioPreferido = corpo["peso_predio_preferido"].d();
    }

    return configuracao;
}

void RotasEnsalamento::registrar(AppComCors& aplicacao) {
    CROW_ROUTE(aplicacao, "/api/saude").methods(crow::HTTPMethod::GET)(
        [](const crow::request&) {
            crow::json::wvalue resposta;
            resposta["status"] = "ok";
            return crow::response(200, resposta);
        }
    );

    CROW_ROUTE(aplicacao, "/api/periodos/<string>/ensalamento/validar").methods(crow::HTTPMethod::GET)(
        [](const crow::request&, const std::string& periodoLetivoId) {
            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                ensalamento::EstadoEnsalamento estado = RepositorioEstado::carregarEstadoPorPeriodo(*conexao, periodoLetivoId);
                ensalamento::ResultadoValidacao resultado = ensalamento::Validador::validar(estado);

                crow::json::wvalue resposta = SerializadorRelatorio::serializarResultadoValidacao(resultado);
                return crow::response(200, resposta);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                return crow::response(500, resposta);
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/periodos/<string>/ensalamento/gerar").methods(crow::HTTPMethod::POST)(
        [](const crow::request& requisicao, const std::string& periodoLetivoId) {
            try {
                crow::json::rvalue corpo = crow::json::load(requisicao.body);

                ensalamento::ConfiguracaoIndiceAdequacao configuracao = construirConfiguracaoAPartirDoCorpo(corpo);

                std::optional<std::string> usuarioId;
                if (corpo && corpo.has("usuario_id")) {
                    usuarioId = std::string(corpo["usuario_id"].s());
                }

                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                ensalamento::EstadoEnsalamento estado = RepositorioEstado::carregarEstadoPorPeriodo(*conexao, periodoLetivoId);

                ensalamento::ResultadoAlocacao resultadoAlocacao = ensalamento::Alocador::alocar(estado, configuracao);

                crow::json::wvalue jsonResultado = SerializadorRelatorio::serializarResultadoAlocacao(resultadoAlocacao);
                std::string dadosSerializados = jsonResultado.dump();

                RascunhoSalvo rascunho = RepositorioVersao::salvarRascunho(
                    *conexao,
                    periodoLetivoId,
                    resultadoAlocacao,
                    dadosSerializados,
                    usuarioId
                );

                crow::json::wvalue resposta;
                resposta["revisao_id"] = rascunho.revisaoId;
                resposta["versao"] = rascunho.versao;
                resposta["resultado"] = std::move(jsonResultado);

                return crow::response(201, resposta);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                return crow::response(500, resposta);
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/admin/metricas/ocupacao").methods(crow::HTTPMethod::GET)(
        [](const crow::request& requisicao) {
            try {
                auto parametros = crow::query_string(requisicao.url_params);
                const char* periodoLetivoIdBruto = parametros.get("periodo_letivo_id");

                if (periodoLetivoIdBruto == nullptr) {
                    crow::json::wvalue resposta;
                    resposta["erro"] = "Parametro periodo_letivo_id e obrigatorio";
                    return crow::response(400, resposta);
                }

                std::string periodoLetivoId(periodoLetivoIdBruto);

                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                MetricasOcupacao metricas = RepositorioMetricas::calcularMetricas(*conexao, periodoLetivoId);

                crow::json::wvalue resposta = SerializadorRelatorio::serializarMetricasOcupacao(metricas);
                return crow::response(200, resposta);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                return crow::response(500, resposta);
            }
        }
    );

    CROW_ROUTE(aplicacao, "/api/mapa/campus").methods(crow::HTTPMethod::GET)(
        [](const crow::request&) {
            try {
                std::unique_ptr<pqxx::connection> conexao = ConexaoBanco::abrirConexao();
                std::vector<CampusMapa> campi = RepositorioMapa::carregarMapaCompleto(*conexao);

                crow::json::wvalue resposta = SerializadorRelatorio::serializarMapaCompleto(campi);
                return crow::response(200, resposta);
            } catch (const std::exception& excecao) {
                crow::json::wvalue resposta;
                resposta["erro"] = std::string(excecao.what());
                return crow::response(500, resposta);
            }
        }
    );
}

}