const Autenticacao = (function () {
    let usuarioAtualCache = null;

    async function carregarUsuarioAtual() {
        try {
            usuarioAtualCache = await ClienteApi.obterUsuarioAtual();
            return usuarioAtualCache;
        } catch (erro) {
            usuarioAtualCache = null;
            return null;
        }
    }

    function obterUsuarioEmCache() {
        return usuarioAtualCache;
    }

    async function tratarCredencialGoogle(resposta) {
        try {
            const usuario = await ClienteApi.autenticarComGoogle(resposta.credential);
            usuarioAtualCache = usuario;
            redirecionarConformePapel(usuario.papel);
        } catch (erro) {
            exibirErroLogin(erro.message);
        }
    }

    function redirecionarConformePapel(papel) {
        if (papel === "aluno") {
            window.location.href = "agenda-aluno.html";
        } else if (papel === "prof") {
            window.location.href = "agenda-professor.html";
        } else if (papel === "coord") {
            window.location.href = "painel-coordenador.html";
        } else if (papel === "admin") {
            window.location.href = "painel-administrador.html";
        } else {
            window.location.href = "index.html";
        }
    }

    function exibirErroLogin(mensagem) {
        const elementoErro = document.querySelector("[data-papel='mensagem-erro-login']");
        if (elementoErro) {
            elementoErro.textContent = mensagem;
            elementoErro.hidden = false;
        }
    }

    function inicializarBotaoGoogle(elementoContainerId) {
        if (!window.google || !window.google.accounts || !window.google.accounts.id) {
            window.setTimeout(function () {
                inicializarBotaoGoogle(elementoContainerId);
            }, 200);
            return;
        }

        window.google.accounts.id.initialize({
            client_id: window.CONFIGURACAO_ENSALAMENTO.googleClientId,
            callback: tratarCredencialGoogle
        });

        window.google.accounts.id.renderButton(
            document.getElementById(elementoContainerId),
            { theme: "outline", size: "large", width: 320, text: "continue_with" }
        );
    }

    async function tratarLoginMicrosoft() {
        try {
            const configuracaoMsal = {
                auth: {
                    clientId: window.CONFIGURACAO_ENSALAMENTO.microsoftClientId,
                    authority: "https://login.microsoftonline.com/common",
                    redirectUri: window.location.origin + window.location.pathname
                }
            };

            const aplicacaoMsal = new msal.PublicClientApplication(configuracaoMsal);
            await aplicacaoMsal.initialize();

            const resultadoLogin = await aplicacaoMsal.loginPopup({
                scopes: ["openid", "profile", "email"]
            });

            const idToken = resultadoLogin.idToken;
            const usuario = await ClienteApi.autenticarComMicrosoft(idToken);
            usuarioAtualCache = usuario;
            redirecionarConformePapel(usuario.papel);
        } catch (erro) {
            exibirErroLogin(erro.message);
        }
    }

    async function encerrarSessaoERedirecionar() {
        try {
            await ClienteApi.encerrarSessao();
        } catch (erro) {
            /* mesmo se falhar no servidor, seguimos limpando o estado local */
        }
        usuarioAtualCache = null;
        window.location.href = "index.html";
    }

    async function exigirAutenticacaoOuRedirecionar(papeisPermitidos) {
        const usuario = await carregarUsuarioAtual();

        if (!usuario) {
            window.location.href = "index.html";
            return null;
        }

        if (papeisPermitidos && papeisPermitidos.indexOf(usuario.papel) === -1) {
            window.location.href = "index.html";
            return null;
        }

        return usuario;
    }

    return {
        carregarUsuarioAtual: carregarUsuarioAtual,
        obterUsuarioEmCache: obterUsuarioEmCache,
        inicializarBotaoGoogle: inicializarBotaoGoogle,
        tratarLoginMicrosoft: tratarLoginMicrosoft,
        encerrarSessaoERedirecionar: encerrarSessaoERedirecionar,
        exigirAutenticacaoOuRedirecionar: exigirAutenticacaoOuRedirecionar,
        redirecionarConformePapel: redirecionarConformePapel
    };
})();