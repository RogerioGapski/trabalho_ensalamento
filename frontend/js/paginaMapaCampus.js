(function () {
    const NAMESPACE_SVG = "http://www.w3.org/2000/svg";

    let dadosMapa = { campi: [] };
    let campusSelecionadoId = null;
    let predioSelecionadoId = null;
    let salaSelecionadaId = null;

    const RÓTULOS_TIPO_SALA = {
        sala_aula: "Sala de aula",
        laboratorio: "Laboratório",
        auditorio: "Auditório",
        sala_reuniao: "Sala de reunião",
        quadra: "Quadra",
        outro: "Outro"
    };

    function criarElementoSvg(nomeTag, atributos) {
        const elemento = document.createElementNS(NAMESPACE_SVG, nomeTag);
        Object.keys(atributos).forEach(function (chave) {
            elemento.setAttribute(chave, atributos[chave]);
        });
        return elemento;
    }

    function obterCampusAtual() {
        return dadosMapa.campi.find(function (campus) {
            return campus.id === campusSelecionadoId;
        });
    }

    function obterPredioAtual() {
        const campus = obterCampusAtual();
        if (!campus) {
            return null;
        }
        return campus.predios.find(function (predio) {
            return predio.id === predioSelecionadoId;
        });
    }

    function renderizarAbasCampus() {
        const elementoAbas = document.getElementById("abas-campus");

        if (dadosMapa.campi.length <= 1) {
            elementoAbas.innerHTML = "";
            return;
        }

        elementoAbas.innerHTML = dadosMapa.campi.map(function (campus) {
            const selecionado = campus.id === campusSelecionadoId ? "true" : "false";
            return (
                '<button class="botao" role="tab" aria-selected="' + selecionado + '" data-campus-id="' +
                escaparHtml(campus.id) + '" type="button">' + escaparHtml(campus.nome) + "</button>"
            );
        }).join("");

        elementoAbas.querySelectorAll("[data-campus-id]").forEach(function (botao) {
            botao.addEventListener("click", function () {
                campusSelecionadoId = botao.getAttribute("data-campus-id");
                predioSelecionadoId = null;
                salaSelecionadaId = null;
                renderizarTudo();
            });
        });
    }

    function renderizarTrilhaNavegacao() {
        const elementoTrilha = document.getElementById("trilha-navegacao");
        const campus = obterCampusAtual();
        const predio = obterPredioAtual();

        let partes = [];

        if (campus) {
            partes.push('<button class="botao-trilha" data-nivel="campus">' + escaparHtml(campus.nome) + "</button>");
        }
        if (predio) {
            partes.push('<button class="botao-trilha" data-nivel="predio">' + escaparHtml(predio.nome) + "</button>");
        }
        if (salaSelecionadaId && predio) {
            const sala = predio.salas.find(function (s) { return s.id === salaSelecionadaId; });
            if (sala) {
                partes.push('<span class="botao-trilha botao-trilha--atual">' + escaparHtml(sala.nome) + "</span>");
            }
        }

        elementoTrilha.innerHTML = partes.join(' <span aria-hidden="true">›</span> ');

        const botaoCampus = elementoTrilha.querySelector("[data-nivel='campus']");
        if (botaoCampus) {
            botaoCampus.addEventListener("click", function () {
                predioSelecionadoId = null;
                salaSelecionadaId = null;
                renderizarTudo();
            });
        }

        const botaoPredio = elementoTrilha.querySelector("[data-nivel='predio']");
        if (botaoPredio) {
            botaoPredio.addEventListener("click", function () {
                salaSelecionadaId = null;
                renderizarTudo();
            });
        }
    }

    function renderizarMapaSvg() {
        const areaMapa = document.getElementById("area-mapa-svg");
        areaMapa.innerHTML = "";

        const predio = obterPredioAtual();

        if (predio) {
            areaMapa.appendChild(construirSvgAndaresDoPredio(predio));
            return;
        }

        const campus = obterCampusAtual();
        if (!campus || campus.predios.length === 0) {
            areaMapa.innerHTML = '<p class="mensagem-estado">Nenhum prédio cadastrado para este campus.</p>';
            return;
        }

        areaMapa.appendChild(construirSvgPrediosDoCampus(campus));
    }

    function construirSvgPrediosDoCampus(campus) {
        const colunas = Math.max(1, Math.ceil(Math.sqrt(campus.predios.length)));
        const larguraBloco = 180;
        const alturaBloco = 110;
        const espacamento = 20;
        const largura = colunas * (larguraBloco + espacamento) + espacamento;
        const linhas = Math.ceil(campus.predios.length / colunas);
        const altura = linhas * (alturaBloco + espacamento) + espacamento;

        const svg = criarElementoSvg("svg", {
            viewBox: "0 0 " + largura + " " + altura,
            width: "100%",
            role: "img",
            "aria-label": "Mapa esquemático de prédios do campus " + campus.nome
        });
        svg.setAttribute("class", "mapa-svg");

        campus.predios.forEach(function (predio, indice) {
            const coluna = indice % colunas;
            const linha = Math.floor(indice / colunas);
            const x = espacamento + coluna * (larguraBloco + espacamento);
            const y = espacamento + linha * (alturaBloco + espacamento);

            const grupo = criarElementoSvg("g", {
                class: "predio-grupo",
                tabindex: "0",
                role: "button",
                "aria-label": "Prédio " + predio.nome + ", " + predio.salas.length + " sala(s) cadastrada(s)"
            });

            const retangulo = criarElementoSvg("rect", {
                x: x,
                y: y,
                width: larguraBloco,
                height: alturaBloco,
                rx: 10,
                class: "predio-retangulo"
            });

            const rotulo = criarElementoSvg("text", {
                x: x + larguraBloco / 2,
                y: y + alturaBloco / 2 - 6,
                "text-anchor": "middle",
                class: "predio-rotulo"
            });
            rotulo.textContent = predio.nome;

            const subRotulo = criarElementoSvg("text", {
                x: x + larguraBloco / 2,
                y: y + alturaBloco / 2 + 16,
                "text-anchor": "middle",
                class: "predio-sub-rotulo"
            });
            subRotulo.textContent = predio.salas.length + " sala(s)";

            grupo.appendChild(retangulo);
            grupo.appendChild(rotulo);
            grupo.appendChild(subRotulo);

            function selecionarPredio() {
                predioSelecionadoId = predio.id;
                salaSelecionadaId = null;
                renderizarTudo();
            }

            grupo.addEventListener("click", selecionarPredio);
            grupo.addEventListener("keydown", function (evento) {
                if (evento.key === "Enter" || evento.key === " ") {
                    evento.preventDefault();
                    selecionarPredio();
                }
            });

            svg.appendChild(grupo);
        });

        return svg;
    }

    function construirSvgAndaresDoPredio(predio) {
        const salasPorAndar = {};
        predio.salas.forEach(function (sala) {
            if (!salasPorAndar[sala.andar]) {
                salasPorAndar[sala.andar] = [];
            }
            salasPorAndar[sala.andar].push(sala);
        });

        const andaresOrdenados = Object.keys(salasPorAndar).map(Number).sort(function (a, b) {
            return b - a;
        });

        const larguraSala = 130;
        const alturaSala = 80;
        const espacamento = 16;
        const alturaFaixaAndar = alturaSala + espacamento + 26;
        const maiorQuantidadeSalas = Math.max.apply(null, andaresOrdenados.map(function (andar) {
            return salasPorAndar[andar].length;
        }).concat([1]));

        const largura = maiorQuantidadeSalas * (larguraSala + espacamento) + espacamento;
        const altura = andaresOrdenados.length * alturaFaixaAndar + espacamento;

        const svg = criarElementoSvg("svg", {
            viewBox: "0 0 " + largura + " " + altura,
            width: "100%",
            role: "img",
            "aria-label": "Mapa esquemático de andares e salas do prédio " + predio.nome
        });
        svg.setAttribute("class", "mapa-svg");

        andaresOrdenados.forEach(function (andar, indiceAndar) {
            const yFaixa = espacamento + indiceAndar * alturaFaixaAndar;

            const rotuloAndar = criarElementoSvg("text", {
                x: espacamento,
                y: yFaixa + 14,
                class: "andar-rotulo"
            });
            rotuloAndar.textContent = andar === 0 ? "Térreo" : andar + "º andar";
            svg.appendChild(rotuloAndar);

            salasPorAndar[andar].forEach(function (sala, indiceSala) {
                const x = espacamento + indiceSala * (larguraSala + espacamento);
                const y = yFaixa + 22;

                const grupo = criarElementoSvg("g", {
                    class: "sala-grupo" + (sala.id === salaSelecionadaId ? " sala-grupo--selecionada" : ""),
                    tabindex: "0",
                    role: "button",
                    "aria-label": "Sala " + sala.nome + ", capacidade " + sala.capacidade + (sala.acessivel ? ", acessível" : "")
                });

                const retangulo = criarElementoSvg("rect", {
                    x: x,
                    y: y,
                    width: larguraSala,
                    height: alturaSala,
                    rx: 8,
                    class: "sala-retangulo"
                });

                const rotuloSala = criarElementoSvg("text", {
                    x: x + larguraSala / 2,
                    y: y + alturaSala / 2 - 6,
                    "text-anchor": "middle",
                    class: "sala-rotulo"
                });
                rotuloSala.textContent = sala.nome;

                const capacidadeTexto = criarElementoSvg("text", {
                    x: x + larguraSala / 2,
                    y: y + alturaSala / 2 + 14,
                    "text-anchor": "middle",
                    class: "sala-sub-rotulo"
                });
                capacidadeTexto.textContent = sala.capacidade + " lugares" + (sala.acessivel ? " · ♿" : "");

                grupo.appendChild(retangulo);
                grupo.appendChild(rotuloSala);
                grupo.appendChild(capacidadeTexto);

                function selecionarSala() {
                    salaSelecionadaId = sala.id;
                    renderizarTudo();
                }

                grupo.addEventListener("click", selecionarSala);
                grupo.addEventListener("keydown", function (evento) {
                    if (evento.key === "Enter" || evento.key === " ") {
                        evento.preventDefault();
                        selecionarSala();
                    }
                });

                svg.appendChild(grupo);
            });
        });

        return svg;
    }

    function renderizarDetalheSala() {
        const cartaoDetalhe = document.getElementById("cartao-detalhe-sala");
        const predio = obterPredioAtual();
        const campus = obterCampusAtual();

        if (!salaSelecionadaId || !predio || !campus) {
            cartaoDetalhe.hidden = true;
            cartaoDetalhe.innerHTML = "";
            return;
        }

        const sala = predio.salas.find(function (s) {
            return s.id === salaSelecionadaId;
        });

        if (!sala) {
            cartaoDetalhe.hidden = true;
            return;
        }

        cartaoDetalhe.hidden = false;

        const listaRecursos = sala.recursos.length > 0
            ? "<ul>" + sala.recursos.map(function (recurso) {
                return "<li>" + escaparHtml(recurso.replace(/_/g, " ")) + "</li>";
            }).join("") + "</ul>"
            : '<p class="encontro__local">Nenhum recurso especial cadastrado.</p>';

        cartaoDetalhe.innerHTML =
            "<h2 style=\"margin-top: 0;\">" + escaparHtml(sala.nome) + "</h2>" +
            '<p class="encontro__local">' +
            escaparHtml(campus.nome) + " · " + escaparHtml(predio.nome) + " · " +
            (sala.andar === 0 ? "Térreo" : sala.andar + "º andar") +
            "</p>" +
            '<div class="grade-detalhe-sala">' +
            "<div><strong>Tipo</strong><br>" + escaparHtml(RÓTULOS_TIPO_SALA[sala.tipo] || sala.tipo) + "</div>" +
            "<div><strong>Capacidade</strong><br>" + sala.capacidade + " lugares</div>" +
            "<div><strong>Acessibilidade</strong><br>" + (sala.acessivel ? "Sim" : "Não") + "</div>" +
            "</div>" +
            "<h3>Recursos disponíveis</h3>" +
            listaRecursos +
            (predio.instrucoesAcesso
                ? "<h3>Como chegar</h3><p>" + escaparHtml(predio.instrucoesAcesso) + "</p>"
                : "");
    }

    function renderizarTudo() {
        renderizarAbasCampus();
        renderizarTrilhaNavegacao();
        renderizarMapaSvg();
        renderizarDetalheSala();
    }

    function selecionarSalaPorId(salaId) {
        for (const campus of dadosMapa.campi) {
            for (const predio of campus.predios) {
                const sala = predio.salas.find(function (s) {
                    return s.id === salaId;
                });
                if (sala) {
                    campusSelecionadoId = campus.id;
                    predioSelecionadoId = predio.id;
                    salaSelecionadaId = sala.id;
                    return true;
                }
            }
        }
        return false;
    }

    async function inicializar() {
        const usuario = await Autenticacao.exigirAutenticacaoOuRedirecionar(["aluno", "prof", "coord", "admin"]);
        if (!usuario) {
            return;
        }

        montarCabecalho("cabecalho-app", usuario.nome, usuario.papel);

        try {
            dadosMapa = await ClienteApi.buscarMapaCampus();
        } catch (erro) {
            document.getElementById("area-mapa-svg").innerHTML =
                '<p class="mensagem-estado">Não foi possível carregar o mapa agora.</p>';
            return;
        }

        if (dadosMapa.campi.length === 0) {
            document.getElementById("area-mapa-svg").innerHTML =
                '<p class="mensagem-estado">Nenhum campus cadastrado.</p>';
            return;
        }

        const parametrosUrl = new URLSearchParams(window.location.search);
        const salaIdParaAbrir = parametrosUrl.get("sala_id");

        if (salaIdParaAbrir && selecionarSalaPorId(salaIdParaAbrir)) {
            renderizarTudo();
            return;
        }

        campusSelecionadoId = dadosMapa.campi[0].id;
        renderizarTudo();
    }

    inicializar();
})();