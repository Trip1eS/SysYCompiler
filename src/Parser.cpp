#include "Parser.hpp"
#include <fstream>
#include <stack>
#include "Logger.hpp"

BinaryOp getBinaryOp(TokenType type) {
#define CASE(X)        \
    case TokenType::X: \
        return BinaryOp::X;
    switch (type) {
        CASE(PLUS)
        CASE(SUB)
        CASE(MUL)
        CASE(DIV)
        CASE(MOD)
        CASE(LESS)
        CASE(GREATER)
        CASE(LESSEQ)
        CASE(GREATEREQ)
        CASE(EQUAL)
        CASE(NEQUAL)
        CASE(LOGICAND)
        CASE(LOGICOR)
        default:
            return BinaryOp::SINGLE;
    }
#undef CASE
}

Parser::Parser(std::vector<Token> tokens)
    : _tokens(tokens) {
    _tokenIter = _tokens.begin();
}

void Parser::reset() {
    _astDepth = -1;
    _hasError = false;
}

void Parser::parse() {
    reset();
    log() << "(Parser) Start parsing...\n";
    while (!eof()) {
        try {
            auto compUnit = parseCompUnit();
            if (compUnit) _compUnits.push_back(std::move(compUnit));
        } catch (const ParsingError &err) {
            nextToken();
        }
    }
    if (_hasError) {
        log() << "(Parser) Parsing done with errors.\n";
    } else {
        log() << "(Parser) Parsing done with success.\n";
    }
}

void Parser::outputAst(const std::string &filePath) {
    if (_compUnits.empty()) {
        err() << "(Parser) No CompUnit avaliable\n";
        return;
    }
    std::ofstream file(filePath, std::ios::out);
    file << _astLogStream.rdbuf();
    log() << "(Parser) Written AST to file: " << filePath << "\n";
}

void Parser::logAstToken(const Token &token) {
    std::stringstream log;
    if (token.is(TokenType::ID) || token.is(TokenType::INTCON)) {
        log << token.getName() << ": " << token.getValue();
    } else if (token.is(TokenType::INTTK) || token.is(TokenType::VOID)) {
        log << "Type: " << token.getValue();
    } else {
        log << token.getName() << ": " << token.getValue();
    }
    logAstLeaf(log.str());
}

ParsingError Parser::error(const std::string &msg) {
    int lineno;
    if (_tokenIter == _tokens.end())
        lineno = _tokens.back().getLineno();
    else
        lineno = curToken().getLineno();
    _errors.emplace_back(lineno, msg);
    err() << "Error Type B at line " << lineno << " : " << msg << "\n";
    _hasError = true;
    return ParsingError(curToken().getLineno(), msg);
}

void Parser::logAstNode(std::string_view str) {
    _astDepth++;
    for (int i = 0; i < _astDepth * 2; i++) {
        _astLogStream << ' ';
    }
    _astLogStream << str << " (" << curToken().getLineno() << ")\n";
}

void Parser::logAstLeaf(std::string_view str) {
    _astDepth++;
    for (int i = 0; i < _astDepth * 2; i++) {
        _astLogStream << ' ';
    }
    _astLogStream << str << '\n';
    _astDepth--;
}

void Parser::match(TokenType type, const std::string &msg) {
    if (tryToken(type)) {
        nextToken();
    } else {
        throw error(msg);
    }
}

void Parser::match(TokenType type) {
    if (type == TokenType::ID) {
        match(type, "expected an identifier");
    } else if (type == TokenType::INTCON) {
        match(type, "expected a number");
    } else {
        match(type, "expected a '" + std::string(getTokenValue(type)) + "'");
    }
}

bool Parser::tryMatch(TokenType type) {
    if (tryToken(type)) {
        nextToken();
        return true;
    } else {
        return false;
    }
}

void Parser::nextToken() {
    logAstToken(curToken());
    ++_tokenIter;
}

void Parser::prevToken() {
    --_tokenIter;
}

bool Parser::findToken(TokenType target, TokenType until) {
    auto tempIter = _tokenIter;
    while (!tempIter->is(until)) {
        if (tempIter->is(target)) return true;
        ++tempIter;
    }
    return false;
}

std::string Parser::parseID() {
    auto id = curToken().getValue();
    match(TokenType::ID);
    return id;
}

/**
 * CompUnit -> Decl | FuncDef
 */
