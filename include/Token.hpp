#pragma once
#include <string>

// X Macro technique: https://en.wikipedia.org/wiki/X_Macro
#define TOKENS  \
    X(LPARENT)  \
    X(RPARENT)  \
    X(LSQBRA)   \
    X(RSQBRA)   \
    X(LBRACE)   \
    X(RBRACE)   \
    X(SEMICN)   \
    X(COMMA)    \
    X(ASSIGN)   \
    X(PLUS)     \
    X(SUB)      \
    X(NOT)      \
    X(MUL)      \
    X(DIV)      \
    X(MOD)      \
    X(LESS)     \
    X(GREATER)  \
    X(LESSEQ)   \
    X(GRETEREQ) \
    X(EQUAL)    \
    X(NEQUAL)   \
    X(LOGICAND) \
    X(LOGICOR)  \
    X(INTTK)    \
    X(VOID)     \
    X(CONST)    \
    X(IF)       \
    X(ELSE)     \
    X(WHILE)    \
    X(BREAK)    \
    X(CONT)     \
    X(RETURN)   \
    X(ID)       \
    X(INTCON)

enum class TokenType {
#define X(item) item,
    TOKENS
#undef X
};

constexpr std::string_view getTokenName(TokenType type) noexcept {
    switch (type) {
#define X(type)           \
    case TokenType::type: \
        return #type;
        TOKENS
#undef X
    }
    return "undefined token";
}

constexpr std::string_view getTokenValue(TokenType type) noexcept {
    switch (type) {
        case TokenType::LPARENT:
            return "(";
        case TokenType::RPARENT:
            return ")";
        case TokenType::LSQBRA:
            return "[";
        case TokenType::RSQBRA:
            return "]";
        case TokenType::LBRACE:
            return "{";
        case TokenType::RBRACE:
            return "}";
        case TokenType::SEMICN:
            return ";";
        case TokenType::COMMA:
            return ",";
        case TokenType::ASSIGN:
            return "=";
        case TokenType::PLUS:
            return "+";
        case TokenType::SUB:
            return "-";
        case TokenType::NOT:
            return "!";
        case TokenType::MUL:
            return "*";
        case TokenType::DIV:
            return "/";
        case TokenType::MOD:
            return "%";
        case TokenType::LESS:
            return "<";
        case TokenType::GREATER:
            return ">";
        case TokenType::LESSEQ:
            return "<=";
        case TokenType::GRETEREQ:
            return ">=";
        case TokenType::EQUAL:
            return "==";
        case TokenType::NEQUAL:
            return "!=";
        case TokenType::LOGICAND:
            return "&&";
        case TokenType::LOGICOR:
            return "||";
        case TokenType::INTTK:
            return "int";
        case TokenType::VOID:
            return "void";
        case TokenType::CONST:
            return "const";
        case TokenType::IF:
            return "if";
        case TokenType::ELSE:
            return "else";
        case TokenType::WHILE:
            return "while";
        case TokenType::BREAK:
            return "break";
        case TokenType::CONT:
            return "continue";
        case TokenType::RETURN:
            return "return";
        default:
            return "";
    }
}

class Token {
   public:
    Token(TokenType type, std::string value, int lineno)
        : _type(type), _value(value), _lineno(lineno) {}
    std::string_view getName() const { return getTokenName(_type); }
    TokenType getType() const { return _type; }
    const std::string& getValue() const { return _value; }
    int getLineno() const { return _lineno; }

    bool is(TokenType type) const { return this->_type == type; }

   private:
    TokenType _type;
    std::string _value;
    int _lineno;
};