#ifndef ENSALAMENTO_TESTES_FRAMEWORK_HPP
#define ENSALAMENTO_TESTES_FRAMEWORK_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace testes {

struct FalhaAsserto : public std::runtime_error {
    explicit FalhaAsserto(const std::string& mensagem) : std::runtime_error(mensagem) {}
};

struct CasoDeTeste {
    std::string nome;
    std::function<void()> funcao;
};

class RegistroDeTestes {
public:
    static RegistroDeTestes& instancia() {
        static RegistroDeTestes unica;
        return unica;
    }

    void registrar(const std::string& nome, std::function<void()> funcao) {
        casos.push_back(CasoDeTeste{ nome, funcao });
    }

    int executarTodos() {
        int totalExecutados = 0;
        int totalFalhas = 0;

        for (const auto& caso : casos) {
            ++totalExecutados;
            try {
                caso.funcao();
                std::cout << "[OK]    " << caso.nome << std::endl;
            } catch (const FalhaAsserto& falha) {
                ++totalFalhas;
                std::cout << "[FALHA] " << caso.nome << " -> " << falha.what() << std::endl;
            } catch (const std::exception& excecao) {
                ++totalFalhas;
                std::cout << "[ERRO]  " << caso.nome << " -> excecao inesperada: " << excecao.what() << std::endl;
            } catch (...) {
                ++totalFalhas;
                std::cout << "[ERRO]  " << caso.nome << " -> excecao desconhecida" << std::endl;
            }
        }

        std::cout << std::endl;
        std::cout << totalExecutados << " teste(s) executado(s), " << totalFalhas << " falha(s)." << std::endl;

        return totalFalhas == 0 ? 0 : 1;
    }

private:
    std::vector<CasoDeTeste> casos;
};

class RegistradorAutomatico {
public:
    RegistradorAutomatico(const std::string& nome, std::function<void()> funcao) {
        RegistroDeTestes::instancia().registrar(nome, funcao);
    }
};

}

#define TESTE_CONCAT_INTERNO(a, b) a##b
#define TESTE_CONCAT(a, b) TESTE_CONCAT_INTERNO(a, b)

#define TESTE(nome) \
    void nome(); \
    static testes::RegistradorAutomatico TESTE_CONCAT(registrador_, nome)(#nome, nome); \
    void nome()

#define ASSERT_VERDADEIRO(condicao) \
    do { \
        if (!(condicao)) { \
            std::ostringstream mensagem; \
            mensagem << "esperava que fosse verdadeiro: " #condicao " (" << __FILE__ << ":" << __LINE__ << ")"; \
            throw testes::FalhaAsserto(mensagem.str()); \
        } \
    } while (false)

#define ASSERT_FALSO(condicao) \
    do { \
        if (condicao) { \
            std::ostringstream mensagem; \
            mensagem << "esperava que fosse falso: " #condicao " (" << __FILE__ << ":" << __LINE__ << ")"; \
            throw testes::FalhaAsserto(mensagem.str()); \
        } \
    } while (false)

#define ASSERT_IGUAL(esperado, obtido) \
    do { \
        if (!((esperado) == (obtido))) { \
            std::ostringstream mensagem; \
            mensagem << "esperado '" << (esperado) << "' mas obteve '" << (obtido) << "' (" << __FILE__ << ":" << __LINE__ << ")"; \
            throw testes::FalhaAsserto(mensagem.str()); \
        } \
    } while (false)

#define ASSERT_DIFERENTE(esperado, obtido) \
    do { \
        if ((esperado) == (obtido)) { \
            std::ostringstream mensagem; \
            mensagem << "nao esperava que fossem iguais: '" << (esperado) << "' (" << __FILE__ << ":" << __LINE__ << ")"; \
            throw testes::FalhaAsserto(mensagem.str()); \
        } \
    } while (false)

#endif