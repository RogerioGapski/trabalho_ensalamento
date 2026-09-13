#include "include/ensalamento/Validador.hpp"
#include <sstream>
#include <algorithm>

namespace ensalamento {

std::string paraTexto(TipoSala tipo) {
    switch (tipo) {
        case TipoSala::SalaComum: return "sala_comum";
        case TipoSala::Auditorio: return "auditorio";
        case TipoSala::Laboratorio: return "laboratorio";
        case TipoSala::Atelie: return "atelie";
        case TipoSala::Outro: return "outro";
    }
    return "desconhecido";
}

std::string paraTexto(StatusSala status) {
    switch (status) {
        case StatusSala::Ativa: return "ativa";
        case StatusSala::EmManutencao: return "em_manutencao";
        case StatusSala::Desativada: return "desativada";
    }
    return "desconhecido";
}

std::string paraTexto(DiaSemana dia) {
    switch (dia) {
        case DiaSemana::Segunda: return "segunda";
        case DiaSemana::Terca: return "terca";
        case DiaSemana::Quarta: return "quarta";
        case DiaSemana::Quinta: return "quinta";
        case DiaSemana::Sexta: return "sexta";
        case DiaSemana::Sabado: return "sabado";
        case DiaSemana::Domingo: return "domingo";
    }
    return "desconhecido";
}

std::string paraTexto(StatusAlocacao status) {
    switch (status) {
        case StatusAlocacao::Pendente: return "pendente";
        case StatusAlocacao::Ativa: return "ativa";
        case StatusAlocacao::Cancelada: return "cancelada";
        case StatusAlocacao::Conflito: return "conflito";
    }
    return "desconhecido";
}

std::string paraTexto(CodigoRegra codigo) {
    switch (codigo) {
        case CodigoRegra::RB01: return "RB-01";
        case CodigoRegra::RB02: return "RB-02";
        case CodigoRegra::RB03: return "RB-03";
        case CodigoRegra::RB04: return "RB-04";
        case CodigoRegra::RB05: return "RB-05";
        case CodigoRegra::RB06: return "RB-06";
        case CodigoRegra::RB07: return "RB-07";
        case CodigoRegra::RB08: return "RB-08";
    }
    return "RB-00";
}

bool Validador::horariosSobrepoem(const HoraDia& inicioA, const HoraDia& fimA, const HoraDia& inicioB, const HoraDia& fimB) {
    return inicioA < fimB && inicioB < fimA;
}

const Sala* Validador::encontrarSala(const EstadoEnsalamento& estado, const std::string& id) {
    for (const auto& sala : estado.salas) {
        if (sala.id == id) {
            return &sala;
        }
    }
    return nullptr;
}

const Turma* Validador::encontrarTurma(const EstadoEnsalamento& estado, const std::string& id) {
    for (const auto& turma : estado.turmas) {
        if (turma.id == id) {
            return &turma;
        }
    }
    return nullptr;
}

const Encontro* Validador::encontrarEncontro(const EstadoEnsalamento& estado, const std::string& id) {
    for (const auto& encontro : estado.encontros) {
        if (encontro.id == id) {
            return &encontro;
        }
    }
    return nullptr;
}

std::vector<Alocacao> Validador::alocacoesConsideradas(const EstadoEnsalamento& estado) {
    std::vector<Alocacao> resultado;
    for (const auto& alocacao : estado.alocacoes) {
        if (alocacao.status != StatusAlocacao::Cancelada) {
            resultado.push_back(alocacao);
        }
    }
    return resultado;
}

void Validador::validarRB01ChoqueDeSala(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);

