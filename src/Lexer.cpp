#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <memory>

Lexer::Lexer() {
    addDifinition<StringMatcher>(TokenType::LPARENT, "(");
    addDifinition<StringMatcher>(TokenType::RPARENT, ")");
    addDifinition<StringMatcher>(TokenType::LSQBRA, "[");
    addDifinition<StringMatcher>(TokenType::RSQBRA, "]");
    addDifinition<StringMatcher>(TokenType::LBRACE, "{");
    addDifinition<StringMatcher>(TokenType::RBRACE, "}");
    addDifinition<StringMatcher>(TokenType::SEMICN, ";");
    addDifinition<StringMatcher>(TokenType::COMMA, ",");
    addDifinition<StringMatcher>(TokenType::ASSIGN, "=");
    addDifinition<StringMatcher>(TokenType::PLUS, "+");
    addDifinition<StringMatcher>(TokenType::SUB, "-");
    addDifinition<StringMatcher>(TokenType::NOT, "!");
    addDifinition<StringMatcher>(TokenType::MUL, "*");
    addDifinition<StringMatcher>(TokenType::DIV, "/");
    addDifinition<StringMatcher>(TokenType::MOD, "%");
    addDifinition<StringMatcher>(TokenType::LESS, "<");
    addDifinition<StringMatcher>(TokenType::GREATER, ">");
    addDifinition<StringMatcher>(TokenType::LESSEQ, "<=");
    addDifinition<StringMatcher>(TokenType::GRETEREQ, ">=");
    addDifinition<StringMatcher>(TokenType::EQUAL, "==");
    addDifinition<StringMatcher>(TokenType::NEQUAL, "!=");
    addDifinition<StringMatcher>(TokenType::LOGICAND, "&&");
    addDifinition<StringMatcher>(TokenType::LOGICOR, "||");
    addDifinition<StringMatcher>(TokenType::INTTK, "int");
    addDifinition<StringMatcher>(TokenType::VOID, "void");
    addDifinition<StringMatcher>(TokenType::CONST, "const");
    addDifinition<StringMatcher>(TokenType::IF, "if");
    addDifinition<StringMatcher>(TokenType::ELSE, "else");
    addDifinition<StringMatcher>(TokenType::WHILE, "while");
    addDifinition<StringMatcher>(TokenType::BREAK, "break");
    addDifinition<StringMatcher>(TokenType::CONT, "continue");
    addDifinition<StringMatcher>(TokenType::RETURN, "return");
    addDifinition<IdMatcher>(TokenType::ID);
    addDifinition<IntConstMatcher>(TokenType::INTCON);
}

void Lexer::addDifinition(MatcherPtr matcher) {
    _difinitions.push_back(std::move(matcher));
}

std::tuple<std::vector<Token>, std::vector<std::string>> Lexer::lex(const std::string& filePath) {
    std::vector<Token> tokens;
    std::vector<std::string> errors;
    loadFile(filePath);
    std::optional<Token> token;

    while (true) {
        try {
            if ((token = getNextToken())) {
                tokens.push_back(*token);
            } else {
                break;  // finish Lexical analysis
            }
        } catch (const LexingError& e) {
            errors.push_back(e.what());
            nextLine();
        }
    }

    return {tokens, errors};
}

void Lexer::loadFile(const std::string& filePath) {
    _inputStream = std::ifstream(filePath, std::ios::in);
    _lineno = 1;
}

std::optional<Token> Lexer::getNextToken() {
    // _lastError = std::nullopt;
    for (auto& matcher : _difinitions) matcher->reset();
    char _curChar = _inputStream.get();
    while (_curChar == ' ' || _curChar == '\n' || _curChar == '\t') {
        if (_curChar == '\n') _lineno++;
        _curChar = _inputStream.get();
    }

    while (_curChar != EOF) {
        bool hasReading = false;
        bool hasAccept = false;
        bool allReject = true;
        bool hasError = false;
        for (auto& matcher : _difinitions) {
            if (matcher->getCurrentStatus() == MatchStatus::Reject) continue;
            matcher->read(_curChar, _inputStream.peek());
            auto status = matcher->getCurrentStatus();
            if (status != MatchStatus::Reject) allReject = false;
            if (status == MatchStatus::Reading) hasReading = true;
            if (status == MatchStatus::Accept) hasAccept = true;
            if (status == MatchStatus::Error) {
                hasError = true;
            }
        }

        if (hasError) {
            auto& matcher = *std::find_if(
                _difinitions.begin(), _difinitions.end(),
                [](auto& m) { return m->getCurrentStatus() == MatchStatus::Error; });
            char buf[256];
            if (auto onError = matcher->getOnError()) {
                auto errMsg = (*onError)();
                snprintf(buf, sizeof(buf), "Error type A at line %d : %s", _lineno, errMsg.c_str());
            } else {
                // unhandled error
                snprintf(buf, sizeof(buf), "Error type A at line %d : Invaild character \"%c\"", _lineno, _inputStream.peek());
            }
            throw LexingError(buf);
        }

        if (allReject) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Error type A at line %d : Invaild character \"%c\"", _lineno, _curChar);
            throw LexingError(buf);
        }

        if (hasAccept && !hasReading) {
            auto& matcher = *std::find_if(
                _difinitions.begin(), _difinitions.end(),
                [](auto& m) { return m->getCurrentStatus() == MatchStatus::Accept; });
            return Token(matcher->getTokenType(), matcher->getValue(), _lineno);
        }
        _curChar = _inputStream.get();
    }
    return std::nullopt;
}

void Lexer::nextLine() {
    while (_curChar != '\n') _curChar = _inputStream.get();
    _lineno++;
}