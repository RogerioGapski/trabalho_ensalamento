const PreferenciaTurma = (function () {
    function construirChave(usuarioId, periodoLetivoId) {
        return "ensalamento:preferenciaTurma:" + usuarioId + ":" + periodoLetivoId;
    }

    function salvar(usuarioId, periodoLetivoId, turmaId, turmaDescricao) {
        const chave = construirChave(usuarioId, periodoLetivoId);
        const valor = JSON.stringify({
            turmaId: turmaId,
            turmaDescricao: turmaDescricao,
            salvoEm: new Date().toISOString()
        });
        window.localStorage.setItem(chave, valor);
    }

    function obter(usuarioId, periodoLetivoId) {
        const chave = construirChave(usuarioId, periodoLetivoId);
        const valorBruto = window.localStorage.getItem(chave);

        if (!valorBruto) {
            return null;
        }

        try {
            const valorObjeto = JSON.parse(valorBruto);
            if (!valorObjeto.turmaId) {
                return null;
            }
            return valorObjeto;
        } catch (erroDeParse) {
            window.localStorage.removeItem(chave);
            return null;
        }
    }

    function remover(usuarioId, periodoLetivoId) {
        const chave = construirChave(usuarioId, periodoLetivoId);
        window.localStorage.removeItem(chave);
    }

    return {
        salvar: salvar,
        obter: obter,
        remover: remover
    };
})();