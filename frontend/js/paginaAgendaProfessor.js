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

    function formatarDataIso(data) {
        return data.toISOString().slice(0, 10);
    }

    function renderizarEncontros(encontros) {
        if (!encontros || encontros.length === 0) {
            areaAgenda.innerHTML = '<p class="mensagem-estado">Nenhuma aula encontrada para o período selecionado.</p>';
            return;
        }

        let html = "";

        encontros.forEach(function (encontro) {
            let selo = "";
            if (encontro.statusTemporal === "atual") {
                selo = '<span class="selo selo--agora">Agora</span>';
            } else if (encontro.statusTemporal === "proxima") {
                selo = '<span class="selo selo--proxima">Próxima</span>';
            }

            let seloAlteracao = "";
            if (encontro.alteradaRecentemente) {
                seloAlteracao = '<span class="selo selo--alterada">Alterada</span>';
            }

            html +=
                '<div class="encontro">' +
                '<div class="encontro__horario">' + escaparHtml(encontro.horaInicio) + "<br>" + escaparHtml(encontro.horaFim) + "</div>" +
                '<div class="encontro__detalhes">' +
                "<p class=\"encontro__disciplina\">" + escaparHtml(encontro.disciplina) + " " + selo + " " + seloAlteracao + "</p>" +
                "<p class=\"encontro__local\">" +
                escaparHtml(encontro.campusNome) + " · " + escaparHtml(encontro.predioNome) + " · Sala " + escaparHtml(encontro.salaNome) +
                "</p>" +
                "<p class=\"encontro__local\">Turma: " + escaparHtml(encontro.turmaCodigo) + "</p>" +
                "</div>" +
                "</div>";
        });

        areaAgenda.innerHTML = html;
    }

    async function carregarAgenda() {
        areaAgenda.innerHTML = '<p class="mensagem-estado">Carregando agenda...</p>';

        const dataConsulta = modoAtual === "dia" ? formatarDataIso(new Date()) : "semana-atual";

        try {
            const resultado = await ClienteApi.buscarAgendaProfessor(dataConsulta);
            renderizarEncontros(resultado.encontros);
        } catch (erro) {
            areaAgenda.innerHTML = '<p class="mensagem-estado">Não foi possível carregar a agenda agora.</p>';
        }
    }

    abaDia.addEventListener("click", function () {
        modoAtual = "dia";
        abaDia.setAttribute("aria-selected", "true");
        abaSemana.setAttribute("aria-selected", "false");
        carregarAgenda();
    });

    abaSemana.addEventListener("click", function () {
        modoAtual = "semana";
        abaDia.setAttribute("aria-selected", "false");
        abaSemana.setAttribute("aria-selected", "true");
        carregarAgenda();
    });

    carregarAgenda();

    window.addEventListener("focus", carregarAgenda);
    window.setInterval(carregarAgenda, 5 * 60 * 1000);
})();