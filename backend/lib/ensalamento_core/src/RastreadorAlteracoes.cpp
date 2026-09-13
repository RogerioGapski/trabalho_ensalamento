#include "ensalamento/RastreadorAlteracoes.hpp"
#include <functional>

namespace ensalamento {

void RastreadorAlteracoes::combinarHash(std::size_t& baseHash, std::size_t novoValor) {
    baseHash ^= novoValor + 0x9e3779b97f4a7c15ULL + (baseHash << 6) + (baseHash >> 2);
}

std::size_t RastreadorAlteracoes::calcularHashSala(const Sala& sala) {
    std::size_t hashResultado = 0;
    std::hash<std::string> hashTexto;
    std::hash<int> hashInteiro;
    std::hash<bool> hashBooleano;

    combinarHash(hashResultado, hashTexto(sala.id));
    combinarHash(hashResultado, hashTexto(sala.nome));
    combinarHash(hashResultado, hashTexto(sala.predioId));
    combinarHash(hashResultado, hashTexto(sala.campusId));
    combinarHash(hashResultado, hashInteiro(static_cast<int>(sala.tipo)));
    combinarHash(hashResultado, hashInteiro(sala.capacidadeRegular));
    combinarHash(hashResultado, hashInteiro(sala.capacidadeAvaliacao.value_or(-1)));
    combinarHash(hashResultado, hashBooleano(sala.acessivel));
    combinarHash(hashResultado, hashInteiro(static_cast<int>(sala.status)));

    for (const auto& recurso : sala.recursos) {
        combinarHash(hashResultado, hashTexto(recurso));
    }

    return hashResultado;
}

std::size_t RastreadorAlteracoes::calcularHashTurma(const Turma& turma) {
    std::size_t hashResultado = 0;
    std::hash<std::string> hashTexto;
    std::hash<int> hashInteiro;
    std::hash<bool> hashBooleano;

    combinarHash(hashResultado, hashTexto(turma.id));
    combinarHash(hashResultado, hashTexto(turma.disciplina));
    combinarHash(hashResultado, hashTexto(turma.coordenacaoId));
    combinarHash(hashResultado, hashInteiro(turma.quantidadePrevista));
    combinarHash(hashResultado, hashInteiro(turma.quantidadeConfirmada.value_or(-1)));
    combinarHash(hashResultado, hashBooleano(turma.necessidades.acessibilidadeSolicitada));

    if (turma.necessidades.tipoSalaObrigatorio.has_value()) {
        combinarHash(hashResultado, hashInteiro(static_cast<int>(turma.necessidades.tipoSalaObrigatorio.value())));
    }

    for (const auto& professorId : turma.professoresIds) {
        combinarHash(hashResultado, hashTexto(professorId));
    }

    for (const auto& recurso : turma.necessidades.recursosObrigatorios) {
        combinarHash(hashResultado, hashTexto(recurso));
    }

    return hashResultado;
}

std::size_t RastreadorAlteracoes::calcularHashEncontro(const Encontro& encontro) {
    std::size_t hashResultado = 0;
    std::hash<std::string> hashTexto;
    std::hash<int> hashInteiro;
    std::hash<bool> hashBooleano;

    combinarHash(hashResultado, hashTexto(encontro.id));
    combinarHash(hashResultado, hashTexto(encontro.turmaId));
    combinarHash(hashResultado, hashInteiro(static_cast<int>(encontro.diaSemana)));
    combinarHash(hashResultado, hashInteiro(paraMinutos(encontro.horaInicio)));
    combinarHash(hashResultado, hashInteiro(paraMinutos(encontro.horaFim)));
    combinarHash(hashResultado, hashBooleano(encontro.permiteDivisaoDeSala));

    return hashResultado;
}

std::map<std::string, std::size_t> RastreadorAlteracoes::mapaHashSalas(const EstadoEnsalamento& estado) {
    std::map<std::string, std::size_t> resultado;
    for (const auto& sala : estado.salas) {
        resultado[sala.id] = calcularHashSala(sala);
    }
    return resultado;
}

std::map<std::string, std::size_t> RastreadorAlteracoes::mapaHashTurmas(const EstadoEnsalamento& estado) {
    std::map<std::string, std::size_t> resultado;
    for (const auto& turma : estado.turmas) {
        resultado[turma.id] = calcularHashTurma(turma);
    }
    return resultado;
}

std::map<std::string, std::size_t> RastreadorAlteracoes::mapaHashEncontros(const EstadoEnsalamento& estado) {
    std::map<std::string, std::size_t> resultado;
    for (const auto& encontro : estado.encontros) {
        resultado[encontro.id] = calcularHashEncontro(encontro);
    }
    return resultado;
}

std::map<std::string, Alocacao> RastreadorAlteracoes::mapaAlocacoesPorId(const EstadoEnsalamento& estado) {
    std::map<std::string, Alocacao> resultado;
    for (const auto& alocacao : estado.alocacoes) {
        resultado[alocacao.id] = alocacao;
    }
    return resultado;
}

std::map<std::string, Encontro> RastreadorAlteracoes::mapaEncontrosPorId(const EstadoEnsalamento& estado) {
    std::map<std::string, Encontro> resultado;
    for (const auto& encontro : estado.encontros) {
        resultado[encontro.id] = encontro;
    }
    return resultado;
}

std::vector<std::string> RastreadorAlteracoes::identificarAlocacoesParaRevalidar(
    const EstadoEnsalamento& estadoAnterior,
    const EstadoEnsalamento& estadoAtual
) {
    std::vector<std::string> alocacoesParaRevalidar;

    std::map<std::string, std::size_t> hashSalasAnterior = mapaHashSalas(estadoAnterior);
    std::map<std::string, std::size_t> hashTurmasAnterior = mapaHashTurmas(estadoAnterior);
    std::map<std::string, std::size_t> hashEncontrosAnterior = mapaHashEncontros(estadoAnterior);
    std::map<std::string, Alocacao> alocacoesAnteriores = mapaAlocacoesPorId(estadoAnterior);
    std::map<std::string, Encontro> encontrosAnterioresPorId = mapaEncontrosPorId(estadoAnterior);

    std::map<std::string, std::size_t> hashSalasAtual = mapaHashSalas(estadoAtual);
    std::map<std::string, std::size_t> hashTurmasAtual = mapaHashTurmas(estadoAtual);
    std::map<std::string, std::size_t> hashEncontrosAtual = mapaHashEncontros(estadoAtual);
    std::map<std::string, Encontro> encontrosAtuaisPorId = mapaEncontrosPorId(estadoAtual);

    for (const auto& alocacaoAtual : estadoAtual.alocacoes) {
        if (alocacaoAtual.status == StatusAlocacao::Cancelada) {
            continue;
        }

        bool precisaRevalidar = false;

        auto iteradorAlocacaoAnterior = alocacoesAnteriores.find(alocacaoAtual.id);
        if (iteradorAlocacaoAnterior == alocacoesAnteriores.end()) {
            precisaRevalidar = true;
        } else {
            const Alocacao& alocacaoAnterior = iteradorAlocacaoAnterior->second;
            if (alocacaoAnterior.encontroId != alocacaoAtual.encontroId || alocacaoAnterior.salaId != alocacaoAtual.salaId) {
                precisaRevalidar = true;
            }
        }

        if (!precisaRevalidar) {
            auto iteradorHashSalaAnterior = hashSalasAnterior.find(alocacaoAtual.salaId);
            auto iteradorHashSalaAtual = hashSalasAtual.find(alocacaoAtual.salaId);

            bool salaAnteriorExistia = iteradorHashSalaAnterior != hashSalasAnterior.end();
            bool salaAtualExiste = iteradorHashSalaAtual != hashSalasAtual.end();

            if (salaAnteriorExistia != salaAtualExiste) {
                precisaRevalidar = true;
            } else if (salaAtualExiste && iteradorHashSalaAnterior->second != iteradorHashSalaAtual->second) {
                precisaRevalidar = true;
            }
        }

        if (!precisaRevalidar) {
            auto iteradorEncontroAtual = encontrosAtuaisPorId.find(alocacaoAtual.encontroId);
            if (iteradorEncontroAtual != encontrosAtuaisPorId.end()) {
                const std::string& turmaId = iteradorEncontroAtual->second.turmaId;

                auto iteradorHashEncontroAnterior = hashEncontrosAnterior.find(alocacaoAtual.encontroId);
                auto iteradorHashEncontroAtual = hashEncontrosAtual.find(alocacaoAtual.encontroId);

                bool encontroAnteriorExistia = iteradorHashEncontroAnterior != hashEncontrosAnterior.end();
                bool encontroAtualExiste = iteradorHashEncontroAtual != hashEncontrosAtual.end();

                if (encontroAnteriorExistia != encontroAtualExiste) {
                    precisaRevalidar = true;
                } else if (encontroAtualExiste && iteradorHashEncontroAnterior->second != iteradorHashEncontroAtual->second) {
                    precisaRevalidar = true;
                }

                if (!precisaRevalidar) {
                    auto iteradorHashTurmaAnterior = hashTurmasAnterior.find(turmaId);
                    auto iteradorHashTurmaAtual = hashTurmasAtual.find(turmaId);

                    bool turmaAnteriorExistia = iteradorHashTurmaAnterior != hashTurmasAnterior.end();
                    bool turmaAtualExiste = iteradorHashTurmaAtual != hashTurmasAtual.end();

                    if (turmaAnteriorExistia != turmaAtualExiste) {
                        precisaRevalidar = true;
                    } else if (turmaAtualExiste && iteradorHashTurmaAnterior->second != iteradorHashTurmaAtual->second) {
                        precisaRevalidar = true;
                    }
                }
            }
        }

        if (precisaRevalidar) {
            alocacoesParaRevalidar.push_back(alocacaoAtual.id);
        }
    }

    return alocacoesParaRevalidar;
}

}