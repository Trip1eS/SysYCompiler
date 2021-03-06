#pragma once
#include <string>

enum class TokenCategory {
    KEYWORD,
    OPERATOR,
    INT,
    ID,
    DELIMITER,
};

enum class TokenType {
#define TOKEN(item, value, category) item,
#include "Tokens.def"
#undef TOKEN
};

constexpr std::string_view getTokenName(TokenType type) noexcept {
    switch (type) {
#define TOKEN(type, value, category) \
    case TokenType::type:            \
        return #type;
#include "Tokens.def"
#undef TOKEN
    }
    return "undefined token";
}

constexpr std::string_view getTokenValue(TokenType type) noexcept {
    switch (type) {
#define TOKEN(type, value, category) \
    case TokenType::type:            \
        return value;
#include "Tokens.def"
#undef TOKEN
    }
    return "undefined token";
}

constexpr TokenCategory getTokenCategory(TokenType type) noexcept {
    switch (type) {
#define TOKEN(type, value, category) \
    case TokenType::type:            \
        return TokenCategory::category;
#include "Tokens.def"
#undef TOKEN
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