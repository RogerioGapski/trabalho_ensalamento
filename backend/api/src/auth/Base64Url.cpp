#include "auth/Base64Url.hpp"
#include <stdexcept>

namespace ensalamento_api {

static int valorCaractere(char caractere) {
    if (caractere >= 'A' && caractere <= 'Z') return caractere - 'A';
    if (caractere >= 'a' && caractere <= 'z') return caractere - 'a' + 26;
    if (caractere >= '0' && caractere <= '9') return caractere - '0' + 52;
    if (caractere == '-') return 62;
    if (caractere == '_') return 63;
    return -1;
}

std::vector<unsigned char> Base64Url::decodificar(const std::string& textoBase64Url) {
    std::string textoNormalizado = textoBase64Url;
    while (textoNormalizado.size() % 4 != 0) {
        textoNormalizado.push_back('=');
    }

    std::vector<unsigned char> resultado;
    int buffer = 0;
    int bitsAcumulados = 0;

    for (char caractere : textoNormalizado) {
        if (caractere == '=') {
            break;
        }
        int valor = valorCaractere(caractere);
        if (valor < 0) {
            throw std::runtime_error("Caractere invalido em texto base64url");
        }
        buffer = (buffer << 6) | valor;
        bitsAcumulados += 6;
        if (bitsAcumulados >= 8) {
            bitsAcumulados -= 8;
            resultado.push_back(static_cast<unsigned char>((buffer >> bitsAcumulados) & 0xFF));
        }
    }

    return resultado;
}

}