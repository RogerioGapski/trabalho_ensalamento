const RenderizadorAgenda = (function () {
    function construirCardEncontro(encontro, statusTemporal, dataReferencia, textoLinhaExtra) {
        let selo = "";
        if (statusTemporal === "atual") {
            selo = '<span class="selo selo--agora">Agora</span>';
        } else if (statusTemporal === "proxima") {
            selo = '<span class="selo selo--proxima">Próxima</span>';
        }

        let seloAlteracao = "";
        if (LogicaAgenda.foiAlteradaRecentemente(encontro, dataReferencia)) {
            const dataFormatada = LogicaAgenda.formatarDataHoraLegivel(encontro.atualizadoEm);
            seloAlteracao =
                '<span class="selo selo--alterada" title="Atualizado em ' + escaparHtml(dataFormatada) + '">' +
                "Alterada</span>" +
                '<span class="encontro__data-atualizacao">Atualizado em ' + escaparHtml(dataFormatada) + "</span>";
        }

        const linkLocalizacao = encontro.salaId
            ? '<a class="link-ver-localizacao" href="mapa-campus.html?sala_id=' + encodeURIComponent(encontro.salaId) + '">Ver localização →</a>'
            : "";

        return (
            '<div class="encontro" data-encontro-id="' + escaparHtml(encontro.id) + '">' +
            '<div class="encontro__horario">' + escaparHtml(encontro.horaInicio) + "<br>" + escaparHtml(encontro.horaFim) + "</div>" +
            '<div class="encontro__detalhes">' +
            '<p class="encontro__disciplina">' + escaparHtml(encontro.disciplina) + " " + selo + "</p>" +
            '<p class="encontro__local">' +
            escaparHtml(encontro.campusNome) + " · " + escaparHtml(encontro.predioNome) + " · Sala " + escaparHtml(encontro.salaNome) +
            "</p>" +
            (textoLinhaExtra ? '<p class="encontro__local">' + escaparHtml(textoLinhaExtra) + "</p>" : "") +
            seloAlteracao +
            linkLocalizacao +
            "</div>" +
            "</div>"
        );
    }

    function renderizarVisaoDiaria(elementoAlvo, encontrosDoDia, statusPorEncontroId, funcaoLinhaExtra) {
        if (encontrosDoDia.length === 0) {
            elementoAlvo.innerHTML = '<p class="mensagem-estado">Nenhuma aula hoje.</p>';
            return;
        }

        const dataReferencia = new Date();
        let html = "";

        encontrosDoDia.forEach(function (encontro) {
            const statusTemporal = statusPorEncontroId[encontro.id] || "nenhum";
            html += construirCardEncontro(encontro, statusTemporal, dataReferencia, funcaoLinhaExtra(encontro));
        });

        elementoAlvo.innerHTML = html;
    }

    function renderizarVisaoSemanal(elementoAlvo, gruposPorDia, chaveDiaDeHoje, funcaoLinhaExtra) {
        const dataReferencia = new Date();
        let html = "";

        LogicaAgenda.DIAS_SEMANA_EM_ORDEM.forEach(function (diaChave) {
            const encontrosDoDia = gruposPorDia[diaChave];
            const ehHoje = diaChave === chaveDiaDeHoje;
            const statusPorEncontroId = ehHoje
                ? LogicaAgenda.calcularStatusTemporalDoDia(encontrosDoDia, dataReferencia)
                : {};

            html += '<div class="agenda-semanal__dia' + (ehHoje ? " agenda-semanal__dia--hoje" : "") + '">';
            html += "<h3>" + escaparHtml(LogicaAgenda.RÓTULOS_DIAS_SEMANA[diaChave]) + (ehHoje ? " (hoje)" : "") + "</h3>";

            if (encontrosDoDia.length === 0) {
                html += '<p class="mensagem-estado">Sem aulas.</p>';
            } else {
                encontrosDoDia.forEach(function (encontro) {
                    const statusTemporal = statusPorEncontroId[encontro.id] || "nenhum";
                    html += construirCardEncontro(encontro, statusTemporal, dataReferencia, funcaoLinhaExtra(encontro));
                });
            }

            html += "</div>";
        });

        elementoAlvo.innerHTML = html;
    }

    return {
        renderizarVisaoDiaria: renderizarVisaoDiaria,
        renderizarVisaoSemanal: renderizarVisaoSemanal
    };
})();