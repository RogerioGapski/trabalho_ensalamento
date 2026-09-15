#include "calendario/GerenciadorChaveCalendario.hpp"
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace ensalamento_api {

std::string GerenciadorChaveCalendario::gerarTokenAleatorio() {
    unsigned char bytesAleatorios[24];
    if (RAND_bytes(bytesAleatorios, sizeof(bytesAleatorios)) != 1) {
        throw std::runtime_error("Falha ao gerar token de calendario");
    }

    std::ostringstream fluxoHexadecimal;
    for (unsigned char byte : bytesAleatorios) {
        fluxoHexadecimal << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    return fluxoHexadecimal.str();
}

std::string GerenciadorChaveCalendario::obterOuCriarToken(pqxx::connection& conexao, const std::string& usuarioId) {
    pqxx::work transacao(conexao);

    pqxx::result linhasExistentes = transacao.exec_params(
        "SELECT token FROM chaves_calendario WHERE usuario_id = $1",
        usuarioId
    );

    if (!linhasExistentes.empty()) {
        std::string tokenExistente = linhasExistentes[0][0].as<std::string>();
        transacao.commit();
        return tokenExistente;
    }

    std::string novoToken = gerarTokenAleatorio();

    transacao.exec_params(
        "INSERT INTO chaves_calendario (usuario_id, token) VALUES ($1, $2)",
        usuarioId,
        novoToken
    );

    transacao.commit();

    return novoToken;
}

std::optional<UsuarioPorChaveCalendario> GerenciadorChaveCalendario::buscarUsuarioPorToken(pqxx::connection& conexao, const std::string& token) {
    pqxx::work transacao(conexao);

    pqxx::result linhas = transacao.exec_params(
        "SELECT u.id::text, u.papel::text "
        "FROM chaves_calendario cc "
        "JOIN usuarios u ON u.id = cc.usuario_id "
        "WHERE cc.token = $1 AND u.ativo = true",
        token
    );

    transacao.commit();

    if (linhas.empty()) {
        return std::nullopt;
    }

    UsuarioPorChaveCalendario usuario;
    usuario.id = linhas[0][0].as<std::string>();
    usuario.papel = linhas[0][1].as<std::string>();

    return usuario;
}

}