#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// Mapping keywords
std::unordered_map<std::string, std::string> keywords = {
        {"def:", "VARIABLE_DEFINITION"},
        {"super:def", "FUNCTION_DEFINITION"},
        {"yeet", "RETURN"},
        {"i:", "IF_CONDITIONAL"},
        {"e:", "ELSE_CONDITIONAL"},
        {"ie:", "IFELSE_CONDITIONAL"},
        {"w:", "WHILE_LOOP"},
        {"f:", "FOR_LOOP"},
        {"read:", "DATA_INPUT"},
        {"put::", "DATA_OUTPUT"},
        {"int", "TYPE"},
        {"float", "TYPE"},
        {"boolean", "TYPE"},
        {"string", "TYPE"},
        {"char", "TYPE"},
        {"void", "TYPE"}
};

// Mapping operators
std::unordered_map<std::string, std::string> operators = {
        {"=-=", "EQUALS"},
        {"=!=", "DIFFERENT"},
        {">=", "GREATER_EQUAL"},
        {"=<<=", "LESSER_EQUAL"},
        {"=||=", "OR"},
        {"*.*", "MULTIPLY"},
        {"*+*", "SUM"},
        {"*++*", "INCREMENT"},
        {"*--*", "DECREMENT"},
        {"*-*", "SUBTRACT"},
        {"*/*", "DIVIDE"},
};

// Mapping symbols
std::unordered_map<std::string, std::string> symbols = {
        {"@:!", "PROGRAM_START"},
        {"%:!", "PROGRAM_END"},
        {"(", "LEFT_PAREN"},
        {")", "RIGHT_PAREN"},
        {"{", "BLOCK_START"},
        {"}", "BLOCK_END"},
        {":", "START_STATEMENT"},
        {";", "COMMAND_END"},
        {"*>>*", "ASSIGN"},
};

bool isIdentifier(const std::string& str) {
    if (str.empty() || !std::isalpha(str[0]))
        return false;

    return std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

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

std::string formatOutput(const std::string& type, const std::string& value) {
    std::string formattedOutput;

    formattedOutput.append(R"(  {"type": ")")
                        .append(type)
                            .append(R"(", "value": ")")
                                .append(value).append("\"}");

    if (value != "%:!") {
        formattedOutput.append(",").append("\n");
    }

    return formattedOutput;
}

void analyzeLine(const std::string& line, std::ofstream& outfile) {
    std::string separatedLine = separateSymbols(line);
    std::stringstream ss(separatedLine);
    std::string token;

    while (ss >> token) {
        // Check if token is a literal string
        if (token.front() == '"') {
            std::string literal = token;

            while (literal.back() != '"') {
                std::string nextPart;
                ss >> nextPart;
                literal += " " + nextPart;
            }

            token = literal;
            outfile << formatOutput("LITERAL_STRING", token);
            continue;
        }

        // Check if token is a keyword
        auto itKeyworkds = keywords.find(token);
        if (itKeyworkds != keywords.end()) {
            std::string tokenType = itKeyworkds->second;
            outfile << formatOutput(tokenType, token);
            continue;
        }

        // Check if token is an operator
        auto itOperators = operators.find(token);
        if (itOperators != operators.end()) {
            std::string tokenType = itOperators->second;
            outfile << formatOutput(tokenType, token);
            continue;
        }

        // Check if token is a symbol
        auto itSymbols = symbols.find(token);
        if (itSymbols != symbols.end()) {
            std::string tokenType = itSymbols->second;
            outfile << formatOutput(tokenType, token);
            continue;
        }

        // Check if token is a number
        if (isNumber(token)) {
            outfile << formatOutput("NUMBER", token);
            continue;
        }

        // Check if token is a comment
        if (token == "#") {
            std::string comment;
            std::getline(ss, comment);
            outfile << formatOutput("COMMENT", token);
            break;
        }

        // Check if token is an identifier
        if (isIdentifier(token)) {
            outfile << formatOutput("IDENTIFIER", token);
            continue;
        } else {
            outfile << formatOutput("LEXICAL_ERROR", token);
        }
    }
}

int main() {
    std::string fileName;

    std::cout << "Type the input file name: ";
    std::cin >> fileName;

    // Open input file
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file.\n";
        return 1;
    }

    // Open output file
    std::ofstream outfile(fileName + ".krn");
    if (!outfile.is_open()) {
        std::cerr << "Error opening output file.\n";
        return 1;
    }

    std::cout << "[STARTING PROGRAM...]\n";
    outfile << "[\n";

    std::string line;
    while (std::getline(inputFile, line)) {
        analyzeLine(line, outfile);
    }

    std::cout << "[READING FINISHED, OUTPUT FILE " << fileName << ".krn READY!]\n";
    outfile << "\n]";
    inputFile.close();
    outfile.close();

    return 0;
}