const SincronizacaoCalendario = (function () {
    function construirUrlHttpsAluno(token, turmaId) {
        const urlBase = window.CONFIGURACAO_ENSALAMENTO.urlBaseApi.replace(/\/$/, "");
        return urlBase + "/api/calendario/aluno.ics?turma_id=" + encodeURIComponent(turmaId) + "&token=" + encodeURIComponent(token);
    }

    function construirUrlHttpsProfessor(token) {
        const urlBase = window.CONFIGURACAO_ENSALAMENTO.urlBaseApi.replace(/\/$/, "");
        return urlBase + "/api/calendario/professor.ics?token=" + encodeURIComponent(token);
    }

    function paraWebcal(urlHttps) {
        return urlHttps.replace(/^https?:\/\//, "webcal://");
    }

    function paraLinkGoogleCalendar(urlHttps) {
        return "https://calendar.google.com/calendar/render?cid=" + encodeURIComponent(paraWebcal(urlHttps));
    }

    async function montarLinksParaAluno(turmaId) {
        const resultado = await ClienteApi.obterChaveCalendario();
        const urlHttps = construirUrlHttpsAluno(resultado.token, turmaId);

        return {
            urlDownload: urlHttps,
            urlAppleCalendar: paraWebcal(urlHttps),
            urlGoogleCalendar: paraLinkGoogleCalendar(urlHttps)
        };
    }

    async function montarLinksParaProfessor() {
        const resultado = await ClienteApi.obterChaveCalendario();
        const urlHttps = construirUrlHttpsProfessor(resultado.token);

        return {
            urlDownload: urlHttps,
            urlAppleCalendar: paraWebcal(urlHttps),
            urlGoogleCalendar: paraLinkGoogleCalendar(urlHttps)
        };
    }

    return {
        montarLinksParaAluno: montarLinksParaAluno,
        montarLinksParaProfessor: montarLinksParaProfessor
    };
})();