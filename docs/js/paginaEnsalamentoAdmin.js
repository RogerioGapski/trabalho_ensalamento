(function () {
    let usuarioLogado = null;
    let periodoLetivoAtual = null;
    let revisaoIdAtual = null;
    let relatorioAtual = null;

    function contarTurmasAlocadas() {
        return relatorioAtual.relatorios_turmas.filter(function (turma) {
            return turma.totalmente_alocada;
        }).length;
    }

    function contarTurmasPendentes() {
        return relatorioAtual.relatorios_turmas.filter(function (turma) {
            return !turma.totalmente_alocada;
        }).length;
    }

    function renderizarResumo() {
        const cartaoResumo = document.getElementById("cartao-resumo-alocacao");
        cartaoResumo.hidden = false;

        document.getElementById("grade-resumo-alocacao").innerHTML =
            '<div class="resumo-importacao__item resumo-importacao__item--sucesso">' +
            '<span class="resumo-importacao__valor">' + contarTurmasAlocadas() + "</span>" +
            '<span class="resumo-importacao__rotulo">Turmas totalmente alocadas</span></div>' +
            '<div class="resumo-importacao__item resumo-importacao__item--erro">' +
            '<span class="resumo-importacao__valor">' + contarTurmasPendentes() + "</span>" +
            '<span class="resumo-importacao__rotulo">Turmas com pendência</span></div>';
    }

    function renderizarListaTurmas() {
        const elementoLista = document.getElementById("lista-turmas-alocacao");
        let html = "";

        relatorioAtual.relatorios_turmas.forEach(function (relatorioTurma) {
            html += '<div class="cartao">';
            html += "<h3 style=\"margin-top: 0;\">Turma " + escaparHtml(relatorioTurma.turma_id) + "</h3>";

            relatorioTurma.encontros.forEach(function (relatorioEncontro) {
                html += renderizarLinhaEncontro(relatorioEncontro);
            });

            html += "</div>";
        });

        elementoLista.innerHTML = html;

        elementoLista.querySelectorAll("select[data-encontro-id]").forEach(function (seletor) {
            seletor.addEventListener("change", function () {
                moverEncontro(seletor.getAttribute("data-encontro-id"), seletor.value);
            });
        });

        document.getElementById("cartao-publicacao").hidden = contarTurmasPendentes() > 0 && relatorioAtual.relatorios_turmas.length === 0;
        document.getElementById("cartao-publicacao").hidden = false;
    }

    function renderizarLinhaEncontro(relatorioEncontro) {
        const opcoesSalas = relatorioEncontro.salas_avaliadas
            .slice()
            .sort(function (a, b) {
                return b.indice_adequacao - a.indice_adequacao;
            })
            .map(function (avaliacao) {
                const selecionada = avaliacao.sala_id === relatorioEncontro.sala_escolhida_id ? "selected" : "";
                const rotulo =
                    avaliacao.sala_id +
                    (avaliacao.elegivel ? " (índice " + avaliacao.indice_adequacao.toFixed(2) + ")" : " — inelegível");
                return (
                    '<option value="' + escaparHtml(avaliacao.sala_id) + '" ' + selecionada + " " +
                    (avaliacao.elegivel ? "" : "disabled") + ">" + escaparHtml(rotulo) + "</option>"
                );
            })
            .join("");

        const listaMotivos = relatorioEncontro.motivos
            .map(function (motivo) {
                return "<li>" + escaparHtml(motivo) + "</li>";
            })
            .join("");

        return (
            '<div class="linha-encontro-alocacao" data-encontro-alocacao-id="' + escaparHtml(relatorioEncontro.encontro_id) + '">' +
            "<div>" +
            "<strong>Encontro " + escaparHtml(relatorioEncontro.encontro_id) + "</strong>" +
            '<span class="selo ' + (relatorioEncontro.alocado ? "selo--agora" : "selo--alterada") + '">' +
            (relatorioEncontro.alocado ? "Alocado" : "Pendente") +
            "</span>" +
            "<ul class=\"lista-motivos\">" + listaMotivos + "</ul>" +
            "</div>" +
            '<div class="campo" style="margin: 0;">' +
            "<label>Sala</label>" +
            '<select data-encontro-id="' + escaparHtml(relatorioEncontro.encontro_id) + '">' +
            '<option value="">— nenhuma —</option>' +
            opcoesSalas +
            "</select>" +
            "</div>" +
            '<div class="mensagem-erro" data-mensagem-encontro-id="' + escaparHtml(relatorioEncontro.encontro_id) + '" hidden></div>' +
            "</div>"
        );
    }

    async function gerarProposta() {
        const botaoGerar = document.getElementById("botao-gerar-proposta");
        botaoGerar.disabled = true;
        botaoGerar.textContent = "Gerando...";

        try {
            const resultado = await ClienteApi.gerarPropostaEnsalamento(periodoLetivoAtual.id, {});
            revisaoIdAtual = resultado.revisao_id;
            relatorioAtual = resultado.resultado;
            renderizarResumo();
            renderizarListaTurmas();
        } catch (erro) {
            window.alert("Não foi possível gerar a proposta: " + erro.message);
        } finally {
            botaoGerar.disabled = false;
            botaoGerar.textContent = "Gerar proposta";
        }
    }

    async function moverEncontro(encontroId, novaSalaId) {
        const elementoMensagem = document.querySelector("[data-mensagem-encontro-id='" + encontroId + "']");
        elementoMensagem.hidden = true;

        if (!novaSalaId) {
            return;
        }

        try {
            const resultado = await ClienteApi.moverEncontroNaRevisao(periodoLetivoAtual.id, revisaoIdAtual, encontroId, novaSalaId);

            if (resultado.violacoes && resultado.violacoes.length > 0) {
                elementoMensagem.hidden = false;
                elementoMensagem.textContent = resultado.violacoes.map(function (violacao) {
                    return violacao.codigo + ": " + violacao.mensagem;
                }).join(" | ");
            } else {
                relatorioAtual.relatorios_turmas.forEach(function (relatorioTurma) {
                    relatorioTurma.encontros.forEach(function (relatorioEncontro) {
                        if (relatorioEncontro.encontro_id === encontroId) {
                            relatorioEncontro.sala_escolhida_id = novaSalaId;
                            relatorioEncontro.alocado = true;
                        }
                    });
                });
                renderizarResumo();
            }
        } catch (erro) {
            elementoMensagem.hidden = false;
            elementoMensagem.textContent = "Falha ao revalidar: " + erro.message;
        }
    }

    async function publicarVersao() {
        const descricao = document.getElementById("campo-descricao-publicacao").value.trim();
        const mensagemErro = document.getElementById("mensagem-erro-publicacao");
        mensagemErro.hidden = true;

        if (!descricao) {
            mensagemErro.textContent = "Informe uma descrição para a versão.";
            mensagemErro.hidden = false;
            return;
        }

        if (contarTurmasPendentes() > 0) {
            const confirmou = window.confirm(
                "Existem " + contarTurmasPendentes() + " turma(s) com pendência. Turmas pendentes não terão sala publicada. Deseja continuar?"
            );
            if (!confirmou) {
                return;
            }
        }

        const botaoPublicar = document.getElementById("botao-publicar");
        botaoPublicar.disabled = true;
        botaoPublicar.textContent = "Publicando...";

        try {
            const resultado = await ClienteApi.publicarVersaoEnsalamento(periodoLetivoAtual.id, revisaoIdAtual, descricao);
            window.alert("Versão " + resultado.versao + " publicada com sucesso.");
        } catch (erro) {
            mensagemErro.textContent = "Falha ao publicar: " + erro.message;
            mensagemErro.hidden = false;
        } finally {
            botaoPublicar.disabled = false;
            botaoPublicar.textContent = "Publicar versão";
        }
    }

    (async function inicializar() {
        usuarioLogado = await Autenticacao.exigirAutenticacaoOuRedirecionar(["admin"]);
        if (!usuarioLogado) {
            return;
        }

        montarCabecalho("cabecalho-app", usuarioLogado.nome, usuarioLogado.papel);

        try {
            periodoLetivoAtual = await ClienteApi.buscarPeriodoLetivoAtual();
        } catch (erro) {
            document.querySelector("main").innerHTML =
                '<div class="cartao"><p class="mensagem-estado">Não há período letivo ativo no momento.</p></div>';
            return;
        }

        document.getElementById("botao-gerar-proposta").addEventListener("click", gerarProposta);
        document.getElementById("botao-publicar").addEventListener("click", publicarVersao);
    })();
})();