    for (std::size_t i = 0; i < alocacoes.size(); ++i) {
        for (std::size_t j = i + 1; j < alocacoes.size(); ++j) {
            const Alocacao& alocacaoA = alocacoes[i];
            const Alocacao& alocacaoB = alocacoes[j];

            if (alocacaoA.salaId != alocacaoB.salaId) {
                continue;
            }

            const Encontro* encontroA = encontrarEncontro(estado, alocacaoA.encontroId);
            const Encontro* encontroB = encontrarEncontro(estado, alocacaoB.encontroId);

            if (encontroA == nullptr || encontroB == nullptr) {
                continue;
            }

            if (encontroA->id == encontroB->id) {
                continue;
            }

            if (encontroA->diaSemana != encontroB->diaSemana) {
                continue;
            }

            if (!horariosSobrepoem(encontroA->horaInicio, encontroA->horaFim, encontroB->horaInicio, encontroB->horaFim)) {
                continue;
            }

            const Sala* sala = encontrarSala(estado, alocacaoA.salaId);
            std::string nomeSala = sala != nullptr ? sala->nome : alocacaoA.salaId;

            std::ostringstream mensagem;
            mensagem << "Sala '" << nomeSala << "' recebeu dois encontros sobrepostos na " << paraTexto(encontroA->diaSemana)
                      << " (alocacoes '" << alocacaoA.id << "' e '" << alocacaoB.id << "')";

            violacoes.push_back(Violacao{
                CodigoRegra::RB01,
                mensagem.str(),
                { alocacaoA.id, alocacaoB.id, alocacaoA.salaId }
            });
        }
    }
}

void Validador::validarRB02DivisaoNaoCadastrada(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);
    std::map<std::string, std::vector<Alocacao>> alocacoesPorEncontro;

    for (const auto& alocacao : alocacoes) {
        alocacoesPorEncontro[alocacao.encontroId].push_back(alocacao);
    }

    for (const auto& parEncontroAlocacoes : alocacoesPorEncontro) {
        const std::string& encontroId = parEncontroAlocacoes.first;
        const std::vector<Alocacao>& alocacoesDoEncontro = parEncontroAlocacoes.second;

        std::set<std::string> salasDistintas;
        for (const auto& alocacao : alocacoesDoEncontro) {
            salasDistintas.insert(alocacao.salaId);
        }

        if (salasDistintas.size() <= 1) {
            continue;
        }

        const Encontro* encontro = encontrarEncontro(estado, encontroId);
        if (encontro != nullptr && encontro->permiteDivisaoDeSala) {
            continue;
        }

        std::vector<std::string> idsRelacionados = { encontroId };
        for (const auto& alocacao : alocacoesDoEncontro) {
            idsRelacionados.push_back(alocacao.id);
        }

        std::ostringstream mensagem;
        mensagem << "Encontro '" << encontroId << "' foi associado a " << salasDistintas.size()
                  << " salas diferentes sem divisao previamente cadastrada";

        violacoes.push_back(Violacao{
            CodigoRegra::RB02,
            mensagem.str(),
            idsRelacionados
        });
    }
}

void Validador::validarRB03Capacidade(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);

    for (const auto& alocacao : alocacoes) {
        const Encontro* encontro = encontrarEncontro(estado, alocacao.encontroId);
        if (encontro == nullptr) {
            continue;
        }

        const Turma* turma = encontrarTurma(estado, encontro->turmaId);
        const Sala* sala = encontrarSala(estado, alocacao.salaId);

        if (turma == nullptr || sala == nullptr) {
            continue;
        }

        int tamanhoParaCalculo = turma->tamanhoParaCalculo();

        if (tamanhoParaCalculo > sala->capacidadeRegular) {
            std::ostringstream mensagem;
            mensagem << "Capacidade da sala '" << sala->nome << "' (" << sala->capacidadeRegular
                      << ") e insuficiente para a turma '" << turma->id << "' com tamanho calculado de "
                      << tamanhoParaCalculo;

            violacoes.push_back(Violacao{
                CodigoRegra::RB03,
                mensagem.str(),
                { alocacao.id, turma->id, sala->id }
            });
        }
    }
}

