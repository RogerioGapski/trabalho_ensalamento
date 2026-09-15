#include "db/RepositorioMapa.hpp"
#include <crow.h>
#include <map>

namespace ensalamento_api {

std::vector<CampusMapa> RepositorioMapa::carregarMapaCompleto(pqxx::connection& conexao) {
    pqxx::work transacao(conexao);

    pqxx::result linhas = transacao.exec(
        "SELECT c.id::text, c.nome, c.endereco, c.cidade, "
        "p.id::text, p.nome, p.instrucoes_acesso, "
        "s.id::text, s.nome, s.tipo::text, s.andar, s.capacidade, s.acessivel, s.recursos::text "
        "FROM campi c "
        "JOIN predios p ON p.campus_id = c.id "
        "LEFT JOIN salas s ON s.predio_id = p.id AND s.ativo = true "
        "ORDER BY c.nome, p.nome, s.andar, s.nome"
    );

    transacao.commit();

    std::vector<CampusMapa> campiOrdenados;
    std::map<std::string, std::size_t> indiceCampusPorId;
    std::map<std::string, std::size_t> indicePredioPorId;

    for (const auto& linha : linhas) {
        std::string campusId = linha[0].as<std::string>();
        std::string campusNome = linha[1].as<std::string>();

        if (indiceCampusPorId.find(campusId) == indiceCampusPorId.end()) {
            CampusMapa campus;
            campus.id = campusId;
            campus.nome = campusNome;
            campus.endereco = linha[2].is_null() ? std::nullopt : std::optional<std::string>(linha[2].as<std::string>());
            campus.cidade = linha[3].is_null() ? std::nullopt : std::optional<std::string>(linha[3].as<std::string>());

            campiOrdenados.push_back(campus);
            indiceCampusPorId[campusId] = campiOrdenados.size() - 1;
        }

        CampusMapa& campusAtual = campiOrdenados[indiceCampusPorId[campusId]];

        std::string predioId = linha[4].as<std::string>();
        std::string chavePredio = campusId + ":" + predioId;

        if (indicePredioPorId.find(chavePredio) == indicePredioPorId.end()) {
            PredioMapa predio;
            predio.id = predioId;
            predio.nome = linha[5].as<std::string>();
            predio.instrucoesAcesso = linha[6].is_null() ? std::nullopt : std::optional<std::string>(linha[6].as<std::string>());

            campusAtual.predios.push_back(predio);
            indicePredioPorId[chavePredio] = campusAtual.predios.size() - 1;
        }

        if (linha[7].is_null()) {
            continue;
        }

        PredioMapa& predioAtual = campusAtual.predios[indicePredioPorId[chavePredio]];

        SalaMapa sala;
        sala.id = linha[7].as<std::string>();
        sala.nome = linha[8].as<std::string>();
        sala.tipo = linha[9].as<std::string>();
        sala.andar = linha[10].as<int>();
        sala.capacidade = linha[11].as<int>();
        sala.acessivel = linha[12].as<bool>();

        crow::json::rvalue jsonRecursos = crow::json::load(linha[13].as<std::string>());
        if (jsonRecursos) {
            for (const auto& recurso : jsonRecursos) {
                sala.recursos.push_back(recurso.s());
            }
        }

        predioAtual.salas.push_back(sala);
    }

    return campiOrdenados;
}

}