#pragma once
#include <string>
#include <optional>
#include <memory>
#include <functional>
#include "Token.hpp"

enum class MatchStatus {
    Accept,
    Reject,
    Error,
    Reading
};

class Matcher {
   public:
    using OnErrorFunc = std::function<std::string(void)>;

   public:
    Matcher(
        TokenType tokenType,
        std::optional<OnErrorFunc> onError = std::nullopt)
        : _tokenType(tokenType), _onError(onError) {}
    virtual ~Matcher() {}

    const auto& getOnError() const { return _onError; }
    MatchStatus getCurrentStatus() const { return _status; }
    MatchStatus getLastStatus() const { return _lastStatus; }
    std::string_view getName() const { return getTokenName(_tokenType); }
    void setStatus(MatchStatus status) { _lastStatus = _status, _status = status; }
    virtual void reset() { _status = _lastStatus = MatchStatus::Reading; }
    virtual void read(char curChar, char nextChar) = 0;
    virtual std::string getValue() = 0;
    TokenType getTokenType() const { return _tokenType; }

   protected:
    TokenType _tokenType;
    std::optional<OnErrorFunc> _onError;

   private:
    MatchStatus _status;
    MatchStatus _lastStatus;
};

using MatcherPtr = std::unique_ptr<Matcher>;

class StringMatcher : public Matcher {
   public:
    StringMatcher(
        TokenType tokenType,
        std::string str,
        std::optional<OnErrorFunc> onError = std::nullopt)
        : _str(str), _index(0), Matcher(tokenType, onError) {}
    virtual ~StringMatcher() {}

    virtual void reset() override;
    virtual void read(char curChar, char nextChar) override;
    virtual std::string getValue() override { return _str; }

   private:
    std::string _str;
    size_t _index;
};

using StringMatcherPtr = std::unique_ptr<StringMatcher>;

class KeywordMatcher : public StringMatcher {
    using StringMatcher::StringMatcher;
};

class OperatorMatcher : public StringMatcher {
    using StringMatcher::StringMatcher;
};

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
    virtual void reset() override;
    virtual void read(char curChar, char nextChar) override;
    virtual std::string getValue() override;
    std::string getErrorMsg();
    bool check();

   private:
    std::string _val;
    Type _type;
};

using IntConstMatcherPtr = std::unique_ptr<IntConstMatcher>;

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