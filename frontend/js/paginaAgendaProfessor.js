(async function () {
    const usuario = await Autenticacao.exigirAutenticacaoOuRedirecionar(["prof"]);
    if (!usuario) {
        return;
    }

    montarCabecalho("cabecalho-app", usuario.nome, usuario.papel);

    const abaDia = document.getElementById("aba-dia");
    const abaSemana = document.getElementById("aba-semana");
    const areaAgenda = document.getElementById("area-agenda");

    let modoAtual = "dia";
    let encontrosCarregados = [];

    function extrairLinhaExtra(encontro) {
        return encontro.turmaCodigo ? "Turma: " + encontro.turmaCodigo : "";
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
            const resultado = await ClienteApi.buscarAgendaSemanalProfessor();
            encontrosCarregados = resultado.encontros || [];
            renderizarConformeModo();
        } catch (erro) {
            areaAgenda.innerHTML = '<p class="mensagem-estado">Não foi possível carregar a agenda agora.</p>';
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