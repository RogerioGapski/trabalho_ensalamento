#ifndef ENSALAMENTO_API_REPOSITORIO_MAPA_HPP
#define ENSALAMENTO_API_REPOSITORIO_MAPA_HPP

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <optional>

namespace ensalamento_api {

struct SalaMapa {
    std::string id;
    std::string nome;
    std::string tipo;
    int andar;
    int capacidade;
    bool acessivel;
    std::vector<std::string> recursos;
};

struct PredioMapa {
    std::string id;
    std::string nome;
    std::optional<std::string> instrucoesAcesso;
    std::vector<SalaMapa> salas;
};

struct CampusMapa {
    std::string id;
    std::string nome;
    std::optional<std::string> endereco;
    std::optional<std::string> cidade;
    std::vector<PredioMapa> predios;
};

class RepositorioMapa {
public:
    static std::vector<CampusMapa> carregarMapaCompleto(pqxx::connection& conexao);
};

}

#endif