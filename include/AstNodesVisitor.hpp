#pragma once

/**
 * @brief Visitor pattern. ref: https://en.wikipedia.org/wiki/Visitor_pattern
 */

class AstCompUnit;
class AstDecl;
class AstBType;
class AstVarDecl;
class AstVarDef;
class AstInitVal;
class AstFuncDef;
class AstFuncType;
class AstFuncFParams;
class AstFuncFParam;
class AstBlock;
class AstBlockItem;
class AstAssignStmt;
class AstExpStmt;
class AstBlockStmt;
class AstIfStmt;
class AstWhileStmt;
class AstBreakStmt;
class AstContinueStmt;
class AstReturnStmt;
class AstExp;
class AstCond;
class AstLVal;
class AstPrimaryExp;
class AstNumber;
class AstBinaryExp;
class AstUnaryExp;
class AstFuncRParams;
class AstFuncCall;

/**
 * @brief Base class for visitors of AstNodes
 */
class AstNodesVisitor {
   public:
    virtual void visit(AstCompUnit&) = 0;
    virtual void visit(AstDecl&) = 0;
    virtual void visit(AstBType&) = 0;
    virtual void visit(AstVarDecl&) = 0;
    virtual void visit(AstVarDef&) = 0;
    virtual void visit(AstInitVal&) = 0;
    virtual void visit(AstFuncDef&) = 0;
    virtual void visit(AstFuncType&) = 0;
    virtual void visit(AstFuncFParams&) = 0;
    virtual void visit(AstFuncFParam&) = 0;
    virtual void visit(AstBlock&) = 0;
    virtual void visit(AstBlockItem&) = 0;
    virtual void visit(AstAssignStmt&) = 0;
    virtual void visit(AstExpStmt&) = 0;
    virtual void visit(AstBlockStmt&) = 0;
    virtual void visit(AstIfStmt&) = 0;
    virtual void visit(AstWhileStmt&) = 0;
    virtual void visit(AstBreakStmt&) = 0;
    virtual void visit(AstContinueStmt&) = 0;
    virtual void visit(AstReturnStmt&) = 0;
    virtual void visit(AstExp&) = 0;
    virtual void visit(AstCond&) = 0;
    virtual void visit(AstLVal&) = 0;
    virtual void visit(AstPrimaryExp&) = 0;
    virtual void visit(AstNumber&) = 0;
    virtual void visit(AstBinaryExp&) = 0;
    virtual void visit(AstUnaryExp&) = 0;
    virtual void visit(AstFuncRParams&) = 0;
    virtual void visit(AstFuncCall&) = 0;
};