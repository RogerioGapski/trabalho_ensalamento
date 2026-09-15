(function () {
    function formatarPercentual(valorDecimal) {
        return Math.round(valorDecimal * 100) + "%";
    }

    function corConformeTaxa(taxa) {
        if (taxa >= 0.75) return "var(--cor-sucesso)";
        if (taxa >= 0.4) return "var(--cor-primaria)";
        return "var(--cor-erro)";
    }

    function renderizarResumoGeral(metricas) {
        return (
            '<div class="cartao">' +
            '<div class="grade-resumo-importacao">' +
            '<div class="resumo-importacao__item resumo-importacao__item--sucesso">' +
            '<span class="resumo-importacao__valor">' + formatarPercentual(metricas.taxaOcupacaoGeral) + "</span>" +
            '<span class="resumo-importacao__rotulo">Taxa de ocupação geral</span></div>' +
            '<div class="resumo-importacao__item">' +
            '<span class="resumo-importacao__valor">' + metricas.totalHorasOcupadas.toFixed(1) + "h</span>" +
            '<span class="resumo-importacao__rotulo">Horas-aula ocupadas na semana</span></div>' +
            '<div class="resumo-importacao__item">' +
            '<span class="resumo-importacao__valor">' + metricas.totalHorasDisponiveis.toFixed(1) + "h</span>" +
            '<span class="resumo-importacao__rotulo">Horas-sala disponíveis na semana</span></div>' +
            '<div class="resumo-importacao__item resumo-importacao__item--erro">' +
            '<span class="resumo-importacao__valor">' + metricas.totalAssentosOciosos + "</span>" +
            '<span class="resumo-importacao__rotulo">Assentos ociosos por semana</span></div>' +
            "</div>" +
            "</div>"
        );
    }

    function renderizarGraficoOcupacaoPorSala(ocupacaoPorSala) {
        const salasOrdenadas = ocupacaoPorSala.slice().sort(function (a, b) {
            return b.taxaOcupacao - a.taxaOcupacao;
        });

        let barras = "";
        salasOrdenadas.forEach(function (sala) {
            const larguraPercentual = Math.round(sala.taxaOcupacao * 100);
            barras +=
                '<div class="barra-ocupacao">' +
                '<span class="barra-ocupacao__rotulo">' + escaparHtml(sala.salaNome) + "</span>" +
                '<div class="barra-ocupacao__trilho">' +
                '<div class="barra-ocupacao__preenchimento" style="width: ' + larguraPercentual + "%; background-color: " +
                corConformeTaxa(sala.taxaOcupacao) + ';"></div>' +
                "</div>" +
                '<span class="barra-ocupacao__valor">' + formatarPercentual(sala.taxaOcupacao) +
                ' <span class="encontro__local">(' + sala.horasOcupadas.toFixed(1) + "h de " + sala.horasDisponiveis.toFixed(1) + "h)</span></span>" +
                "</div>";
        });

        return (
            '<div class="cartao">' +
            "<h2 style=\"margin-top: 0;\">Taxa de ocupação por sala</h2>" +
            (salasOrdenadas.length === 0
                ? '<p class="mensagem-estado">Nenhuma sala com dados de ocupação.</p>'
                : '<div class="grafico-barras-ocupacao">' + barras + "</div>") +
            "</div>"
        );
    }

    function renderizarMapaCalorHorariosPico(horariosPico) {
        const diasOrdem = ["segunda", "terca", "quarta", "quinta", "sexta", "sabado"];
        const rotulosDias = { segunda: "Seg", terca: "Ter", quarta: "Qua", quinta: "Qui", sexta: "Sex", sabado: "Sáb" };
        const horasOrdem = ["07:00", "08:00", "09:00", "10:00", "11:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00", "20:00", "21:00"];

        const mapaContagem = {};
        let maiorContagem = 0;

        horariosPico.forEach(function (item) {
            const chave = item.diaSemana + "|" + item.horaInicio;
            mapaContagem[chave] = item.quantidadeEncontros;
            if (item.quantidadeEncontros > maiorContagem) {
                maiorContagem = item.quantidadeEncontros;
            }
        });

        let html = '<div class="mapa-calor-pico">';
        html += '<div class="mapa-calor-pico__celula mapa-calor-pico__cabecalho"></div>';
        diasOrdem.forEach(function (dia) {
            html += '<div class="mapa-calor-pico__celula mapa-calor-pico__cabecalho">' + rotulosDias[dia] + "</div>";
        });

        horasOrdem.forEach(function (hora) {
            html += '<div class="mapa-calor-pico__celula mapa-calor-pico__cabecalho">' + hora + "</div>";
            diasOrdem.forEach(function (dia) {
                const contagem = mapaContagem[dia + "|" + hora] || 0;
                const intensidade = maiorContagem > 0 ? contagem / maiorContagem : 0;
                const opacidade = 0.08 + intensidade * 0.85;
                html +=
                    '<div class="mapa-calor-pico__celula" style="background-color: rgba(29, 78, 216, ' + opacidade.toFixed(2) + ');" ' +
                    'title="' + rotulosDias[dia] + " " + hora + ": " + contagem + ' encontro(s)">' +
                    (contagem > 0 ? contagem : "") +
                    "</div>";
            });
        });

        html += "</div>";

        return (
            '<div class="cartao">' +
            "<h2 style=\"margin-top: 0;\">Horários de pico</h2>" +
            '<p class="encontro__local">Quanto mais escuro, maior a quantidade de encontros simultâneos naquele horário.</p>' +
            html +
            "</div>"
        );
    }

    function renderizarDesperdicioAssentos(desperdicioAssentos) {
        const listaOrdenada = desperdicioAssentos.slice().sort(function (a, b) {
            return b.assentosOciosos - a.assentosOciosos;
        }).slice(0, 15);

        if (listaOrdenada.length === 0) {
            return (
                '<div class="cartao">' +
                "<h2 style=\"margin-top: 0;\">Desperdício de assentos ociosos</h2>" +
                '<p class="mensagem-estado">Nenhum dado de desperdício disponível.</p>' +
                "</div>"
            );
        }

        let linhas = "";
        listaOrdenada.forEach(function (item) {
            linhas +=
                "<tr>" +
                "<td>" + escaparHtml(item.turmaCodigo) + "</td>" +
                "<td>" + escaparHtml(item.salaNome) + "</td>" +
                "<td>" + item.capacidade + "</td>" +
                "<td>" + item.tamanhoTurma + "</td>" +
                "<td><strong>" + item.assentosOciosos + "</strong></td>" +
                "</tr>";
        });

        return (
            '<div class="cartao">' +
            "<h2 style=\"margin-top: 0;\">Maiores desperdícios de assentos ociosos</h2>" +
            '<p class="encontro__local">Top 15 encontros com maior diferença entre capacidade da sala e tamanho da turma.</p>' +
            '<table class="tabela-relatorio">' +
            "<thead><tr><th>Turma</th><th>Sala</th><th>Capacidade</th><th>Turma</th><th>Ociosos</th></tr></thead>" +
            "<tbody>" + linhas + "</tbody>" +
            "</table>" +
            "</div>"
        );
    }

    async function carregarMetricas(periodoLetivoId) {
        const areaMetricas = document.getElementById("area-metricas");

        try {
            const metricas = await ClienteApi.buscarMetricasOcupacao(periodoLetivoId);

            areaMetricas.innerHTML =
                renderizarResumoGeral(metricas) +
                renderizarGraficoOcupacaoPorSala(metricas.ocupacaoPorSala) +
                renderizarMapaCalorHorariosPico(metricas.horariosPico) +
                renderizarDesperdicioAssentos(metricas.desperdicioAssentos);
        } catch (erro) {
            if (erro.status === 404) {
                areaMetricas.innerHTML = '<div class="cartao"><p class="mensagem-estado">Ainda não há versão publicada para calcular métricas neste período.</p></div>';
            } else {
                areaMetricas.innerHTML = '<div class="cartao"><p class="mensagem-estado">Não foi possível carregar as métricas agora.</p></div>';
            }
        }
    }

    (async function inicializar() {
        const usuarioLogado = await Autenticacao.exigirAutenticacaoOuRedirecionar(["admin"]);
        if (!usuarioLogado) {
            return;
        }

        montarCabecalho("cabecalho-app", usuarioLogado.nome, usuarioLogado.papel);

        let periodoLetivoAtual = null;
        try {
            periodoLetivoAtual = await ClienteApi.buscarPeriodoLetivoAtual();
        } catch (erro) {
            document.getElementById("area-metricas").innerHTML =
                '<div class="cartao"><p class="mensagem-estado">Não há período letivo ativo no momento.</p></div>';
            return;
        }

        await carregarMetricas(periodoLetivoAtual.id);
    })();
})();