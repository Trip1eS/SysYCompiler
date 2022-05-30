#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include "Logger.hpp"

Lexer::Lexer(const std::string& filePath) {
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
    addDifinition<StringMatcher>(TokenType::GREATEREQ, ">=");
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
    loadFile(filePath);
}

void Lexer::addDifinition(MatcherPtr matcher) {
    _difinitions.push_back(std::move(matcher));
}

void Lexer::lex() {
    _tokens.clear();
    _hasError = false;
    std::optional<Token> token;

    log() << "(Lexer) Start lexing...\n";
    while (true) {
        try {
            if ((token = getNextToken())) {
                _tokens.push_back(*token);
            } else {
                break;  // finish Lexical analysis
            }
        } catch (const LexingError& e) {
            err() << e.what() << "\n";
            nextLine();
            _hasError = true;
        }
    }
    if (_hasError) {
        log() << "(Lexer) lexing done with errors.\n";
    } else {
        log() << "(Lexer) lexing done successfully with " << _tokens.size() << " tokens.\n";
    }
}

void Lexer::loadFile(const std::string& filePath) {
    _inputStream = std::ifstream(filePath, std::ios::in);
    _lineno = 1;
}

void Lexer::outputTokens(const std::string& path) const {
    std::ofstream of(path);
    for (auto& token : _tokens) {
        of << token.getName() << " " << token.getValue() << "\n";
    }
}

std::optional<Token> Lexer::getNextToken() {
    for (auto& matcher : _difinitions) matcher->reset();
    char _curChar = _inputStream.get();
    while (_curChar == ' ' || _curChar == '\n' || _curChar == '\t') {
        if (_curChar == '\n') _lineno++;
        _curChar = _inputStream.get();
    }

    // Scan through the code file
    while (_curChar != EOF) {
        bool hasReading = false;  // Whether there are matchers still reading
        bool hasAccept = false;   // Whether there are matchers that has Accept status
        bool allReject = true;    // Whether every matcher rejects, meaning there's an error
        bool hasError = false;    // Whether there are matchers that encounters an error

        // Feed input characters into matchers.
        for (auto& matcher : _difinitions) {
            if (matcher->getCurrentStatus() == MatchStatus::Reject) continue;
            // use istream::peek() to get nextChar
            matcher->read(_curChar, _inputStream.peek());
            auto status = matcher->getCurrentStatus();
            if (status != MatchStatus::Reject) allReject = false;
            if (status == MatchStatus::Reading) hasReading = true;
            if (status == MatchStatus::Accept) hasAccept = true;
            if (status == MatchStatus::Error) hasError = true;
        }

        if (hasError) {
            auto& matcher = *std::find_if(
                _difinitions.begin(), _difinitions.end(),
                [](auto& m) { return m->getCurrentStatus() == MatchStatus::Error; });
            std::string msg;
            // Get the error message from Matcher if available.
            if (auto errMsg = matcher->getErrorMsg()) {
                msg = stringFormat("Error type A at line %d : %s", _lineno, errMsg->c_str());
            } else {
                msg = stringFormat("Error type A at line %d : Invaild character \"%c\"", _lineno, _inputStream.peek());
            }
            // Just throw the error, let lex() handle it.
            throw LexingError(msg);
        }

        if (allReject) {
            // allReject also means there's an error.
            std::string msg = stringFormat("Error type A at line %d : Invaild character \"%c\"", _lineno, _curChar);
            throw LexingError(msg);
        }

        if (hasAccept && !hasReading) {
            // If there's only one Matcher remaining and it says Accept, then we get a token.
            // find that matcher
            auto& matcher = *std::find_if(
                _difinitions.begin(), _difinitions.end(),
                [](auto& m) { return m->getCurrentStatus() == MatchStatus::Accept; });
            // construct the token
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