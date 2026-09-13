#ifndef ENSALAMENTO_CORE_TIPOS_HPP
#define ENSALAMENTO_CORE_TIPOS_HPP

#include <string>
#include <set>
#include <vector>
#include <optional>
#include <map>
#include <cstddef>

namespace ensalamento {

enum class TipoSala {
    SalaComum,
    Auditorio,
    Laboratorio,
    Atelie,
    Outro
};

enum class StatusSala {
    Ativa,
    EmManutencao,
    Desativada
};

enum class DiaSemana {
    Segunda,
    Terca,
    Quarta,
    Quinta,
    Sexta,
    Sabado,
    Domingo
};

enum class StatusAlocacao {
    Pendente,
    Ativa,
    Cancelada,
    Conflito
};

enum class CodigoRegra {
    RB01,
    RB02,
    RB03,
    RB04,
    RB05,
    RB06,
    RB07,
    RB08
};

struct HoraDia {
    int horas;
    int minutos;
};

inline int paraMinutos(const HoraDia& hora) {
    return hora.horas * 60 + hora.minutos;
}

inline bool operator<(const HoraDia& esquerda, const HoraDia& direita) {
    return paraMinutos(esquerda) < paraMinutos(direita);
}

inline bool operator<=(const HoraDia& esquerda, const HoraDia& direita) {
    return paraMinutos(esquerda) <= paraMinutos(direita);
}

inline bool operator>(const HoraDia& esquerda, const HoraDia& direita) {
    return paraMinutos(esquerda) > paraMinutos(direita);
}

inline bool operator>=(const HoraDia& esquerda, const HoraDia& direita) {
    return paraMinutos(esquerda) >= paraMinutos(direita);
}

inline bool operator==(const HoraDia& esquerda, const HoraDia& direita) {
    return paraMinutos(esquerda) == paraMinutos(direita);
}

struct Sala {
    std::string id;
    std::string nome;
    std::string predioId;
    std::string campusId;
    TipoSala tipo;
    int capacidadeRegular;
    std::optional<int> capacidadeAvaliacao;
    bool acessivel;
    std::set<std::string> recursos;
    StatusSala status;
};

struct NecessidadeTurma {
    std::optional<TipoSala> tipoSalaObrigatorio;
    bool acessibilidadeSolicitada;
    std::set<std::string> recursosObrigatorios;
    std::optional<std::string> campusPreferido;
    std::optional<std::string> predioPreferido;
};

struct Turma {
    std::string id;
    std::string disciplina;
    std::string coordenacaoId;
    std::set<std::string> professoresIds;
    int quantidadePrevista;
    std::optional<int> quantidadeConfirmada;
    NecessidadeTurma necessidades;

    int tamanhoParaCalculo() const {
        return quantidadeConfirmada.value_or(quantidadePrevista);
    }
};

struct Encontro {
    std::string id;
    std::string turmaId;
    DiaSemana diaSemana;
    HoraDia horaInicio;
    HoraDia horaFim;
    bool permiteDivisaoDeSala;
};

struct Alocacao {
    std::string id;
    std::string encontroId;
    std::string salaId;
    StatusAlocacao status;
};

struct EstadoEnsalamento {
    std::vector<Sala> salas;
    std::vector<Turma> turmas;
    std::vector<Encontro> encontros;
    std::vector<Alocacao> alocacoes;
};

struct Violacao {
    CodigoRegra codigo;
    std::string mensagem;
    std::vector<std::string> idsRelacionados;
};

struct ResultadoValidacao {
    std::vector<Violacao> violacoes;

    bool valido() const {
        return violacoes.empty();
    }
};

std::string paraTexto(TipoSala tipo);
std::string paraTexto(StatusSala status);
std::string paraTexto(DiaSemana dia);
std::string paraTexto(StatusAlocacao status);
std::string paraTexto(CodigoRegra codigo);

}

#endif