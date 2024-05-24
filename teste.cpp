#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <vector>

// Mapeamento de strings para tokens
std::unordered_map<std::string, std::string> keywords = {
    {"=-=", "IGUAL"},
    {"=!=", "DIFERENTE"},
    {">=", "MAIOR_IGUAL"},
    {"=<<=", "MENOR_IGUAL"},
    {"=||=", "OU"},
    {"*.*", "MULTIPLICAR"},
    {"*+*", "SOMAR"},
    {"*++*", "INCREMENTAR"},
    {"*--*", "DECREMENTAR"},
    {"*-*", "SUBTRAIR"},
    {"*/*", "DIVIDIR"},
    {"@:!", "INICIO_PROGRAMA"},
    {"%:!", "FIM_PROGRAMA"},
    {"{", "INICIO_BLOCO"},
    {"}", "FIM_BLOCO"},
    {";", "FIM_COMANDO"},
    {"*>>*", "ATRIBUIR"},
    {"def:", "PALAVRA_RESERVADA"},
    {"super:def", "PALAVRA_RESERVADA"},
    {"yeet", "PALAVRA_RESERVADA"},
    {"i:", "PALAVRA_RESERVADA"},
    {"e:", "PALAVRA_RESERVADA"},
    {"ie:", "PALAVRA_RESERVADA"},
    {"w:", "PALAVRA_RESERVADA"},
    {"f:", "PALAVRA_RESERVADA"},
    {"read:", "PALAVRA_RESERVADA"},
    {"put::", "PALAVRA_RESERVADA"},
    {"int", "TIPAGEM"},
    {"float", "TIPAGEM"},
    {"boolean", "TIPAGEM"},
    {"string", "TIPAGEM"},
    {"char", "TIPAGEM"}
};

// Função para verificar se uma string é um identificador válido
bool isIdentifier(const std::string& str) {
    if (!std::isalpha(str[0]))
        return false;
    for (char c : str) {
        if (!std::isalnum(c) && c != '_')
            return false;
    }
    return true;
}

// Função para verificar se uma string é um número válido
bool isNumber(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c))
            return false;
    }
    return true;
}

// Função para analisar uma linha de entrada
void analyzeLine(const std::string& line) {
    int errors = 0;
    std::stringstream ss(line);
    std::string token;
    char prevChar = ' ';

    while (ss >> token) {
        // Verificar se o token é uma palavra reservada
        auto it = keywords.find(token);
        if (it != keywords.end()) {
            std::string tokenType = it->second;
            std::cout << "Palavra reservada: " << token << " [" << tokenType << "]" << std::endl;
            continue;
        }

        // Verificar se o token é um número
        if (isNumber(token)) {
            std::cout << "Número: " << token << std::endl;
            continue;
        }

        // Verificar se o token é um identificador
        if (isIdentifier(token)) {
            std::cout << "Identificador: " << token << std::endl;
            continue;
        }

        // Verificar se o token é um símbolo
        if (token.size() == 1 && !std::isalnum(token[0])) {
            std::cout << "Símbolo: " << token << std::endl;
            continue;
        }

        // Verificar se o token é um comentário
        if (token == "#") {
            std::cout << "Comentário: ";
            std::string comment;
            std::getline(ss, comment);
            std::cout << comment << std::endl;
            break; // Comentário engole o resto da linha
        }

        // Se chegou aqui, o token é desconhecido
        std::cerr << "Erro léxico: token desconhecido -> " << token << std::endl;
        break;
    }
}

int main() {
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de entrada.\n";
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        analyzeLine(line);
    }

    inputFile.close();
    return 0;
}

