#include "auth/RepositorioUsuarios.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace ensalamento_api {

static std::string paraMinusculas(const std::string& texto) {
    std::string resultado = texto;
    std::transform(resultado.begin(), resultado.end(), resultado.begin(), [](unsigned char caractere) {
        return static_cast<char>(std::tolower(caractere));
    });
    return resultado;
}

std::optional<UsuarioAutenticavel> RepositorioUsuarios::buscarPorEmail(pqxx::connection& conexao, const std::string& email) {
    pqxx::work transacao(conexao);

    pqxx::result linhas = transacao.exec_params(
        "SELECT id::text, nome, email, papel::text, ativo FROM usuarios WHERE LOWER(email) = LOWER($1)",
        email
    );

    transacao.commit();

    if (linhas.empty()) {
        return std::nullopt;
    }

    UsuarioAutenticavel usuario;
    usuario.id = linhas[0][0].as<std::string>();
    usuario.nome = linhas[0][1].as<std::string>();
    usuario.email = linhas[0][2].as<std::string>();
    usuario.papel = linhas[0][3].as<std::string>();
    usuario.ativo = linhas[0][4].as<bool>();

    return usuario;
}

bool RepositorioUsuarios::dominioPermitido(const std::string& email) {
    const char* dominiosAmbiente = std::getenv("ALLOWED_STUDENT_DOMAINS");
    if (dominiosAmbiente == nullptr) {
        return false;
    }

    std::size_t posicaoArroba = email.find('@');
    if (posicaoArroba == std::string::npos) {
        return false;
    }

    std::string dominioDoEmail = paraMinusculas(email.substr(posicaoArroba + 1));

    std::istringstream fluxo(dominiosAmbiente);
    std::string dominioPermitidoAtual;

    while (std::getline(fluxo, dominioPermitidoAtual, ',')) {
        if (paraMinusculas(dominioPermitidoAtual) == dominioDoEmail) {
            return true;
        }
    }

    return false;
}

UsuarioAutenticavel RepositorioUsuarios::provisionarComoAluno(pqxx::connection& conexao, const std::string& email, const std::string& nome) {
    pqxx::work transacao(conexao);

    pqxx::result resultadoInsercao = transacao.exec_params(
        "INSERT INTO usuarios (nome, email, senha_hash, papel, ativo) "
        "VALUES ($1, $2, '', 'aluno', true) "
        "RETURNING id::text, nome, email, papel::text, ativo",
        nome,
        email
    );

    transacao.commit();

    UsuarioAutenticavel usuario;
    usuario.id = resultadoInsercao[0][0].as<std::string>();
    usuario.nome = resultadoInsercao[0][1].as<std::string>();
    usuario.email = resultadoInsercao[0][2].as<std::string>();
    usuario.papel = resultadoInsercao[0][3].as<std::string>();
    usuario.ativo = resultadoInsercao[0][4].as<bool>();

    return usuario;
}

}