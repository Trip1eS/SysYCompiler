#pragma once
#include "Token.hpp"
#include "AstNodes.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <sstream>

/**
 * CompUnit     ->  [CompUnit] (Decl | FuncDef)
 * Decl         ->  ConstDecl | VarDecl
 * ConstDecl    ->  'const' BType ConstDef {',' ConstDef} ';'
 * BType        ->  'int'
 * ConstDef     ->  Ident {'[' ConstExp ']'} '=' ConstInitVal
 * ConstInitVal ->  ConstExp
 *                  | '{' [ConstInitVal {',' ConstInitVal}] '}'
 * VarDecl      ->  BType VarDef {',' VarDef} ';'
 * VarDef       ->  Ident {'[' ConstExp ']'}
 *                  | Ident {'[' ConstExp ']'} '=' InitVal
 * InitVal      ->  Exp | '{' [InitVal {',' InitVal}] '}'
 * FuncDef      ->  FuncType Ident '(' [FuncFParams] ')' Block
 * FuncType     ->  'void' | 'int'
 * FuncFParams  ->  FuncFParam {',' FuncFParam}
 * FuncFParam   ->  BType Ident ['[' ']' { '[' Exp ']' }]
 * Block        ->  '{' { BlockItem } '}'
 * BlockItem    ->  Decl | Stmt
 * Stmt         ->  LVal '=' Exp ';'
 *                  | [Exp] ';'
 *                  | Block
 *                  | 'if' '( Cond ')' Stmt [ 'else' Stmt ]
 *                  | 'while' '(' Cond ')' Stmt
 *                  | 'break' ';'
 *                  | 'continue' ';'
 *                  | 'return' [Exp] ';'
 * Exp          ->  AddExp 注:SysY 表达式是 int 型表达式
 * Cond         ->  LOrExp
 * LVal         ->  Ident {'[' Exp ']'}
 * PrimaryExp   ->  '(' Exp ')' | LVal | Number
 * Number       ->  IntConst
 * UnaryExp     ->  PrimaryExp
 *                  | Ident '(' [FuncRParams] ')'
 *                  | UnaryOp UnaryExp
 * UnaryOp      ->  '+' | '−' | '!' 注:'!'仅出现在条件表达式中
 * FuncRParams  ->  Exp{','Exp}
 * MulExp       ->  UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
 * AddExp       ->  MulExp | AddExp ('+' | '−') MulExp
 * RelExp       ->  AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
 * EqExp        ->  RelExp | EqExp ('==' | '!=') RelExp
 * LAndExp      ->  EqExp | LAndExp '&&' EqExp
 * LOrExp       ->  LAndExp | LOrExp '||' LAndExp
 * ConstExp     ->  AddExp 注:使用的 Ident 必须是常量
 */

class ParsingError {
   public:
    int lineno;
    std::string msg;
    ParsingError(int _lineno, std::string _msg)
        : lineno(_lineno), msg(_msg) {}
};

class Parser {
   public:
    Parser(std::vector<Token> tokens) : _tokens(tokens) {
        _tokenIter = _tokens.begin();
    }
    void parse();
    void outputAst(const std::string& filePath);

   private:
    ParsingError error(const std::string& msg);

    template <typename T, typename... Args>
    std::unique_ptr<T> makeAstNode(Args&&... args) {
        auto ast = std::make_unique<T>(std::forward<Args>(args)...);
        _astDepth--;
        return ast;
    }

#pragma region Logging functions
    void addDepth() { _astDepth++; }
    void reduceDepth() { _astDepth--; }
    void logAstNode(std::string_view str) {
        addDepth();
        for (int i = 0; i < _astDepth * 2; i++) {
            _astLogStream << ' ';
        }
        _astLogStream << str << " (" << curToken().getLineno() << ")\n";
    }
    void logAstLeaf(std::string_view str) {
        addDepth();
        for (int i = 0; i < _astDepth * 2; i++) {
            _astLogStream << ' ';
        }
        _astLogStream << str << '\n';
        reduceDepth();
    }

    void logAstToken(const Token& token);
#pragma endregion

#pragma region Tokens

    void match(TokenType type, const std::string& msg) {
        if (tryToken(type)) {
            nextToken();
        } else {
            throw error(msg);
        }
    }

    void match(TokenType type) {
        if (type == TokenType::ID) {
            match(type, "expected an identifier");
        } else if (type == TokenType::INTCON) {
            match(type, "expected a number");
        } else {
            match(type, "expected a '" + std::string(getTokenValue(type)) + "'");
        }
    }

    bool tryMatch(TokenType type) {
        if (tryToken(type)) {
            nextToken();
            return true;
        } else {
            return false;
        }
    }
    void nextToken() {
        // TODO: output AST leaf
        logAstToken(curToken());
        ++_tokenIter;
    }
    void prevToken() {
        --_tokenIter;
    }
    const Token& curToken() const { return *_tokenIter; }

    template <class... Args>
    bool tryTokenAhead(int distance, Args... args) {
        auto tempIter = _tokenIter;
        std::advance(_tokenIter, distance);
        auto ret = tryToken(args...);
        _tokenIter = tempIter;
        return ret;
    }

    template <class... Args>
    bool tryToken(Args... args) const {
        if ((_tokenIter->is(args) || ...)) return true;
        return false;
    }

    bool findToken(TokenType target, TokenType until) {
        auto tempIter = _tokenIter;
        while (!tempIter->is(until)) {
            if (tempIter->is(target)) return true;
            ++tempIter;
        }
        return false;
    }

#pragma endregion

#pragma region Parsing functions
    std::string parseID();
    AstNodePtr parseCompUnit();
    AstNodePtr parseDecl();
    AstNodePtr parseConstDecl();
    AstNodePtr parseBType();
    AstNodePtr parseConstDef();
    AstNodePtr parseConstInitVal();
    AstNodePtr parseVarDecl();
    AstNodePtr parseVarDef();
    AstNodePtr parseInitVal();
    AstNodePtr parseFuncDef();
    AstNodePtr parseFuncType();
    AstNodePtr parseFuncFParams();
    AstNodePtr parseFuncFParam();
    AstNodePtr parseBlock();
    AstNodePtr parseBlockItem();
    AstNodePtr parseStmt();
    AstNodePtr parseExp();
    AstNodePtr parseConstExp();
    AstNodePtr parseCond();
    AstNodePtr parseLVal();
    AstNodePtr parsePrimaryExp();
    AstNodePtr parseNumber();
    AstNodePtr parseUnaryExp();
    AstNodePtr parseFuncRParams();
    AstNodePtr parseMulExp();
    AstNodePtr parseAddExp();
    AstNodePtr parseRelExp();
    AstNodePtr parseEqExp();
    AstNodePtr parseLAndExp();
    AstNodePtr parseLOrExp();
    AstNodePtr parseFuncCall();
#pragma endregion

   private:
    std::vector<Token> _tokens;
    std::vector<Token>::iterator _tokenIter;
    std::vector<ParsingError> _errors;
    AstNodePtrVector _compUnits;

    std::stringstream _astLogStream;
    int _astDepth = -1;
};
