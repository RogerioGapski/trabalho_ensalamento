#include "auth/GerenciadorSessao.hpp"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>

namespace ensalamento_api {

std::string GerenciadorSessao::gerarTokenAleatorio() {
    unsigned char bytesAleatorios[32];
    if (RAND_bytes(bytesAleatorios, sizeof(bytesAleatorios)) != 1) {
        throw std::runtime_error("Falha ao gerar bytes aleatorios para sessao");
    }

    std::ostringstream fluxoHexadecimal;
    for (unsigned char byte : bytesAleatorios) {
        fluxoHexadecimal << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    return fluxoHexadecimal.str();
}

std::string GerenciadorSessao::calcularHashSha256(const std::string& texto) {
    unsigned char resumo[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(texto.data()), texto.size(), resumo);

    std::ostringstream fluxoHexadecimal;
    for (unsigned char byte : resumo) {
        fluxoHexadecimal << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    return fluxoHexadecimal.str();
}

SessaoCriada GerenciadorSessao::criarSessao(pqxx::connection& conexao, const std::string& usuarioId) {
    std::string tokenBruto = gerarTokenAleatorio();
    std::string tokenHash = calcularHashSha256(tokenBruto);

    int horasValidade = 12;
    const char* horasAmbiente = std::getenv("SESSION_TTL_HORAS");
    if (horasAmbiente != nullptr) {
        horasValidade = std::atoi(horasAmbiente);
    }

    pqxx::work transacao(conexao);

    pqxx::result resultado = transacao.exec_params(
        "INSERT INTO sessoes (usuario_id, token_hash, expira_em) "
        "VALUES ($1, $2, now() + ($3 || ' hours')::interval) "
        "RETURNING expira_em::text",
        usuarioId,
        tokenHash,
        horasValidade
    );

    transacao.commit();

    SessaoCriada sessaoCriada;
    sessaoCriada.tokenBruto = tokenBruto;
    sessaoCriada.expiraEmIso8601 = resultado[0][0].as<std::string>();

    return sessaoCriada;
}

std::optional<UsuarioDaSessao> GerenciadorSessao::validarSessao(pqxx::connection& conexao, const std::string& tokenBruto) {
    std::string tokenHash = calcularHashSha256(tokenBruto);

    pqxx::work transacao(conexao);

    pqxx::result linhas = transacao.exec_params(
        "SELECT u.id::text, u.nome, u.email, u.papel::text "
        "FROM sessoes s "
        "JOIN usuarios u ON u.id = s.usuario_id "
        "WHERE s.token_hash = $1 "
        "AND s.expira_em > now() "
        "AND s.revogada_em IS NULL "
        "AND u.ativo = true",
        tokenHash
    );

    transacao.commit();

    if (linhas.empty()) {
        return std::nullopt;
    }

    UsuarioDaSessao usuario;
    usuario.id = linhas[0][0].as<std::string>();
    usuario.nome = linhas[0][1].as<std::string>();
    usuario.email = linhas[0][2].as<std::string>();
    usuario.papel = linhas[0][3].as<std::string>();

    return usuario;
}

void GerenciadorSessao::revogarSessao(pqxx::connection& conexao, const std::string& tokenBruto) {
    std::string tokenHash = calcularHashSha256(tokenBruto);

    pqxx::work transacao(conexao);

    transacao.exec_params(
        "UPDATE sessoes SET revogada_em = now() WHERE token_hash = $1",
        tokenHash
    );

    transacao.commit();
}

}