void Validador::validarRB04SalaIndisponivel(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);

    for (const auto& alocacao : alocacoes) {
        const Sala* sala = encontrarSala(estado, alocacao.salaId);
        if (sala == nullptr) {
            continue;
        }

        if (sala->status == StatusSala::EmManutencao || sala->status == StatusSala::Desativada) {
            std::ostringstream mensagem;
            mensagem << "Sala '" << sala->nome << "' esta com status '" << paraTexto(sala->status)
                      << "' e nao pode receber novos encontros";

            violacoes.push_back(Violacao{
                CodigoRegra::RB04,
                mensagem.str(),
                { alocacao.id, sala->id }
            });
        }
    }
}

void Validador::validarRB05RecursosObrigatorios(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);

    for (const auto& alocacao : alocacoes) {
        const Encontro* encontro = encontrarEncontro(estado, alocacao.encontroId);
        if (encontro == nullptr) {
            continue;
        }

        const Turma* turma = encontrarTurma(estado, encontro->turmaId);
        const Sala* sala = encontrarSala(estado, alocacao.salaId);

        if (turma == nullptr || sala == nullptr) {
            continue;
        }

        std::vector<std::string> recursosFaltantes;
        for (const auto& recurso : turma->necessidades.recursosObrigatorios) {
            if (sala->recursos.find(recurso) == sala->recursos.end()) {
                recursosFaltantes.push_back(recurso);
            }
        }

        if (turma->necessidades.tipoSalaObrigatorio.has_value() && sala->tipo != turma->necessidades.tipoSalaObrigatorio.value()) {
            std::ostringstream mensagemTipo;
            mensagemTipo << "Sala '" << sala->nome << "' e do tipo '" << paraTexto(sala->tipo)
                          << "', mas a turma '" << turma->id << "' exige o tipo '"
                          << paraTexto(turma->necessidades.tipoSalaObrigatorio.value()) << "'";

            violacoes.push_back(Violacao{
                CodigoRegra::RB05,
                mensagemTipo.str(),
                { alocacao.id, turma->id, sala->id }
            });
        }

        if (!recursosFaltantes.empty()) {
            std::ostringstream mensagem;
            mensagem << "Sala '" << sala->nome << "' nao possui os recursos obrigatorios da turma '" << turma->id << "': ";
            for (std::size_t i = 0; i < recursosFaltantes.size(); ++i) {
                mensagem << recursosFaltantes[i];
                if (i + 1 < recursosFaltantes.size()) {
                    mensagem << ", ";
                }
            }

            std::vector<std::string> idsRelacionados = { alocacao.id, turma->id, sala->id };
            for (const auto& recurso : recursosFaltantes) {
                idsRelacionados.push_back(recurso);
            }

            violacoes.push_back(Violacao{
                CodigoRegra::RB05,
                mensagem.str(),
                idsRelacionados
            });
        }
    }
}

void Validador::validarRB06AcessibilidadeObrigatoria(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);

    for (const auto& alocacao : alocacoes) {
        const Encontro* encontro = encontrarEncontro(estado, alocacao.encontroId);
        if (encontro == nullptr) {
            continue;
        }

        const Turma* turma = encontrarTurma(estado, encontro->turmaId);
        const Sala* sala = encontrarSala(estado, alocacao.salaId);

        if (turma == nullptr || sala == nullptr) {
            continue;
        }

        if (turma->necessidades.acessibilidadeSolicitada && !sala->acessivel) {
            std::ostringstream mensagem;
            mensagem << "Turma '" << turma->id << "' solicitou acessibilidade, tratada como obrigatoria, mas a sala '"
                      << sala->nome << "' nao e acessivel";

            violacoes.push_back(Violacao{
                CodigoRegra::RB06,
                mensagem.str(),
                { alocacao.id, turma->id, sala->id }
            });
        }
    }
}