AstCompUnitPtr Parser::parseCompUnit() {
    logAstNode("CompUnit");
    if (tryToken(TokenType::CONST)) {
        // -> ConstDecl
        return makeAstNode<AstCompUnit>(parseDecl());
    }
    if (!tryToken(TokenType::INTTK) && !tryToken(TokenType::VOID)) throw error("expected a type");
    if (!tryTokenAhead(1, TokenType::ID)) throw error("expected an identifier");

    // Check the 2nd token after current token. If it's a '(', then we do 'CompUnit -> FuncDef'.
    // We want to avoid back-tracking and it's a good solution.
    // It means it's an LL(2) grammar, but the 2 look-aheads only happens here.
    if (tryTokenAhead(2, TokenType::LPARENT)) {
        // -> FuncDef
        auto funcDef = parseFuncDef();
        return makeAstNode<AstCompUnit>(std::move(funcDef));
    } else {
        // -> VarDecl
        return makeAstNode<AstCompUnit>(parseDecl());
    }
}

/**
 * Decl -> ConstDecl | VarDecl
 */
AstDeclPtr Parser::parseDecl() {
    logAstNode("Decl");
    if (tryToken(TokenType::CONST))
        return makeAstNode<AstDecl>(parseConstDecl());
    else
        return makeAstNode<AstDecl>(parseVarDecl());
}

AstNodePtr Parser::parseConstDecl() {
    return nullptr;
}

/**
 * BType -> 'int'
 */
AstBTypePtr Parser::parseBType() {
    logAstNode("BType");
    if (tryMatch(TokenType::INTTK)) {
        return makeAstNode<AstBType>(BType::INT);
    } else {
        throw error("invalid type");
    }
}

AstNodePtr Parser::parseConstDef() {
    return nullptr;
}

AstNodePtr Parser::parseConstInitVal() {
    return nullptr;
}

/**
 * VarDecl -> BType VarDef {',' VarDef} ';'
 */
AstVarDeclPtr Parser::parseVarDecl() {
    logAstNode("VarDecl");
    std::vector<AstVarDefPtr> defs;
    auto type = parseBType();
    while (true) {
        auto def = parseVarDef();
        defs.push_back(std::move(def));
        if (!tryMatch(TokenType::COMMA)) break;
    }
    match(TokenType::SEMICN);
    return makeAstNode<AstVarDecl>(std::move(type), std::move(defs));
}

/**
 * VarDef -> Ident {'[' ConstExp ']'}
 *           | Ident {'[' ConstExp ']'} '=' InitVal
 */
AstVarDefPtr Parser::parseVarDef() {
    logAstNode("VarDef");
    std::vector<AstExpPtr> arrLens;
    AstInitValPtr initVal;
    auto id = parseID();
    while (true) {
        if (tryMatch(TokenType::LSQBRA)) {
            auto exp = parseExp();
            arrLens.push_back(std::move(exp));
            if (!tryMatch(TokenType::RSQBRA)) throw error("expected a ']'");
        } else {
            break;
        }
    }
    if (tryMatch(TokenType::ASSIGN)) {
        initVal = parseInitVal();
    }
    return makeAstNode<AstVarDef>(id, std::move(arrLens), std::move(initVal));
}

/**
 * InitVal -> Exp | '{' [InitVal { ',' InitVal }] '}'
 */
AstInitValPtr Parser::parseInitVal() {
    logAstNode("InitVal");
    if (tryMatch(TokenType::LBRACE)) {
        // -> '{' [InitVal { ',' InitVal }] '}'
        std::vector<AstInitValPtr> initVals;
        auto initVal = parseInitVal();
        initVals.push_back(std::move(initVal));
        while (tryToken(TokenType::COMMA)) {
            nextToken();
            initVal = parseInitVal();
            initVals.push_back(std::move(initVal));
        }
        nextToken();  // skip ';'
        return makeAstNode<AstInitVal>(std::move(initVals));
    } else {
        // -> Exp
        auto exp = parseExp();
        return makeAstNode<AstInitVal>(std::move(exp));
    }
}

/**
 * FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
 */
AstFuncDefPtr Parser::parseFuncDef() {
    logAstNode("FuncDef");
    auto funcType = parseFuncType();
    auto id = parseID();
    match(TokenType::LPARENT);
    AstFuncFParamsPtr params;
    if (!tryToken(TokenType::RPARENT)) params = parseFuncFParams();
    match(TokenType::RPARENT);
    auto block = parseBlock();
    return makeAstNode<AstFuncDef>(std::move(funcType), id, std::move(params), std::move(block));
}

/**
 * FuncType -> 'void' | 'int'
 */
