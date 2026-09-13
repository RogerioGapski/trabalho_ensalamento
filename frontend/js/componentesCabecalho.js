function montarCabecalho(elementoAlvoId, nomeUsuario, papelUsuario) {
    const elementoAlvo = document.getElementById(elementoAlvoId);
    if (!elementoAlvo) {
        return;
    }

    const rotulosPapel = {
        aluno: "Aluno",
        prof: "Professor",
        coord: "Coordenador",
        admin: "Administrador"
    };

    elementoAlvo.innerHTML =
        '<a class="cabecalho__marca" href="index.html">' +
        '<span aria-hidden="true">🏫</span> Ensalamento' +
        "</a>" +
        '<div class="cabecalho__usuario">' +
        "<span>" + escaparHtml(nomeUsuario) + " · " + escaparHtml(rotulosPapel[papelUsuario] || papelUsuario) + "</span>" +
        '<button class="botao" id="botao-sair" type="button">Sair</button>' +
        "</div>";

    const botaoSair = document.getElementById("botao-sair");
    if (botaoSair) {
        botaoSair.addEventListener("click", function () {
            Autenticacao.encerrarSessaoERedirecionar();
        });
    }
}

function escaparHtml(textoOriginal) {
    const elementoTemporario = document.createElement("div");
    elementoTemporario.textContent = textoOriginal;
    return elementoTemporario.innerHTML;
}