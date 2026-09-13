(async function () {
    const usuario = await Autenticacao.exigirAutenticacaoOuRedirecionar(["aluno"]);
    if (!usuario) {
        return;
    }

    montarCabecalho("cabecalho-app", usuario.nome, usuario.papel);

    const campoBusca = document.getElementById("campo-busca-turma");
    const listaResultados = document.getElementById("lista-resultados-turmas");
    const mensagemEstado = document.getElementById("mensagem-estado-busca");

    let temporizadorBusca = null;

    function exibirMensagem(texto) {
        mensagemEstado.textContent = texto;
        mensagemEstado.hidden = texto === "";
    }

    function renderizarResultados(turmas) {
        listaResultados.innerHTML = "";

        if (turmas.length === 0) {
            exibirMensagem("Nenhuma turma encontrada com esse termo.");
            return;
        }

        exibirMensagem("");

        turmas.forEach(function (turma) {
            const itemLista = document.createElement("li");
            const botaoTurma = document.createElement("button");
            botaoTurma.type = "button";
            botaoTurma.innerHTML =
                "<strong>" + escaparHtml(turma.codigo) + "</strong> — " + escaparHtml(turma.disciplina) +
                "<br><span style=\"color: var(--cor-texto-suave); font-size: 0.85rem;\">" +
                escaparHtml(turma.turno || "") + " · " + escaparHtml(turma.campusNome || "") +
                "</span>";

            botaoTurma.addEventListener("click", function () {
                confirmarSelecao(turma);
            });

            itemLista.appendChild(botaoTurma);
            listaResultados.appendChild(itemLista);
        });
    }

    function confirmarSelecao(turma) {
        const confirmou = window.confirm(
            "Confirmar seleção da turma " + turma.codigo + " — " + turma.disciplina + "?"
        );

        if (!confirmou) {
            return;
        }

        PreferenciaTurma.salvar(usuario.id, turma.periodoLetivoId, turma.id, turma.codigo + " — " + turma.disciplina);
        window.location.href = "agenda-aluno.html";
    }

    campoBusca.addEventListener("input", function () {
        const termo = campoBusca.value.trim();

        if (temporizadorBusca) {
            window.clearTimeout(temporizadorBusca);
        }

        if (termo.length < 2) {
            listaResultados.innerHTML = "";
            exibirMensagem("");
            return;
        }

        temporizadorBusca = window.setTimeout(async function () {
            exibirMensagem("Buscando...");
            try {
                const turmas = await ClienteApi.buscarTurmas(termo);
                renderizarResultados(turmas);
            } catch (erro) {
                exibirMensagem("Não foi possível buscar turmas agora. Tente novamente.");
            }
        }, 300);
    });

    campoBusca.focus();
})();