void Validador::validarRB07ChoqueDeProfessor(const EstadoEnsalamento& estado, std::vector<Violacao>& violacoes) {
    std::vector<Alocacao> alocacoes = alocacoesConsideradas(estado);

    for (std::size_t i = 0; i < alocacoes.size(); ++i) {
        for (std::size_t j = i + 1; j < alocacoes.size(); ++j) {
            const Alocacao& alocacaoA = alocacoes[i];
            const Alocacao& alocacaoB = alocacoes[j];

            const Encontro* encontroA = encontrarEncontro(estado, alocacaoA.encontroId);
            const Encontro* encontroB = encontrarEncontro(estado, alocacaoB.encontroId);

            if (encontroA == nullptr || encontroB == nullptr) {
                continue;
            }

            if (encontroA->id == encontroB->id) {
                continue;
            }

            if (encontroA->diaSemana != encontroB->diaSemana) {
                continue;
            }

            const Turma* turmaA = encontrarTurma(estado, encontroA->turmaId);
            const Turma* turmaB = encontrarTurma(estado, encontroB->turmaId);

            if (turmaA == nullptr || turmaB == nullptr) {
                continue;
            }

            if (turmaA->professoresIds.empty() || turmaB->professoresIds.empty()) {
                continue;
            }

            if (turmaA->id == turmaB->id) {
                continue;
            }

            if (!horariosSobrepoem(encontroA->horaInicio, encontroA->horaFim, encontroB->horaInicio, encontroB->horaFim)) {
                continue;
            }

            std::vector<std::string> professoresEmComum;
            for (const auto& professorId : turmaA->professoresIds) {
                if (turmaB->professoresIds.find(professorId) != turmaB->professoresIds.end()) {
                    professoresEmComum.push_back(professorId);
                }
            }

            if (professoresEmComum.empty()) {
                continue;
            }

            for (const auto& professorId : professoresEmComum) {
                std::ostringstream mensagem;
                mensagem << "Professor '" << professorId << "' esta associado a encontros sobrepostos na "
                          << paraTexto(encontroA->diaSemana) << " (alocacoes '" << alocacaoA.id << "' e '"
                          << alocacaoB.id << "')";

                violacoes.push_back(Violacao{
                    CodigoRegra::RB07,
                    mensagem.str(),
                    { alocacaoA.id, alocacaoB.id, professorId }
                });
            }
        }
    }
}

std::vector<Violacao> Validador::verificarRestricoesIntrinsecas(const Sala& sala, const Turma& turma) {
    std::vector<Violacao> violacoes;

    if (sala.status == StatusSala::EmManutencao || sala.status == StatusSala::Desativada) {
        std::ostringstream mensagem;
        mensagem << "Sala '" << sala.nome << "' esta com status '" << paraTexto(sala.status)
                  << "' e nao pode receber novos encontros";
        violacoes.push_back(Violacao{ CodigoRegra::RB04, mensagem.str(), { sala.id } });
    }

    int tamanhoParaCalculo = turma.tamanhoParaCalculo();
    if (tamanhoParaCalculo > sala.capacidadeRegular) {
        std::ostringstream mensagem;
        mensagem << "Capacidade da sala '" << sala.nome << "' (" << sala.capacidadeRegular
                  << ") e insuficiente para a turma '" << turma.id << "' com tamanho calculado de "
                  << tamanhoParaCalculo;
        violacoes.push_back(Violacao{ CodigoRegra::RB03, mensagem.str(), { turma.id, sala.id } });
    }

    if (turma.necessidades.tipoSalaObrigatorio.has_value() && sala.tipo != turma.necessidades.tipoSalaObrigatorio.value()) {
        std::ostringstream mensagem;
        mensagem << "Sala '" << sala.nome << "' e do tipo '" << paraTexto(sala.tipo)
                  << "', mas a turma '" << turma.id << "' exige o tipo '"
                  << paraTexto(turma.necessidades.tipoSalaObrigatorio.value()) << "'";
        violacoes.push_back(Violacao{ CodigoRegra::RB05, mensagem.str(), { turma.id, sala.id } });
    }

    std::vector<std::string> recursosFaltantes;
    for (const auto& recurso : turma.necessidades.recursosObrigatorios) {
        if (sala.recursos.find(recurso) == sala.recursos.end()) {
            recursosFaltantes.push_back(recurso);
        }
    }

    if (!recursosFaltantes.empty()) {
        std::ostringstream mensagem;
        mensagem << "Sala '" << sala.nome << "' nao possui os recursos obrigatorios da turma '" << turma.id << "': ";
        for (std::size_t i = 0; i < recursosFaltantes.size(); ++i) {
            mensagem << recursosFaltantes[i];
            if (i + 1 < recursosFaltantes.size()) {
                mensagem << ", ";
            }
        }

        std::vector<std::string> idsRelacionados = { turma.id, sala.id };
        for (const auto& recurso : recursosFaltantes) {
            idsRelacionados.push_back(recurso);
        }

        violacoes.push_back(Violacao{ CodigoRegra::RB05, mensagem.str(), idsRelacionados });
    }

    if (turma.necessidades.acessibilidadeSolicitada && !sala.acessivel) {
        std::ostringstream mensagem;
        mensagem << "Turma '" << turma.id << "' solicitou acessibilidade, tratada como obrigatoria, mas a sala '"
                  << sala.nome << "' nao e acessivel";
        violacoes.push_back(Violacao{ CodigoRegra::RB06, mensagem.str(), { turma.id, sala.id } });
    }

    return violacoes;
}

