#include "include/ensalamento/Alocador.hpp"
#include "include/ensalamento/Validador.hpp"
#include <algorithm>
#include <sstream>
#include <set>
#include <map>

namespace ensalamento {

int Alocador::contarSalasElegiveis(const EstadoEnsalamento& estado, const Turma& turma) {
    int contador = 0;
    for (const auto& sala : estado.salas) {
        std::vector<Violacao> violacoes = Validador::verificarRestricoesIntrinsecas(sala, turma);
        if (violacoes.empty()) {
            ++contador;
        }
    }
    return contador;
}

std::vector<std::string> Alocador::ordenarTurmasPorDificuldade(const EstadoEnsalamento& estado) {
    std::vector<std::pair<std::string, int>> turmasComContagem;

    for (const auto& turma : estado.turmas) {
        turmasComContagem.push_back({ turma.id, contarSalasElegiveis(estado, turma) });
    }

    std::sort(turmasComContagem.begin(), turmasComContagem.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        if (a.second != b.second) {
            return a.second < b.second;
        }
        return a.first < b.first;
    });

    std::vector<std::string> resultado;
    for (const auto& par : turmasComContagem) {
        resultado.push_back(par.first);
    }
    return resultado;
}

ResultadoAlocacao Alocador::alocar(const EstadoEnsalamento& estado, const ConfiguracaoIndiceAdequacao& configuracao) {
    ResultadoAlocacao resultado;

    std::vector<Alocacao> alocacoesAtuais;
    for (const auto& alocacaoExistente : estado.alocacoes) {
        if (alocacaoExistente.status != StatusAlocacao::Cancelada) {
            alocacoesAtuais.push_back(alocacaoExistente);
        }
    }

    std::set<std::string> encontrosJaAlocados;
    for (const auto& alocacaoExistente : alocacoesAtuais) {
        encontrosJaAlocados.insert(alocacaoExistente.encontroId);
    }

    std::map<std::string, std::string> salaEscolhidaPorTurma;
    std::map<std::string, std::string> predioEscolhidoPorTurma;

    for (const auto& alocacaoExistente : alocacoesAtuais) {
        const Encontro* encontroExistente = nullptr;
        for (const auto& encontro : estado.encontros) {
            if (encontro.id == alocacaoExistente.encontroId) {
                encontroExistente = &encontro;
                break;
            }
        }
        if (encontroExistente == nullptr) {
            continue;
        }

        const Sala* salaExistente = nullptr;
        for (const auto& sala : estado.salas) {
            if (sala.id == alocacaoExistente.salaId) {
                salaExistente = &sala;
                break;
            }
        }
        if (salaExistente == nullptr) {
            continue;
        }

        salaEscolhidaPorTurma[encontroExistente->turmaId] = salaExistente->id;
        predioEscolhidoPorTurma[encontroExistente->turmaId] = salaExistente->predioId;
    }

    std::vector<std::string> ordemTurmas = ordenarTurmasPorDificuldade(estado);
    std::size_t contadorAlocacoes = 0;

    for (const auto& turmaId : ordemTurmas) {
        const Turma* turma = nullptr;
        for (const auto& turmaAtual : estado.turmas) {
            if (turmaAtual.id == turmaId) {
                turma = &turmaAtual;
                break;
            }
        }
        if (turma == nullptr) {
            continue;
        }

        RelatorioTurma relatorioTurma;
        relatorioTurma.turmaId = turma->id;
        relatorioTurma.totalmenteAlocada = true;

        std::vector<const Encontro*> encontrosDaTurma;
        for (const auto& encontro : estado.encontros) {
            if (encontro.turmaId == turma->id) {
                encontrosDaTurma.push_back(&encontro);
            }
        }

        std::sort(encontrosDaTurma.begin(), encontrosDaTurma.end(), [](const Encontro* a, const Encontro* b) {
            if (a->diaSemana != b->diaSemana) {
                return static_cast<int>(a->diaSemana) < static_cast<int>(b->diaSemana);
            }
            return paraMinutos(a->horaInicio) < paraMinutos(b->horaInicio);
        });

        for (const auto* encontro : encontrosDaTurma) {
            RelatorioEncontro relatorioEncontro;
            relatorioEncontro.encontroId = encontro->id;
            relatorioEncontro.turmaId = turma->id;
            relatorioEncontro.alocado = false;
            relatorioEncontro.indiceAdequacaoEscolhido = 0.0;

            if (encontrosJaAlocados.find(encontro->id) != encontrosJaAlocados.end()) {
                relatorioEncontro.alocado = true;
                relatorioEncontro.motivos.push_back("Encontro ja possuia alocacao preservada anteriormente");
                relatorioTurma.relatoriosEncontros.push_back(relatorioEncontro);
                continue;
            }

            std::string melhorSalaId;
            double melhorIndice = -1.0;
            bool melhorCampusPreferido = false;
            bool melhorPredioPreferido = false;
            bool melhorPermanenciaSala = false;
            bool melhorPermanenciaPredio = false;

            for (const auto& sala : estado.salas) {
                AvaliacaoSala avaliacao;
                avaliacao.salaId = sala.id;
                avaliacao.indiceAdequacao = 0.0;
                avaliacao.atendeuCampusPreferido = false;
                avaliacao.atendeuPredioPreferido = false;
                avaliacao.atendeuPermanenciaMesmaSala = false;
                avaliacao.atendeuPermanenciaMesmoPredio = false;

                std::vector<Violacao> violacoesIntrinsecas = Validador::verificarRestricoesIntrinsecas(sala, *turma);
                std::vector<Violacao> violacoesDinamicas = Validador::verificarConflitosDinamicos(estado, *encontro, sala, *turma, alocacoesAtuais);

                avaliacao.restricoesVioladas.insert(avaliacao.restricoesVioladas.end(), violacoesIntrinsecas.begin(), violacoesIntrinsecas.end());
                avaliacao.restricoesVioladas.insert(avaliacao.restricoesVioladas.end(), violacoesDinamicas.begin(), violacoesDinamicas.end());

                avaliacao.elegivel = avaliacao.restricoesVioladas.empty();

                if (avaliacao.elegivel) {
                    std::optional<std::string> salaJaUsada;
                    if (salaEscolhidaPorTurma.find(turma->id) != salaEscolhidaPorTurma.end()) {
                        salaJaUsada = salaEscolhidaPorTurma[turma->id];
                    }

                    std::optional<std::string> predioJaUsado;
                    if (predioEscolhidoPorTurma.find(turma->id) != predioEscolhidoPorTurma.end()) {
                        predioJaUsado = predioEscolhidoPorTurma[turma->id];
                    }

                    ResultadoIndiceAdequacao indice = CalculadoraIndiceAdequacao::calcular(
                        sala,
                        *turma,
                        salaJaUsada,
                        predioJaUsado,
                        configuracao
                    );

                    avaliacao.indiceAdequacao = indice.pontuacaoTotal;
                    avaliacao.atendeuCampusPreferido = indice.atendeuCampusPreferido;
                    avaliacao.atendeuPredioPreferido = indice.atendeuPredioPreferido;
                    avaliacao.atendeuPermanenciaMesmaSala = indice.atendeuPermanenciaMesmaSala;
                    avaliacao.atendeuPermanenciaMesmoPredio = indice.atendeuPermanenciaMesmoPredio;

                    bool melhorAinda = avaliacao.indiceAdequacao > melhorIndice;
                    bool empateComDesempate = avaliacao.indiceAdequacao == melhorIndice && (melhorSalaId.empty() || sala.id < melhorSalaId);

                    if (melhorAinda || empateComDesempate) {
                        melhorIndice = avaliacao.indiceAdequacao;
                        melhorSalaId = sala.id;
                        melhorCampusPreferido = avaliacao.atendeuCampusPreferido;
                        melhorPredioPreferido = avaliacao.atendeuPredioPreferido;
                        melhorPermanenciaSala = avaliacao.atendeuPermanenciaMesmaSala;
                        melhorPermanenciaPredio = avaliacao.atendeuPermanenciaMesmoPredio;
                    }
                }

                relatorioEncontro.salasAvaliadas.push_back(avaliacao);
            }

            if (!melhorSalaId.empty()) {
                std::ostringstream idAlocacaoStream;
                idAlocacaoStream << "auto-" << encontro->id << "-" << contadorAlocacoes;
                ++contadorAlocacoes;

                Alocacao novaAlocacao;
                novaAlocacao.id = idAlocacaoStream.str();
                novaAlocacao.encontroId = encontro->id;
                novaAlocacao.salaId = melhorSalaId;
                novaAlocacao.status = StatusAlocacao::Ativa;

                alocacoesAtuais.push_back(novaAlocacao);
                resultado.alocacoesGeradas.push_back(novaAlocacao);

                salaEscolhidaPorTurma[turma->id] = melhorSalaId;

                for (const auto& sala : estado.salas) {
                    if (sala.id == melhorSalaId) {
                        predioEscolhidoPorTurma[turma->id] = sala.predioId;
                        break;
                    }
                }

                relatorioEncontro.alocado = true;
                relatorioEncontro.salaEscolhidaId = melhorSalaId;
                relatorioEncontro.indiceAdequacaoEscolhido = melhorIndice;

                std::ostringstream motivoStream;
                motivoStream << "Sala escolhida por maior indice de adequacao (" << melhorIndice << ")";
                if (melhorCampusPreferido) {
                    motivoStream << "; atendeu campus preferido";
                }
                if (melhorPredioPreferido) {
                    motivoStream << "; atendeu predio preferido";
                }
                if (melhorPermanenciaSala) {
                    motivoStream << "; manteve a turma na mesma sala da semana";
                }
                if (melhorPermanenciaPredio) {
                    motivoStream << "; manteve a turma no mesmo predio da semana";
                }
                relatorioEncontro.motivos.push_back(motivoStream.str());
            } else {
                relatorioTurma.totalmenteAlocada = false;

                std::set<std::string> codigosMotivos;
                for (const auto& avaliacao : relatorioEncontro.salasAvaliadas) {
                    for (const auto& violacao : avaliacao.restricoesVioladas) {
                        codigosMotivos.insert(paraTexto(violacao.codigo));
                    }
                }

                if (estado.salas.empty()) {
                    relatorioEncontro.motivos.push_back("Nenhuma sala cadastrada para avaliacao");
                } else if (codigosMotivos.empty()) {
                    relatorioEncontro.motivos.push_back("Nenhuma sala elegivel por motivo nao identificado");
                } else {
                    std::ostringstream motivoStream;
                    motivoStream << "Nenhuma sala elegivel. Restricoes violadas em pelo menos uma sala candidata: ";
                    std::size_t indiceMotivo = 0;
                    for (const auto& codigo : codigosMotivos) {
                        motivoStream << codigo;
                        if (indiceMotivo + 1 < codigosMotivos.size()) {
                            motivoStream << ", ";
                        }
                        ++indiceMotivo;
                    }
                    relatorioEncontro.motivos.push_back(motivoStream.str());
                }
            }

            relatorioTurma.relatoriosEncontros.push_back(relatorioEncontro);
        }

        resultado.relatoriosTurmas.push_back(relatorioTurma);
    }

    return resultado;
}

}