AstFuncTypePtr Parser::parseFuncType() {
    logAstNode("FuncType");
    if (tryMatch(TokenType::INTTK)) {
        return makeAstNode<AstFuncType>(FuncType::INT);
    } else if (tryMatch(TokenType::VOID)) {
        return makeAstNode<AstFuncType>(FuncType::VOID);
    } else {
        throw error("expected function return type ('int' or 'void')");
    }
}

/**
 * FuncFParams -> FuncFParam { ',' FuncFParam }
 */
AstFuncFParamsPtr Parser::parseFuncFParams() {
    logAstNode("FuncFParams");
    std::vector<AstFuncFParamPtr> params;
    do {
        params.emplace_back(parseFuncFParam());
    } while (tryMatch(TokenType::COMMA));
    return makeAstNode<AstFuncFParams>(std::move(params));
}

/**
 * FuncFParam -> BType Ident [ '[' ']' { '[' Exp ']' } ]
 */
AstFuncFParamPtr Parser::parseFuncFParam() {
    // TODO: array support
    logAstNode("FuncFParam");
    auto type = parseBType();
    auto id = parseID();
    return makeAstNode<AstFuncFParam>(std::move(type), id);
}

/**
 * Block -> '{' {BlockItem} '}'
 */
AstBlockPtr Parser::parseBlock() {
    logAstNode("Block");
    if (!tryMatch(TokenType::LBRACE)) throw error("expected a '{'");
    AstNodePtrVector items;
    while (!tryToken(TokenType::RBRACE)) {
        try {
            items.emplace_back(parseBlockItem());
        } catch (const ParsingError &err) {
            int curLineno = curToken().getLineno();
            while (curToken().getLineno() == curLineno) nextToken();
        }
    }

    nextToken();
    return makeAstNode<AstBlock>(std::move(items));
}

/**
 * BlockItem -> Decl | Stmt
 */
AstBlockItemPtr Parser::parseBlockItem() {
    logAstNode("BlockItem");
    AstNodePtr item;
    if (tryToken(TokenType::CONST, TokenType::INTTK)) {
        item = parseDecl();
    } else {
        item = parseStmt();
    }
    return makeAstNode<AstBlockItem>(std::move(item));
}

/**
 * Stmt   ->  LVal '=' Exp ';'
 *          | [Exp] ';'
 *          | Block
 *          | 'if' '( Cond ')' Stmt [ 'else' Stmt ]
 *          | 'while' '(' Cond ')' Stmt
 *          | 'break' ';'
 *          | 'continue' ';'
 *          | 'return' [Exp] ';'
 */
AstNodePtr Parser::parseStmt() {
    logAstNode("Stmt");
    if (tryMatch(TokenType::IF)) {
        // -> 'if' '( Cond ')' Stmt [ 'else' Stmt ]
        match(TokenType::LPARENT);
        auto cond = parseCond();
        match(TokenType::RPARENT);
        auto stmt = parseStmt();
        AstNodePtr elseStmt;
        if (tryMatch(TokenType::ELSE)) {
            elseStmt = parseStmt();
        }
        return makeAstNode<AstIfStmt>(std::move(cond), std::move(stmt), std::move(elseStmt));
    } else if (tryMatch(TokenType::WHILE)) {
        // -> 'while' '(' Cond ')' Stmt
        match(TokenType::LPARENT);
        auto cond = parseCond();
        match(TokenType::RPARENT);
        auto stmt = parseStmt();
        return makeAstNode<AstWhileStmt>(std::move(cond), std::move(stmt));
    } else if (tryMatch(TokenType::BREAK)) {
        // -> 'break' ';'
        match(TokenType::SEMICN);
        return makeAstNode<AstBreakStmt>();
    } else if (tryMatch(TokenType::CONT)) {
        // -> 'continue' ';'
        match(TokenType::SEMICN);
        return makeAstNode<AstContinueStmt>();
    } else if (tryMatch(TokenType::RETURN)) {
        // -> 'return' [Exp] ';'
        if (tryMatch(TokenType::SEMICN)) {
            return makeAstNode<AstReturnStmt>();
        }
        auto exp = parseExp();
        match(TokenType::SEMICN);
        return makeAstNode<AstReturnStmt>(std::move(exp));
    } else if (tryToken(TokenType::LBRACE)) {
        // -> Block
        auto block = parseBlock();
        return makeAstNode<AstBlockStmt>(std::move(block));
    } else if (findToken(TokenType::ASSIGN, TokenType::SEMICN)) {
        // -> LVal '=' Exp ';'
        auto lVal = parseLVal();
        match(TokenType::ASSIGN);
        auto exp = parseExp();
        match(TokenType::SEMICN);
        return makeAstNode<AstAssignStmt>(std::move(lVal), std::move(exp));
    } else {
        // -> [Exp] ';'
        if (tryMatch(TokenType::SEMICN)) {
            return makeAstNode<AstExpStmt>();
        }
        auto exp = parseExp();
        match(TokenType::SEMICN);
        return makeAstNode<AstExpStmt>(std::move(exp));
    }
}

