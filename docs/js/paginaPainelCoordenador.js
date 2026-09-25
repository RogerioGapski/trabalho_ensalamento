(function () {
    const OPCOES_RECURSOS = ["projetor", "computadores", "microfone", "ar_condicionado", "quadro_digital", "internet_cabeada"];
    const OPCOES_TIPO_SALA = [
        { valor: "", rotulo: "Sem exigência de tipo" },
        { valor: "sala_comum", rotulo: "Sala comum" },
        { valor: "laboratorio", rotulo: "Laboratório" },
        { valor: "auditorio", rotulo: "Auditório" },
        { valor: "outro", rotulo: "Outro" }
    ];

    let usuarioLogado = null;
    let turmasCarregadas = [];

    function rotuloEstado(estado) {
        const rotulos = {
            rascunho: "Rascunho",
            enviada: "Enviada",
            em_revisao: "Devolvida para revisão",
            aprovada: "Aprovada",
            cancelada: "Cancelada",
            encerrada: "Encerrada"
        };
        return rotulos[estado] || estado;
    }

    function podeEditar(estado) {
        return estado === "rascunho" || estado === "em_revisao";
    }

    function renderizarListaTurmas() {
        const elementoLista = document.getElementById("lista-turmas-coordenador");

        if (turmasCarregadas.length === 0) {
            elementoLista.innerHTML = '<p class="mensagem-estado">Nenhuma turma sob sua coordenação neste período.</p>';
            return;
        }

        let html = "";

        turmasCarregadas.forEach(function (turma) {
            const editavel = podeEditar(turma.estado);

            html +=
                '<div class="cartao cartao-turma-coordenador">' +
                '<div class="cartao-turma-coordenador__cabecalho">' +
                "<div>" +
                "<strong>" + escaparHtml(turma.codigo) + "</strong> — " + escaparHtml(turma.disciplina) +
                '<p class="encontro__local" style="margin: 0.2rem 0 0;">' +
                (turma.quantidadeConfirmada != null ? "Confirmados: " + turma.quantidadeConfirmada : "Previstos: " + turma.quantidadePrevista) +
                "</p>" +
                "</div>" +
                '<span class="selo selo--estado-' + escaparHtml(turma.estado) + '">' + escaparHtml(rotuloEstado(turma.estado)) + "</span>" +
                "</div>" +
                (turma.motivoDevolucao
                    ? '<p class="mensagem-erro">Motivo da devolução: ' + escaparHtml(turma.motivoDevolucao) + "</p>"
                    : "") +
                '<div style="margin-top: 0.75rem;">' +
                '<button class="botao botao--primario" data-turma-id="' + escaparHtml(turma.id) + '" data-acao="abrir">' +
                (editavel ? "Editar necessidades" : "Ver necessidades") +
                "</button>" +
                "</div>" +
                "</div>";
        });

        elementoLista.innerHTML = html;

        elementoLista.querySelectorAll("[data-acao='abrir']").forEach(function (botao) {
            botao.addEventListener("click", function () {
                abrirModalTurma(botao.getAttribute("data-turma-id"));
            });
        });
    }

    function construirCheckboxesRecursos(recursosSelecionados, somenteLeitura) {
        return OPCOES_RECURSOS.map(function (recurso) {
            const marcado = recursosSelecionados.indexOf(recurso) !== -1;
            return (
                '<label class="opcao-checkbox">' +
                '<input type="checkbox" name="recurso" value="' + recurso + '" ' +
                (marcado ? "checked " : "") + (somenteLeitura ? "disabled " : "") + "/> " +
                escaparHtml(recurso.replace(/_/g, " ")) +
                "</label>"
            );
        }).join("");
    }

    function construirSelectTipoSala(valorAtual, somenteLeitura) {
        const opcoes = OPCOES_TIPO_SALA.map(function (opcao) {
            const selecionado = (valorAtual || "") === opcao.valor ? "selected" : "";
            return '<option value="' + opcao.valor + '" ' + selecionado + ">" + escaparHtml(opcao.rotulo) + "</option>";
        }).join("");

        return '<select id="campo-tipo-sala" ' + (somenteLeitura ? "disabled" : "") + ">" + opcoes + "</select>";
    }

    function abrirModalTurma(turmaId) {
        const turma = turmasCarregadas.find(function (item) {
            return item.id === turmaId;
        });

        if (!turma) {
            return;
        }

        const editavel = podeEditar(turma.estado);
        const necessidades = turma.necessidades || {};
        const preferencias = turma.preferencias || {};

        document.getElementById("titulo-modal-turma").textContent = turma.codigo + " — " + turma.disciplina;

        const corpoModal = document.getElementById("corpo-modal-turma");
        corpoModal.innerHTML =
            '<div class="campo">' +
            '<label for="campo-quantidade">Quantidade de estudantes' + (turma.quantidadeConfirmada != null ? " (confirmada)" : " (prevista)") + "</label>" +
            '<input type="number" id="campo-quantidade" min="0" value="' +
            (turma.quantidadeConfirmada != null ? turma.quantidadeConfirmada : turma.quantidadePrevista) +
            '" ' + (editavel ? "" : "disabled") + " />" +
            "</div>" +

            '<div class="campo">' +
            "<label>Recursos obrigatórios</label>" +
            '<div class="grade-checkboxes" id="grade-recursos-obrigatorios">' +
            construirCheckboxesRecursos(necessidades.recursosObrigatorios || [], !editavel) +
            "</div>" +
            "</div>" +

            '<div class="campo">' +
            '<label for="campo-tipo-sala">Tipo de sala obrigatório</label>' +
            construirSelectTipoSala(necessidades.tipoSalaObrigatorio, !editavel) +
            "</div>" +

            '<div class="campo">' +
            '<label class="opcao-checkbox">' +
            '<input type="checkbox" id="campo-acessibilidade" ' +
            (necessidades.acessibilidadeSolicitada ? "checked " : "") + (editavel ? "" : "disabled") + "/> " +
            "Necessita de sala acessível (tratado sempre como obrigatório)" +
            "</label>" +
            "</div>" +

            '<div class="campo">' +
            "<label>Recursos preferenciais (não obrigatórios)</label>" +
            '<div class="grade-checkboxes" id="grade-recursos-preferenciais">' +
            construirCheckboxesRecursos(preferencias.recursosDesejados || [], !editavel) +
            "</div>" +
            "</div>" +

            '<div class="campo">' +
            '<label for="campo-justificativa">Justificativa para solicitações especiais</label>' +
            '<textarea id="campo-justificativa" rows="3" ' + (editavel ? "" : "disabled") + ">" +
            escaparHtml(turma.justificativa || "") +
            "</textarea>" +
            "</div>" +

            '<p class="mensagem-erro" id="mensagem-erro-modal" hidden></p>' +

            (editavel
                ? '<div class="modal__acoes">' +
                  '<button class="botao" id="botao-salvar-rascunho" type="button">Salvar rascunho</button>' +
                  '<button class="botao botao--primario" id="botao-enviar-necessidades" type="button">Enviar para o administrador</button>' +
                  "</div>"
                : "");

        document.getElementById("sobreposicao-modal").hidden = false;

        if (editavel) {
            document.getElementById("botao-salvar-rascunho").addEventListener("click", function () {
                salvarNecessidades(turma.id, false);
            });
            document.getElementById("botao-enviar-necessidades").addEventListener("click", function () {
                salvarNecessidades(turma.id, true);
            });
        }
    }

    function coletarValoresDoFormulario() {
        const recursosObrigatorios = Array.from(document.querySelectorAll("#grade-recursos-obrigatorios input:checked")).map(function (input) {
            return input.value;
        });
        const recursosDesejados = Array.from(document.querySelectorAll("#grade-recursos-preferenciais input:checked")).map(function (input) {
            return input.value;
        });

        return {
            quantidadeConfirmada: parseInt(document.getElementById("campo-quantidade").value, 10),
            necessidades: {
                recursosObrigatorios: recursosObrigatorios,
                tipoSalaObrigatorio: document.getElementById("campo-tipo-sala").value || null,
                acessibilidadeSolicitada: document.getElementById("campo-acessibilidade").checked
            },
            preferencias: {
                recursosDesejados: recursosDesejados
            },
            justificativa: document.getElementById("campo-justificativa").value.trim()
        };
    }

    async function salvarNecessidades(turmaId, enviar) {
        const mensagemErro = document.getElementById("mensagem-erro-modal");
        mensagemErro.hidden = true;

        const valores = coletarValoresDoFormulario();
        valores.enviar = enviar;

        if (isNaN(valores.quantidadeConfirmada) || valores.quantidadeConfirmada < 0) {
            mensagemErro.textContent = "Informe uma quantidade de estudantes válida.";
            mensagemErro.hidden = false;
            return;
        }

        try {
            await ClienteApi.salvarNecessidadesTurma(turmaId, valores);
            fecharModal();
            await carregarTurmas();
        } catch (erro) {
            mensagemErro.textContent = erro.message;
            mensagemErro.hidden = false;
        }
    }

    function fecharModal() {
        document.getElementById("sobreposicao-modal").hidden = true;
    }

    async function carregarTurmas() {
        try {
            const resultado = await ClienteApi.buscarTurmasDoCoordenador();
            turmasCarregadas = resultado.turmas || [];
            renderizarListaTurmas();
        } catch (erro) {
            document.getElementById("lista-turmas-coordenador").innerHTML =
                '<p class="mensagem-estado">Não foi possível carregar suas turmas agora.</p>';
        }
    }

    (async function inicializar() {
        usuarioLogado = await Autenticacao.exigirAutenticacaoOuRedirecionar(["coord"]);
        if (!usuarioLogado) {
            return;
        }

        montarCabecalho("cabecalho-app", usuarioLogado.nome, usuarioLogado.papel);

        document.getElementById("botao-fechar-modal").addEventListener("click", fecharModal);
        document.getElementById("sobreposicao-modal").addEventListener("click", function (evento) {
            if (evento.target.id === "sobreposicao-modal") {
                fecharModal();
            }
        });

        await carregarTurmas();
    })();
})();