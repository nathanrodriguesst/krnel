#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

// Mapeamento de strings para tokens
std::unordered_map<std::string, std::string> keywords = {
        {"def:", "DEFINICAO_VARIAVEL"},
        {"super:def", "DEFINICAO_FUNCAO"},
        {"yeet", "RETORNO"},
        {"i:", "CONDICIONAL_SE"},
        {"e:", "CONDICIONAL_SENAO"},
        {"ie:", "CONDICIONAL_SENAOSE"},
        {"w:", "LACO_ENQUANTO"},
        {"f:", "LACO_PARA"},
        {"read:", "ENTRADA_DADOS"},
        {"put::", "SAIDA_DADOS"},
        {"int", "TIPAGEM"},
        {"float", "TIPAGEM"},
        {"boolean", "TIPAGEM"},
        {"string", "TIPAGEM"},
        {"char", "TIPAGEM"},
        {"void", "TIPAGEM"}
};

std::unordered_map<std::string, std::string> operators = {
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
};

std::unordered_map<std::string, std::string> symbols = {
        {"@:!", "INICIO_PROGRAMA"},
        {"%:!", "FIM_PROGRAMA"},
        {"{", "INICIO_BLOCO"},
        {"}", "FIM_BLOCO"},
        {";", "FIM_COMANDO"},
        {"*>>*", "ATRIBUIR"},
};

// Função para verificar se uma string é um identificador válido
bool isIdentifier(const std::string& str) {
    if (str.empty() || !std::isalpha(str[0]))
        return false;
    for (char c : str) {
        if (!std::isalnum(c) && c != '_')
            return false;
    }

    return true;
}

// Verificar se uma string é um número inteiro ou float válido
bool isNumber(const std::string& str) {
    if (str.empty())
        return false;

    bool hasDecimalPoint = false;
    size_t start = 0;

    // Allow for an initial + or - sign
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
        if (str.size() == 1)  // String with only + or - is not a number
            return false;
    }

    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimalPoint)  // More than one decimal point is not allowed
                return false;
            hasDecimalPoint = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }

    // Ensure the string is not just a decimal point
    if (str.size() == 1 && str == ".")
        return false;

    return true;
}

// Função para separar símbolos dos tokens compostos
std::string separateSymbols(const std::string& line) {
    std::unordered_set<char> specialPunctuation = {'(', ')', '{', '}', ';', '#'};
    std::string result;

    for (char ch : line) {
        if (std::ispunct(ch) && ch != '"' && specialPunctuation.count(ch)) {
            result += ' ';
            result += ch;
            result += ' ';
        } else {
            result += ch;
        }
    }

    return result;
}

void analyzeLine(const std::string& line, std::ofstream& outfile) {
    std::string separatedLine = separateSymbols(line);
    std::stringstream ss(separatedLine);
    std::string token;

    while (ss >> token) {
        // Verificar se é um literal
        if (token.front() == '"') {
            std::string literal = token;

            while (literal.back() != '"') {
                std::string nextPart;
                ss >> nextPart;
                literal += " " + nextPart;
            }

            token = literal;
            outfile << R"({"type": "STRING_LITERAL", "value": ")" << token << R"("},)" << std::endl;
            continue;
        }

        // Verificar se o token é uma palavra reservada
        auto itKeyworkds = keywords.find(token);
        if (itKeyworkds != keywords.end()) {
            std::string tokenType = itKeyworkds->second;
            outfile << "Palavra reservada: " << token << " [" << tokenType << "]" << std::endl;
            continue;
        }

        auto itOperators = operators.find(token);
        if (itOperators != operators.end()) {
            std::string tokenType = itOperators->second;
            outfile << "Operador: " << token << " [" << tokenType << "]" << std::endl;
            continue;
        }

        auto itSymbols = symbols.find(token);
        if (itSymbols != symbols.end()) {
            std::string tokenType = itSymbols->second;
            outfile << "Simbolo: " << token << " [" << tokenType << "]" << std::endl;
            continue;
        }

        // Verificar se o token é um número
        if (isNumber(token)) {
            outfile << "Número: " << token << std::endl;
            continue;
        }

        // Verificar se o token é um identificador
        if (isIdentifier(token)) {
            outfile << "Identificador: " << token << std::endl;
            continue;
        }

        // Verificar se o token é um comentário
        if (token == "#") {
            std::string comment;
            std::getline(ss, comment);
            outfile << "Comentário:" << comment << std::endl;
            break; // Comentário engole o resto da linha
        }
    }
}

int main() {
    std::string fileName;

    std::cout << "Digite o nome do arquivo de entrada: ";
    std::cin >> fileName;

    // Abre arquivo de entrada
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de entrada.\n";
        return 1;
    }

    // Abre arquivo de saída
    std::ofstream outfile(fileName + ".krn");
    if (!outfile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de saída.\n";
        return 1;
    }

    std::cout << "[INICIANDO LEITURA DOS TOKENS]\n";
    outfile << "[\n";

    std::string line;
    while (std::getline(inputFile, line)) {
        std::cout << line << "\n";
        analyzeLine(line, outfile);
    }

    std::cout << "[LEITURA FINALIZADA]\n";
    outfile << "\n]";

    inputFile.close();
    outfile.close();
    return 0;
}