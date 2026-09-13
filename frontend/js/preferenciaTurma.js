const PreferenciaTurma = (function () {
    const PREFIXO_CHAVE = "ensalamento:preferenciaTurma:";

    function construirChave(usuarioId, periodoLetivoId) {
        return PREFIXO_CHAVE + usuarioId + ":" + periodoLetivoId;
    }

    function salvar(usuarioId, periodoLetivoId, turmaId, turmaDescricao) {
        const chave = construirChave(usuarioId, periodoLetivoId);
        const valor = JSON.stringify({
            turmaId: turmaId,
            turmaDescricao: turmaDescricao,
            periodoLetivoId: periodoLetivoId,
            salvoEm: new Date().toISOString()
        });

        try {
            window.localStorage.setItem(chave, valor);
        } catch (erroDeArmazenamento) {
            /* localStorage indisponivel (ex.: navegacao privada) — segue sem preferencia salva */
        }
    }

    function obter(usuarioId, periodoLetivoId) {
        const chave = construirChave(usuarioId, periodoLetivoId);
        let valorBruto = null;

        try {
            valorBruto = window.localStorage.getItem(chave);
        } catch (erroDeArmazenamento) {
            return null;
        }

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
        try {
            window.localStorage.removeItem(chave);
        } catch (erroDeArmazenamento) {
            /* nada a fazer */
        }
    }

    function limparPreferenciasDeOutrosPeriodos(usuarioId, periodoLetivoIdAtual) {
        const prefixoDoUsuario = PREFIXO_CHAVE + usuarioId + ":";
        const chaveAtual = construirChave(usuarioId, periodoLetivoIdAtual);
        const chavesParaRemover = [];

        try {
            for (let indice = 0; indice < window.localStorage.length; indice++) {
                const chave = window.localStorage.key(indice);
                if (chave && chave.indexOf(prefixoDoUsuario) === 0 && chave !== chaveAtual) {
                    chavesParaRemover.push(chave);
                }
            }
            chavesParaRemover.forEach(function (chave) {
                window.localStorage.removeItem(chave);
            });
        } catch (erroDeArmazenamento) {
            /* localStorage indisponivel — nada a limpar */
        }
    }

    return {
        salvar: salvar,
        obter: obter,
        remover: remover,
        limparPreferenciasDeOutrosPeriodos: limparPreferenciasDeOutrosPeriodos
    };
})();