(function () {
    let usuarioLogado = null;
    let conteudoCsvAtual = null;
    let tipoAtual = null;
    let ultimoRelatorio = null;

    function lerArquivoComoTexto(arquivo) {
        return new Promise(function (resolver, rejeitar) {
            const leitor = new FileReader();
            leitor.onload = function () {
                resolver(leitor.result);
            };
            leitor.onerror = function () {
                rejeitar(new Error("Não foi possível ler o arquivo selecionado."));
            };
            leitor.readAsText(arquivo, "UTF-8");
        });
    }

    function renderizarResumo(relatorio) {
        document.getElementById("cartao-resumo").hidden = false;

        const grade = document.getElementById("grade-resumo-importacao");
        grade.innerHTML =
            construirCartaoResumo("Total de linhas", relatorio.totalLinhas, "") +
            construirCartaoResumo("Válidas", relatorio.validos, "resumo-importacao__item--sucesso") +
            construirCartaoResumo("Avisos", relatorio.avisos.length, "resumo-importacao__item--alerta") +
            construirCartaoResumo("Erros", relatorio.erros.length, "resumo-importacao__item--erro") +
            construirCartaoResumo("Duplicidades", relatorio.duplicidades.length, "resumo-importacao__item--alerta");

        const areaRelatorio = document.getElementById("area-relatorio-erros");
        const problemas = [].concat(
            relatorio.erros.map(function (item) { return Object.assign({ categoria: "Erro" }, item); }),
            relatorio.duplicidades.map(function (item) { return Object.assign({ categoria: "Duplicidade" }, item); }),
            relatorio.avisos.map(function (item) { return Object.assign({ categoria: "Aviso" }, item); })
        );

        if (problemas.length === 0) {
            areaRelatorio.innerHTML = '<p class="mensagem-estado">Nenhum problema encontrado. Pronto para confirmar.</p>';
            document.getElementById("botao-baixar-relatorio").hidden = true;
            return;
        }

        document.getElementById("botao-baixar-relatorio").hidden = false;

        let html = '<table class="tabela-relatorio"><thead><tr><th>Linha</th><th>Campo</th><th>Categoria</th><th>Motivo</th></tr></thead><tbody>';
        problemas.forEach(function (item) {
            html +=
                "<tr><td>" + escaparHtml(String(item.linha)) + "</td>" +
                "<td>" + escaparHtml(item.campo || "—") + "</td>" +
                "<td>" + escaparHtml(item.categoria) + "</td>" +
                "<td>" + escaparHtml(item.mensagem) + "</td></tr>";
        });
        html += "</tbody></table>";

        areaRelatorio.innerHTML = html;
    }

    function construirCartaoResumo(rotulo, valor, classeExtra) {
        return (
            '<div class="resumo-importacao__item ' + classeExtra + '">' +
            '<span class="resumo-importacao__valor">' + escaparHtml(String(valor)) + "</span>" +
            '<span class="resumo-importacao__rotulo">' + escaparHtml(rotulo) + "</span>" +
            "</div>"
        );
    }

    function gerarCsvDoRelatorio(relatorio) {
        const linhas = ["linha,campo,categoria,motivo"];

        function adicionarLinhas(lista, categoria) {
            lista.forEach(function (item) {
                const camposEscapados = [item.linha, item.campo || "", categoria, item.mensagem].map(function (valor) {
                    const textoValor = String(valor).replace(/"/g, '""');
                    return '"' + textoValor + '"';
                });
                linhas.push(camposEscapados.join(","));
            });
        }

        adicionarLinhas(relatorio.erros, "Erro");
        adicionarLinhas(relatorio.duplicidades, "Duplicidade");
        adicionarLinhas(relatorio.avisos, "Aviso");

        return linhas.join("\n");
    }

    function baixarTextoComoArquivo(nomeArquivo, conteudo) {
        const blob = new Blob([conteudo], { type: "text/csv;charset=utf-8;" });
        const url = URL.createObjectURL(blob);
        const link = document.createElement("a");
        link.href = url;
        link.download = nomeArquivo;
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        URL.revokeObjectURL(url);
    }

    async function validarArquivo() {
        const botaoValidar = document.getElementById("botao-validar");
        botaoValidar.disabled = true;
        botaoValidar.textContent = "Validando...";

        try {
            ultimoRelatorio = await ClienteApi.validarImportacaoCsv(tipoAtual, conteudoCsvAtual);
            renderizarResumo(ultimoRelatorio);
        } catch (erro) {
            document.getElementById("cartao-resumo").hidden = false;
            document.getElementById("grade-resumo-importacao").innerHTML = "";
            document.getElementById("area-relatorio-erros").innerHTML =
                '<p class="mensagem-erro">' + escaparHtml(erro.message) + "</p>";
        } finally {
            botaoValidar.disabled = false;
            botaoValidar.textContent = "Validar arquivo";
        }
    }

    async function confirmarImportacao() {
        const botaoConfirmar = document.getElementById("botao-confirmar-importacao");
        const mensagemPos = document.getElementById("mensagem-pos-confirmacao");

        if (ultimoRelatorio && ultimoRelatorio.erros.length > 0) {
            const confirmou = window.confirm(
                "Existem " + ultimoRelatorio.erros.length + " erro(s) no arquivo. As linhas com erro serão ignoradas. Deseja continuar mesmo assim?"
            );
            if (!confirmou) {
                return;
            }
        }

        botaoConfirmar.disabled = true;
        botaoConfirmar.textContent = "Importando...";

        try {
            const resultado = await ClienteApi.confirmarImportacaoCsv(tipoAtual, conteudoCsvAtual);
            mensagemPos.hidden = false;
            mensagemPos.textContent = resultado.importados + " registro(s) importado(s) com sucesso.";
            document.getElementById("botao-confirmar-importacao").hidden = true;
        } catch (erro) {
            mensagemPos.hidden = false;
            mensagemPos.className = "mensagem-erro";
            mensagemPos.textContent = "Falha ao confirmar importação: " + erro.message;
        } finally {
            botaoConfirmar.disabled = false;
            botaoConfirmar.textContent = "Confirmar importação";
        }
    }

    (async function inicializar() {
        usuarioLogado = await Autenticacao.exigirAutenticacaoOuRedirecionar(["admin"]);
        if (!usuarioLogado) {
            return;
        }

        montarCabecalho("cabecalho-app", usuarioLogado.nome, usuarioLogado.papel);

        const campoArquivo = document.getElementById("campo-arquivo-csv");
        const botaoValidar = document.getElementById("botao-validar");

        campoArquivo.addEventListener("change", async function () {
            const arquivoSelecionado = campoArquivo.files[0];
            if (!arquivoSelecionado) {
                botaoValidar.disabled = true;
                return;
            }

            try {
                conteudoCsvAtual = await lerArquivoComoTexto(arquivoSelecionado);
                botaoValidar.disabled = false;
            } catch (erro) {
                window.alert(erro.message);
                botaoValidar.disabled = true;
            }
        });

        botaoValidar.addEventListener("click", function () {
            tipoAtual = document.getElementById("campo-tipo-importacao").value;
            validarArquivo();
        });

        document.getElementById("botao-confirmar-importacao").addEventListener("click", confirmarImportacao);

        document.getElementById("botao-baixar-relatorio").addEventListener("click", function () {
            if (ultimoRelatorio) {
                baixarTextoComoArquivo("relatorio-importacao-" + tipoAtual + ".csv", gerarCsvDoRelatorio(ultimoRelatorio));
            }
        });
    })();
})();