#pragma once
#include <string>
#include <optional>
#include <memory>
#include <functional>
#include "Token.hpp"

enum class MatchStatus {
    Accept,  // accept curChar but not accept nextChar
    Reject,  // already failed
    Error,   // encountered error
    Reading  // accept curChar and nextChar
};

/**
 * A simple state machine used in Lexer for checking whether the input characters match a certain pattern.
 * We use this for better extensibility and still a good performance (since there are not that much token types).
 * It reads two characters from input, current character and the next character.
 * If it can accept current character, but not the next one, it means it matches a token.
 */
class Matcher {
   public:
    Matcher(
        TokenType tokenType)
        : _tokenType(tokenType) {}
    virtual ~Matcher() {}

    /**
     * @brief Get last error message.
     *
     * @return Last error message. If there's no error message, nullopt is returned.
     */
    virtual std::optional<std::string> getErrorMsg() const { return std::nullopt; }

    MatchStatus getCurrentStatus() const { return _status; }
    MatchStatus getLastStatus() const { return _lastStatus; }
    std::string_view getName() const { return getTokenName(_tokenType); }
    virtual std::string getValue() = 0;
    TokenType getTokenType() const { return _tokenType; }

    /**
     * @brief Reset the state machine
     */
    virtual void reset() { _status = _lastStatus = MatchStatus::Reading; }

    /**
     * @brief Read current character and the next character.
     * @details If it can read curChar and it will read nextChar, the status will be Reading.
     *          It it can read curChar but it won't read nextChar, the status will be Accept.
     */
    virtual void read(char curChar, char nextChar) = 0;

   protected:
    void setStatus(MatchStatus status) { _lastStatus = _status, _status = status; }

   protected:
    TokenType _tokenType;

   private:
    MatchStatus _status;
    MatchStatus _lastStatus;
};

using MatcherPtr = std::unique_ptr<Matcher>;

/**
 * @brief A Matcher that simply check whether input characters match a string.
 */
class StringMatcher : public Matcher {
   public:
    StringMatcher(
        TokenType tokenType,
        std::string str)
        : _str(str), _index(0), Matcher(tokenType) {}
    virtual ~StringMatcher() {}

    virtual void reset() override;
    virtual void read(char curChar, char nextChar) override;
    virtual std::string getValue() override { return _str; }

   private:
    std::string _str;
    size_t _index;
};

using StringMatcherPtr = std::unique_ptr<StringMatcher>;

/**
 * @brief A Matcher for int literals. It can handle decimal, octal and hexadecimal numbers.
 */
class IntConstMatcher : public Matcher {
   public:
    enum class Type {
        Decimal,
        Octal,
        Hexadecimal
    };

   public:
    IntConstMatcher(TokenType tokenType);
    virtual ~IntConstMatcher() = default;
    virtual std::optional<std::string> getErrorMsg() const override;
    virtual void reset() override;
    virtual void read(char curChar, char nextChar) override;

    /**
     * @brief Get the integer literal (decimal).
     */
    virtual std::string getValue() override;

    /**
     * @brief Check whether it's a valid number
     */
    bool check();

   private:
    std::string _val;
    Type _type;
};

using IntConstMatcherPtr = std::unique_ptr<IntConstMatcher>;

/**
 * @brief A Matcher for identifiers.
 */
class IdMatcher : public Matcher {
   public:
    IdMatcher(TokenType tokenType)
        : Matcher(tokenType) {}
    virtual ~IdMatcher() = default;
    virtual void reset() override;
    virtual void read(char curChar, char nextChar) override;
    virtual std::string getValue() override { return _val; }

   private:
    std::string _val;
};

using IdMatcherPtr = std::unique_ptr<IdMatcher>;