/**
 * Exp -> AddExp
 */
AstExpPtr Parser::parseExp() {
    logAstNode("Exp");
    auto addExp = parseAddExp();
    return makeAstNode<AstExp>(std::move(addExp));
}

AstNodePtr Parser::parseConstExp() {
    return nullptr;
}

AstCondPtr Parser::parseCond() {
    logAstNode("Cond");
    auto lOrExp = parseLOrExp();
    return makeAstNode<AstCond>(std::move(lOrExp));
}

/**
 * LVal -> Ident {'[' Exp ']'}
 */
AstLValPtr Parser::parseLVal() {
    logAstNode("LVal");
    auto id = parseID();
    AstNodePtrVector indices;
    while (tryMatch(TokenType::LSQBRA)) {
        auto exp = parseExp();
        if (!tryMatch(TokenType::RSQBRA)) throw error("expected a ']'");
        indices.push_back(std::move(exp));
    }
    return makeAstNode<AstLVal>(id, std::move(indices));
}

/**
 * PrimaryExp -> '(' Exp ')' | LVal | Number
 */
AstPrimaryExpPtr Parser::parsePrimaryExp() {
    logAstNode("PrimaryExp");
    if (tryToken(TokenType::LPARENT)) {
        // -> '(' Exp ')'
        nextToken();
        return makeAstNode<AstPrimaryExp>(parseExp());
    } else if (tryToken(TokenType::ID)) {
        // -> LVal
        return makeAstNode<AstPrimaryExp>(parseLVal());
    } else if (tryToken(TokenType::INTCON)) {
        // -> Number
        return makeAstNode<AstPrimaryExp>(parseNumber());
    }
    return nullptr;
}

AstNumberPtr Parser::parseNumber() {
    logAstNode("Number");
    if (tryToken(TokenType::INTCON)) {
        auto val = std::stoi(curToken().getValue());
        nextToken();
        auto ret = makeAstNode<AstNumber>(val);
        return ret;
    }
    return nullptr;
}

/**
 * UnaryExp -> PrimaryExp
 *          | FuncCall | UnaryOp UnaryExp
 */
AstUnaryExpPtr Parser::parseUnaryExp() {
    logAstNode("UnaryExp");
    if (tryTokenAhead(1, TokenType::LPARENT)) {
        // -> FuncCall
        return makeAstNode<AstUnaryExp>(parseFuncCall());
    } else if (tryToken(TokenType::PLUS, TokenType::SUB, TokenType::NOT)) {
        // -> UnaryOp UnaryExp
        UnaryOp op;
        if (tryToken(TokenType::PLUS)) {
            // '+' has no effects, just skip
            nextToken();
            auto exp = parseUnaryExp();
            _astDepth--;
            return exp;
        } else if (tryToken(TokenType::SUB))
            op = UnaryOp::MINUS;
        else
            op = UnaryOp::NOT;
        nextToken();
        auto exp = parseUnaryExp();
        if (!exp) return nullptr;
        return makeAstNode<AstUnaryExp>(op, std::move(exp));
    } else {
        // -> PrimaryExp
        return makeAstNode<AstUnaryExp>(parsePrimaryExp());
    }
}

AstFuncRParamsPtr Parser::parseFuncRParams() {
    return nullptr;
}

/**
 * MulExp -> UnaryExp | UnaryExp ('*' | '/' | '%') MulExp
 */
AstBinaryExpPtr Parser::parseMulExp() {
    logAstNode("MulExp");
    auto lhs = parseUnaryExp();
    BinaryOp op;
    AstNodePtr rhs;
    if (!lhs) return nullptr;
    if (!tryToken(TokenType::MUL, TokenType::DIV, TokenType::MOD)) {
        return makeAstNode<AstBinaryExp>(std::move(lhs));
    }
    while (tryToken(TokenType::MUL, TokenType::DIV, TokenType::MOD)) {
        if (tryToken(TokenType::MUL)) {
            op = BinaryOp::MUL;
        } else if (tryToken(TokenType::DIV)) {
            op = BinaryOp::DIV;
        } else {
            op = BinaryOp::MOD;
        }
        nextToken();
        rhs = parseMulExp();
        if (!rhs) return nullptr;
    }
    return makeAstNode<AstBinaryExp>(std::move(lhs), op, std::move(rhs));
}

