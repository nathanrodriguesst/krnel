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

int errorCount = 0;

bool isIdentifier(const std::string& token) {
    if (token.empty() || !std::isalpha(token[0]))
        return false;

    return std::all_of(token.begin(), token.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

bool isNumber(const std::string& token) {
    if (token.empty())
        return false;

    bool hasDecimalPoint = false;
    size_t start = 0;

    for (size_t i = start; i < token.size(); ++i) {
        if (token[i] == '.') {
            if (hasDecimalPoint)  // More than one decimal point is not allowed
                return false;
            hasDecimalPoint = true;
        } else if (!std::isdigit(token[i])) {
            return false;
        }
    }

    // Ensure the token is not just a decimal point
    if (token.size() == 1 && token == ".")
        return false;

    return true;
}

std::string separateSymbols(const std::string& line) {
    // Set of symbols that should be separated
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

    if (value != "%:!")
        formattedOutput.append(",").append("\n");


    return formattedOutput;
}

bool processLiteralString(std::string& token, std::stringstream& ss, int& lineCount, std::ofstream& outfile ) {
    std::string literal = token;
    bool isTerminated = false;

    while (ss) {
        if (literal.back() == '"') {
            isTerminated = true;
            break;
        }

        std::string nextPart;
        if (!(ss >> nextPart)) {
            break; // End of stream, exit the loop
        }
        literal += " " + nextPart;
    }

    if (!isTerminated) {
        std::cerr << "[ERROR: UNTERMINATED STRING LITERAL AT LINE "
                  << lineCount << "]" << std::endl;

        outfile << formatOutput("LEXICAL_ERROR", "Unterminated string literal");
        errorCount++;
    } else {
        token = literal;
        outfile << formatOutput("LITERAL_STRING", token);
    }

    return isTerminated;
}

void analyzeLine(const std::string& line, std::ofstream& outfile, int& lineCount) {
    std::string separatedLine = separateSymbols(line);
    std::stringstream ss(separatedLine);
    std::string token;

    while (ss >> token) {
        // Check if token is a literal string
        if (token.front() == '"') {
            if (processLiteralString(token, ss, lineCount, outfile)) {
                continue;
            }
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
            outfile << formatOutput("COMMENT", comment);
            break;
        }

        // Check if token is an identifier
        if (isIdentifier(token)) {
            outfile << formatOutput("IDENTIFIER", token);
            continue;
        } else {
            std::cerr << "[ERROR: UNKNOW TOKEN AT LINE " << lineCount
                      << ": " << token << "]" << std::endl;

            outfile << formatOutput("LEXICAL_ERROR", token);

            errorCount++;
        }
    }
}

int main() {
    std::string fileName;
    int lineCount = 1;

    std::cout << "Type the input file name: ";
    std::cin >> fileName;

    // Open input file
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "[ERROR OPENING INPUT FILE]" << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outfile(fileName + ".krn");
    if (!outfile.is_open()) {
        std::cerr << "[ERROR OPENING OUTPUT FILE]" << std::endl;
        return 1;
    }

    std::cout << "[READING INPUT FILE...]" << std::endl << std::endl;
    outfile << "[" << std::endl;

    std::cout << "[ERRORS]:" << std::endl;

    std::string line;
    while (std::getline(inputFile, line)) {
        analyzeLine(line, outfile, lineCount);
        lineCount++;
    }

    outfile << std::endl << "]";
    inputFile.close();
    outfile.close();

    std::cout << std::endl << "[READING FINISHED, OUTPUT FILE " << fileName << ".krn READY!]" << std::endl;
    std::cout << "[PROGRAM FINISHED WITH " << errorCount << " ERRORS!]" << std::endl;

    return 0;
}