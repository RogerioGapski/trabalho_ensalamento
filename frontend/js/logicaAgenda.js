const LogicaAgenda = (function () {
    const DIAS_SEMANA_EM_ORDEM = ["segunda", "terca", "quarta", "quinta", "sexta", "sabado", "domingo"];

    const RÓTULOS_DIAS_SEMANA = {
        segunda: "Segunda-feira",
        terca: "Terça-feira",
        quarta: "Quarta-feira",
        quinta: "Quinta-feira",
        sexta: "Sexta-feira",
        sabado: "Sábado",
        domingo: "Domingo"
    };

    function chaveDiaSemanaDeHoje(dataReferencia) {
        const mapaIndiceJsParaChave = ["domingo", "segunda", "terca", "quarta", "quinta", "sexta", "sabado"];
        return mapaIndiceJsParaChave[dataReferencia.getDay()];
    }

    function minutosDesdeMeiaNoite(dataReferencia) {
        return dataReferencia.getHours() * 60 + dataReferencia.getMinutes();
    }

    function textoHoraParaMinutos(textoHora) {
        const partes = textoHora.split(":");
        return parseInt(partes[0], 10) * 60 + parseInt(partes[1], 10);
    }

    function agruparEncontrosPorDia(encontros) {
        const grupos = {};
        DIAS_SEMANA_EM_ORDEM.forEach(function (diaChave) {
            grupos[diaChave] = [];
        });

        encontros.forEach(function (encontro) {
            if (grupos[encontro.diaSemana]) {
                grupos[encontro.diaSemana].push(encontro);
            }
        });

        DIAS_SEMANA_EM_ORDEM.forEach(function (diaChave) {
            grupos[diaChave].sort(function (a, b) {
                return textoHoraParaMinutos(a.horaInicio) - textoHoraParaMinutos(b.horaInicio);
            });
        });

        return grupos;
    }

    function calcularStatusTemporalDoDia(encontrosDoDia, dataReferencia) {
        const minutosAgora = minutosDesdeMeiaNoite(dataReferencia);
        const statusPorEncontroId = {};

        let encontroAtualId = null;
        let encontroProximoId = null;
        let menorInicioFuturo = Infinity;

        encontrosDoDia.forEach(function (encontro) {
            const inicioMinutos = textoHoraParaMinutos(encontro.horaInicio);
            const fimMinutos = textoHoraParaMinutos(encontro.horaFim);

            if (minutosAgora >= inicioMinutos && minutosAgora < fimMinutos) {
                encontroAtualId = encontro.id;
            } else if (inicioMinutos > minutosAgora && inicioMinutos < menorInicioFuturo) {
                menorInicioFuturo = inicioMinutos;
                encontroProximoId = encontro.id;
            }
        });

        encontrosDoDia.forEach(function (encontro) {
            if (encontro.id === encontroAtualId) {
                statusPorEncontroId[encontro.id] = "atual";
            } else if (encontro.id === encontroProximoId) {
                statusPorEncontroId[encontro.id] = "proxima";
            } else {
                statusPorEncontroId[encontro.id] = "nenhum";
            }
        });

        return statusPorEncontroId;
    }

    function foiAlteradaRecentemente(encontro, dataReferencia) {
        if (!encontro.atualizadoEm) {
            return false;
        }

        const dataAtualizacao = new Date(encontro.atualizadoEm);
        const horasDesdeAtualizacao = (dataReferencia.getTime() - dataAtualizacao.getTime()) / (1000 * 60 * 60);

        return horasDesdeAtualizacao >= 0 && horasDesdeAtualizacao <= 72;
    }

    function formatarDataHoraLegivel(iso) {
        const data = new Date(iso);
        return data.toLocaleString("pt-BR", {
            day: "2-digit",
            month: "2-digit",
            year: "numeric",
            hour: "2-digit",
            minute: "2-digit"
        });
    }

    return {
        DIAS_SEMANA_EM_ORDEM: DIAS_SEMANA_EM_ORDEM,
        RÓTULOS_DIAS_SEMANA: RÓTULOS_DIAS_SEMANA,
        chaveDiaSemanaDeHoje: chaveDiaSemanaDeHoje,
        agruparEncontrosPorDia: agruparEncontrosPorDia,
        calcularStatusTemporalDoDia: calcularStatusTemporalDoDia,
        foiAlteradaRecentemente: foiAlteradaRecentemente,
        formatarDataHoraLegivel: formatarDataHoraLegivel
    };
})();