/**
 * AddExp -> MulExp | MulExp ('+' | '-') AddExp
 */
AstBinaryExpPtr Parser::parseAddExp() {
    logAstNode("AddExp");
    auto lhs = parseMulExp();
    BinaryOp op;
    AstNodePtr rhs;
    if (!lhs) return nullptr;
    if (!tryToken(TokenType::PLUS, TokenType::SUB)) {
        return makeAstNode<AstBinaryExp>(std::move(lhs));
    }
    if (tryToken(TokenType::PLUS)) {
        op = BinaryOp::PLUS;
    } else {
        op = BinaryOp::SUB;
    }
    nextToken();
    rhs = parseAddExp();
    if (!rhs) return nullptr;
    return makeAstNode<AstBinaryExp>(std::move(lhs), op, std::move(rhs));
}

/**
 * RelExp ->  AddExp | AddExp ('<' | '>' | '<=' | '>=') RelExp
 */
AstBinaryExpPtr Parser::parseRelExp() {
    logAstNode("RelExp");
    auto lhs = parseAddExp();
    BinaryOp op;
    if (!tryToken(TokenType::LESS, TokenType::GREATER, TokenType::LESSEQ, TokenType::GREATEREQ))
        return makeAstNode<AstBinaryExp>(std::move(lhs));
    if (tryToken(TokenType::LESS)) {
        op = BinaryOp::LESS;
    } else if (tryToken(TokenType::GREATER)) {
        op = BinaryOp::GREATER;
    } else if (tryToken(TokenType::LESSEQ)) {
        op = BinaryOp::LESSEQ;
    } else {
        op = BinaryOp::GREATEREQ;
    }
    nextToken();
    auto rhs = parseRelExp();
    return makeAstNode<AstBinaryExp>(std::move(lhs), op, std::move(rhs));
}

/**
 * EaExp -> RelExp | RelExp ('==' | '!=') EqExp
 */
AstBinaryExpPtr Parser::parseEqExp() {
    logAstNode("EqExp");
    auto lhs = parseRelExp();
    BinaryOp op;
    if (!tryToken(TokenType::EQUAL, TokenType::NEQUAL)) return makeAstNode<AstBinaryExp>(std::move(lhs));
    if (tryToken(TokenType::EQUAL)) {
        op = BinaryOp::EQUAL;
    } else {
        op = BinaryOp::NEQUAL;
    }
    nextToken();
    auto rhs = parseEqExp();
    return makeAstNode<AstBinaryExp>(std::move(lhs), op, std::move(rhs));
}

/**
 * LAndExp -> EqExp | EqExp '&&' LAndExp
 */
AstBinaryExpPtr Parser::parseLAndExp() {
    logAstNode("LAndExp");
    auto lhs = parseEqExp();
    BinaryOp op = BinaryOp::LOGICAND;
    if (!tryToken(TokenType::LOGICAND)) return makeAstNode<AstBinaryExp>(std::move(lhs));
    match(TokenType::LOGICAND);
    auto rhs = parseLAndExp();
    return makeAstNode<AstBinaryExp>(std::move(lhs), op, std::move(rhs));
}

/**
 * LOrExp -> LAndExp | LAndExp '||' LOrExp
 */
AstBinaryExpPtr Parser::parseLOrExp() {
    logAstNode("LOrExp");
    auto lhs = parseLAndExp();
    BinaryOp op = BinaryOp::LOGICOR;
    if (!tryToken(TokenType::LOGICOR)) return makeAstNode<AstBinaryExp>(std::move(lhs));
    match(TokenType::LOGICOR);
    auto rhs = parseLOrExp();
    return makeAstNode<AstBinaryExp>(std::move(lhs), op, std::move(rhs));
}

/**
 * FuncCall -> Ident '(' [Exp {',' Exp}] ')'
 */
AstFuncCallPtr Parser::parseFuncCall() {
    logAstNode("FuncCall");
    auto id = parseID();
    match(TokenType::LPARENT);
    AstNodePtrVector params;
    if (!tryToken(TokenType::RPARENT)) {
        // has parameters
        auto exp = parseExp();
        if (!exp) return nullptr;
        params.push_back(std::move(exp));
        while (tryMatch(TokenType::COMMA)) {
            exp = parseExp();
            if (!exp) return nullptr;
            params.push_back(std::move(exp));
        }
    }
    match(TokenType::RPARENT);
    return makeAstNode<AstFuncCall>(id, std::move(params));
}