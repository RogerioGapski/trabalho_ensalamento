#ifndef ENSALAMENTO_CORE_VALIDADOR_HPP
#define ENSALAMENTO_CORE_VALIDADOR_HPP

#include "ensalamento/Tipos.hpp"

namespace ensalamento {

class Validador {
public:
    static ResultadoValidacao validar(const EstadoEnsalamento& estado);

private:
    static void validarRB01ChoqueDeSala(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);
    static void validarRB02DivisaoNaoCadastrada(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);
    static void validarRB03Capacidade(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);
    static void validarRB04SalaIndisponivel(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);
    static void validarRB05RecursosObrigatorios(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);
    static void validarRB06AcessibilidadeObrigatoria(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);
    static void validarRB07ChoqueDeProfessor(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes);

    static bool horariosSobrepoem(const HoraDia& inicioA, const HoraDia& fimA, const HoraDia& inicioB, const HoraDia& fimB);

    static const Sala* encontrarSala(const EstadoEnsalamento& estado, const std::string& id);
    static const Turma* encontrarTurma(const EstadoEnsalamento& estado, const std::string& id);
    static const Encontro* encontrarEncontro(const EstadoEnsalamento& estado, const std::string& id);

    static std::vector<Alocacao> alocacoesConsideradas(const EstadoEnsalamento& estado);
};

}

#endif