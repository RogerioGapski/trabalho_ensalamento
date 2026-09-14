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
            return requisitar("/api/auth/google", { method: "POST", body: JSON.stringify({ id_token: idToken }) });
        },
        autenticarComMicrosoft: function (idToken) {
            return requisitar("/api/auth/microsoft", { method: "POST", body: JSON.stringify({ id_token: idToken }) });
        },
        encerrarSessao: function () {
            return requisitar("/api/auth/logout", { method: "POST" });
        },
        buscarPeriodoLetivoAtual: function () {
            return requisitar("/api/periodos/atual", { method: "GET" });
        },
        buscarTurmas: function (textoBusca) {
            return requisitar("/api/turmas/busca?q=" + encodeURIComponent(textoBusca || ""), { method: "GET" });
        },
        buscarAgendaSemanalAluno: function (turmaId) {
            return requisitar("/api/agenda/aluno?turma_id=" + encodeURIComponent(turmaId), { method: "GET" });
        },
        buscarAgendaSemanalProfessor: function () {
            return requisitar("/api/agenda/professor", { method: "GET" });
        },
        buscarDetalhesSala: function (salaId) {
            return requisitar("/api/salas/" + encodeURIComponent(salaId), { method: "GET" });
        },

        buscarTurmasDoCoordenador: function () {
            return requisitar("/api/coordenador/turmas", { method: "GET" });
        },
        salvarNecessidadesTurma: function (turmaId, dados) {
            return requisitar("/api/coordenador/turmas/" + encodeURIComponent(turmaId), {
                method: "PUT",
                body: JSON.stringify(dados)
            });
        },

        validarImportacaoCsv: function (tipo, conteudoCsv) {
            return requisitar("/api/admin/importacoes/validar", {
                method: "POST",
                body: JSON.stringify({ tipo: tipo, conteudo_csv: conteudoCsv })
            });
        },
        confirmarImportacaoCsv: function (tipo, conteudoCsv) {
            return requisitar("/api/admin/importacoes/confirmar", {
                method: "POST",
                body: JSON.stringify({ tipo: tipo, conteudo_csv: conteudoCsv })
            });
        },

        gerarPropostaEnsalamento: function (periodoLetivoId, configuracaoPesos) {
            return requisitar("/api/periodos/" + encodeURIComponent(periodoLetivoId) + "/ensalamento/gerar", {
                method: "POST",
                body: JSON.stringify(configuracaoPesos || {})
            });
        },
        moverEncontroNaRevisao: function (periodoLetivoId, revisaoId, encontroId, salaId) {
            return requisitar("/api/periodos/" + encodeURIComponent(periodoLetivoId) + "/ensalamento/mover", {
                method: "POST",
                body: JSON.stringify({ revisao_id: revisaoId, encontro_id: encontroId, sala_id: salaId })
            });
        },
        publicarVersaoEnsalamento: function (periodoLetivoId, revisaoId, descricao) {
            return requisitar("/api/periodos/" + encodeURIComponent(periodoLetivoId) + "/ensalamento/publicar", {
                method: "POST",
                body: JSON.stringify({ revisao_id: revisaoId, descricao: descricao })
            });
        }
    };
})();