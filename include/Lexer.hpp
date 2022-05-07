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
    Lexer();

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
    std::tuple<std::vector<Token>, std::vector<std::string>> lex(const std::string& filePath);

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

    /**
     * @brief Get last error message
     *
     * @return an error message if an error occurred in last getNextToken(), otherwise nullopt
     */
    const std::optional<std::string>& getLastError() const { return _lastError; }

    void nextLine();

   private:
    std::vector<MatcherPtr> _difinitions;
    std::ifstream _inputStream;
    std::optional<std::string> _lastError;
    int _lineno;
    char _curChar;
};