std::vector<Violacao> Validador::verificarConflitosDinamicos(
    const EstadoEnsalamento& estado,
    const Encontro& encontroCandidato,
    const Sala& salaCandidata,
    const Turma& turmaCandidata,
    const std::vector<Alocacao>& alocacoesConsideradasAtuais
) {
    std::vector<Violacao> violacoes;

    for (const auto& alocacaoExistente : alocacoesConsideradasAtuais) {
        if (alocacaoExistente.status == StatusAlocacao::Cancelada) {
            continue;
        }

        const Encontro* encontroExistente = encontrarEncontro(estado, alocacaoExistente.encontroId);
        if (encontroExistente == nullptr) {
            continue;
        }

        if (encontroExistente->id == encontroCandidato.id) {
            continue;
        }

        if (encontroExistente->diaSemana != encontroCandidato.diaSemana) {
            continue;
        }

        if (!horariosSobrepoem(encontroExistente->horaInicio, encontroExistente->horaFim, encontroCandidato.horaInicio, encontroCandidato.horaFim)) {
            continue;
        }

        if (alocacaoExistente.salaId == salaCandidata.id) {
            std::ostringstream mensagem;
            mensagem << "Sala '" << salaCandidata.nome << "' ja possui o encontro '" << encontroExistente->id
                      << "' no mesmo horario";
            violacoes.push_back(Violacao{
                CodigoRegra::RB01,
                mensagem.str(),
                { encontroExistente->id, encontroCandidato.id, salaCandidata.id }
            });
        }

        const Turma* turmaExistente = encontrarTurma(estado, encontroExistente->turmaId);
        if (turmaExistente != nullptr && turmaExistente->id != turmaCandidata.id) {
            for (const auto& professorId : turmaCandidata.professoresIds) {
                if (turmaExistente->professoresIds.find(professorId) != turmaExistente->professoresIds.end()) {
                    std::ostringstream mensagem;
                    mensagem << "Professor '" << professorId << "' ja possui o encontro '" << encontroExistente->id
                              << "' no mesmo horario";
                    violacoes.push_back(Violacao{
                        CodigoRegra::RB07,
                        mensagem.str(),
                        { encontroExistente->id, encontroCandidato.id, professorId }
                    });
                }
            }
        }
    }

    return violacoes;
}

ResultadoValidacao Validador::validar(const EstadoEnsalamento& estado) {
    ResultadoValidacao resultado;

    validarRB01ChoqueDeSala(estado, resultado.violacoes);
    validarRB02DivisaoNaoCadastrada(estado, resultado.violacoes);
    validarRB03Capacidade(estado, resultado.violacoes);
    validarRB04SalaIndisponivel(estado, resultado.violacoes);
    validarRB05RecursosObrigatorios(estado, resultado.violacoes);
    validarRB06AcessibilidadeObrigatoria(estado, resultado.violacoes);
    validarRB07ChoqueDeProfessor(estado, resultado.violacoes);

    return resultado;
}

}