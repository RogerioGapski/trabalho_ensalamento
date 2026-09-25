(async function () {
    const usuario = await Autenticacao.exigirAutenticacaoOuRedirecionar(["aluno"]);
    if (!usuario) {
        return;
    }

    montarCabecalho("cabecalho-app", usuario.nome, usuario.papel);

    let periodoLetivoAtual = null;

    try {
        periodoLetivoAtual = await ClienteApi.buscarPeriodoLetivoAtual();
    } catch (erro) {
        document.getElementById("area-agenda").innerHTML =
            '<p class="mensagem-estado">Não há período letivo ativo no momento.</p>';
        document.getElementById("texto-turma-selecionada").textContent = "—";
        return;
    }

    const preferencia = PreferenciaTurma.obter(usuario.id, periodoLetivoAtual.id);

    if (!preferencia) {
        window.location.href = "selecionar-turma.html";
        return;
    }

    document.getElementById("texto-turma-selecionada").textContent = preferencia.turmaDescricao;

    ComponentesSincronizacao.renderizarBotoes("area-botao-sincronizacao", function () {
        return SincronizacaoCalendario.montarLinksParaAluno(preferencia.turmaId);
    });

    const abaDia = document.getElementById("aba-dia");
    const abaSemana = document.getElementById("aba-semana");
    const areaAgenda = document.getElementById("area-agenda");

    let modoAtual = "dia";
    let encontrosCarregados = [];

    function extrairLinhaExtra(encontro) {
        return encontro.professorNome ? "Professor(a): " + encontro.professorNome : "";
    }

    function renderizarConformeModo() {
        const chaveDiaDeHoje = LogicaAgenda.chaveDiaSemanaDeHoje(new Date());
        const gruposPorDia = LogicaAgenda.agruparEncontrosPorDia(encontrosCarregados);

        if (modoAtual === "dia") {
            const encontrosDeHoje = gruposPorDia[chaveDiaDeHoje];
            const statusPorEncontroId = LogicaAgenda.calcularStatusTemporalDoDia(encontrosDeHoje, new Date());
            RenderizadorAgenda.renderizarVisaoDiaria(areaAgenda, encontrosDeHoje, statusPorEncontroId, extrairLinhaExtra);
        } else {
            RenderizadorAgenda.renderizarVisaoSemanal(areaAgenda, gruposPorDia, chaveDiaDeHoje, extrairLinhaExtra);
        }
    }

    async function buscarDadosDoServidor() {
        areaAgenda.innerHTML = '<p class="mensagem-estado">Carregando agenda...</p>';

        try {
            const resultado = await ClienteApi.buscarAgendaSemanalAluno(preferencia.turmaId);
            encontrosCarregados = resultado.encontros || [];
            renderizarConformeModo();
        } catch (erro) {
            if (erro.status === 404) {
                areaAgenda.innerHTML = '<p class="mensagem-estado">Ainda não há ensalamento publicado para este período.</p>';
            } else if (erro.status === 409) {
                PreferenciaTurma.remover(usuario.id, periodoLetivoAtual.id);
                window.location.href = "selecionar-turma.html";
            } else {
                areaAgenda.innerHTML = '<p class="mensagem-estado">Não foi possível carregar a agenda agora.</p>';
            }
        }
    }

    abaDia.addEventListener("click", function () {
        modoAtual = "dia";
        abaDia.setAttribute("aria-selected", "true");
        abaSemana.setAttribute("aria-selected", "false");
        renderizarConformeModo();
    });

    abaSemana.addEventListener("click", function () {
        modoAtual = "semana";
        abaDia.setAttribute("aria-selected", "false");
        abaSemana.setAttribute("aria-selected", "true");
        renderizarConformeModo();
    });

    await buscarDadosDoServidor();

    window.addEventListener("focus", buscarDadosDoServidor);
    window.setInterval(buscarDadosDoServidor, 5 * 60 * 1000);
    window.setInterval(renderizarConformeModo, 30 * 1000);
})();