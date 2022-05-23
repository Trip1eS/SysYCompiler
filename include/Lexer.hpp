#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include <memory>
#include <optional>
#include <fstream>
#include <exception>
#include "Token.hpp"
#include "Matcher.hpp"

class LexingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @brief Lexical analysis class
 */
class Lexer {
   public:
    Lexer(const std::string& filePath);

    /**
     * @brief Add a difinition to this lexer
     *
     * @param matcher a Matcher for this difinition
     */
    void addDifinition(MatcherPtr matcher);

    /**
     * @brief Just a helper function of addDifinition
     *
     * @see addDifinition(MatcherPtr)
     */
    template <class T, class... Ts>
    void addDifinition(Ts&&... args) {
        addDifinition(std::move(std::make_unique<T>(args...)));
    }

    /**
     * @brief Lexical analyze an SysY source file
     *
     * @param filePath the relative path to the file
     * @return a tuple of (vector of tokens, vector of error messages)
     */
    void lex();

    /**
     * @brief Load an SysY source file into input stream
     *
     * @param filePath the relative path to the file
     */
    void loadFile(const std::string& filePath);

    /**
     * @brief Get next token object
     *
     * @return a token if success, otherwise (EOF/errors) nullopt
     */
    std::optional<Token> getNextToken();

    bool hasError() const { return _hasError; }

    /**
     * @brief Output the token list to file.
     */
    void outputTokens(const std::string& path) const;

    auto& getTokens() { return _tokens; }

   private:
    void nextLine();

   private:
    std::vector<MatcherPtr> _difinitions;
    std::vector<Token> _tokens;
    std::ifstream _inputStream;
    int _lineno;
    char _curChar;
    bool _hasError;
};