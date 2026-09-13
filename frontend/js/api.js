const ClienteApi = (function () {
    function obterUrlBase() {
        return window.CONFIGURACAO_ENSALAMENTO.urlBaseApi.replace(/\/$/, "");
    }

    async function requisitar(caminho, opcoes) {
        const opcoesFinais = Object.assign(
            {
                credentials: "include",
                headers: { "Content-Type": "application/json" }
            },
            opcoes || {}
        );

        const resposta = await fetch(obterUrlBase() + caminho, opcoesFinais);
        const textoResposta = await resposta.text();
        let corpoJson = null;

        if (textoResposta.length > 0) {
            try {
                corpoJson = JSON.parse(textoResposta);
            } catch (erroDeParse) {
                corpoJson = null;
            }
        }

        if (!resposta.ok) {
            const mensagemErro = corpoJson && corpoJson.erro ? corpoJson.erro : "Falha na comunicacao com o servidor";
            const erro = new Error(mensagemErro);
            erro.status = resposta.status;
            erro.corpo = corpoJson;
            throw erro;
        }

        return corpoJson;
    }

    return {
        obterUsuarioAtual: function () {
            return requisitar("/api/auth/me", { method: "GET" });
        },
        autenticarComGoogle: function (idToken) {
            return requisitar("/api/auth/google", {
                method: "POST",
                body: JSON.stringify({ id_token: idToken })
            });
        },
        autenticarComMicrosoft: function (idToken) {
            return requisitar("/api/auth/microsoft", {
                method: "POST",
                body: JSON.stringify({ id_token: idToken })
            });
        },
        encerrarSessao: function () {
            return requisitar("/api/auth/logout", { method: "POST" });
        },
        buscarPeriodoLetivoAtual: function () {
            return requisitar("/api/periodos/atual", { method: "GET" });
        },
        buscarTurmas: function (textoBusca) {
            const parametro = encodeURIComponent(textoBusca || "");
            return requisitar("/api/turmas/busca?q=" + parametro, { method: "GET" });
        },
        buscarAgendaAluno: function (turmaId, data) {
            return requisitar("/api/agenda/aluno?turma_id=" + encodeURIComponent(turmaId) + "&data=" + encodeURIComponent(data), {
                method: "GET"
            });
        },
        buscarAgendaProfessor: function (data) {
            return requisitar("/api/agenda/professor?data=" + encodeURIComponent(data), { method: "GET" });
        },
        buscarDetalhesSala: function (salaId) {
            return requisitar("/api/salas/" + encodeURIComponent(salaId), { method: "GET" });
        }
    };
})();