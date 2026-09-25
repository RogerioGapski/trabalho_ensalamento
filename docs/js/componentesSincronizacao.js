const ComponentesSincronizacao = (function () {
    function renderizarBotoes(elementoAlvoId, montarLinksFn) {
        const elementoAlvo = document.getElementById(elementoAlvoId);
        if (!elementoAlvo) {
            return;
        }

        elementoAlvo.innerHTML = '<button class="botao" id="botao-abrir-sincronizacao" type="button">Sincronizar com calendário</button>';

        document.getElementById("botao-abrir-sincronizacao").addEventListener("click", async function () {
            const botao = document.getElementById("botao-abrir-sincronizacao");
            botao.disabled = true;
            botao.textContent = "Gerando links...";

            try {
                const links = await montarLinksFn();
                exibirModalLinks(links);
            } catch (erro) {
                window.alert("Não foi possível gerar os links de sincronização agora.");
            } finally {
                botao.disabled = false;
                botao.textContent = "Sincronizar com calendário";
            }
        });
    }

    function exibirModalLinks(links) {
        let sobreposicao = document.getElementById("sobreposicao-modal-calendario");

        if (!sobreposicao) {
            sobreposicao = document.createElement("div");
            sobreposicao.id = "sobreposicao-modal-calendario";
            sobreposicao.className = "sobreposicao-modal";
            document.body.appendChild(sobreposicao);
        }

        sobreposicao.hidden = false;
        sobreposicao.innerHTML =
            '<div class="modal" role="dialog" aria-modal="true" aria-labelledby="titulo-modal-calendario">' +
            '<div class="modal__cabecalho">' +
            '<h2 id="titulo-modal-calendario">Sincronizar agenda</h2>' +
            '<button class="botao" id="botao-fechar-modal-calendario" type="button" aria-label="Fechar">✕</button>' +
            "</div>" +
            '<div class="modal__corpo">' +
            '<p class="encontro__local">Assinar mantém sua agenda sempre atualizada automaticamente. Baixar gera um arquivo único (não atualiza sozinho).</p>' +
            '<a class="botao botao--primario" style="width: 100%; margin-bottom: 0.5rem;" href="' + links.urlGoogleCalendar + '" target="_blank" rel="noopener">Assinar no Google Calendar</a>' +
            '<a class="botao" style="width: 100%; margin-bottom: 0.5rem;" href="' + links.urlAppleCalendar + '">Assinar no Apple Calendar / Outlook</a>' +
            '<a class="botao" style="width: 100%;" href="' + links.urlDownload + '" download="agenda-academica.ics">Baixar arquivo .ics</a>' +
            "</div>" +
            "</div>";

        document.getElementById("botao-fechar-modal-calendario").addEventListener("click", function () {
            sobreposicao.hidden = true;
        });

        sobreposicao.addEventListener("click", function (evento) {
            if (evento.target === sobreposicao) {
                sobreposicao.hidden = true;
            }
        });
    }

    return {
        renderizarBotoes: renderizarBotoes
    };
})();