#pragma once
#include <any>
#include "AstNodesDecls.hpp"

/**
 * @brief Visitor pattern. ref: https://en.wikipedia.org/wiki/Visitor_pattern
 */

/**
 * @brief Base class for visitors of AstNodes
 */
class AstNodesVisitor {
   public:
    virtual void visit(const AstCompUnit&) = 0;
    virtual void visit(const AstDecl&) = 0;
    virtual void visit(const AstBType&) = 0;
    virtual void visit(const AstVarDecl&) = 0;
    virtual void visit(const AstVarDef&) = 0;
    virtual void visit(const AstInitVal&) = 0;
    virtual void visit(const AstFuncDef&) = 0;
    virtual void visit(const AstFuncType&) = 0;
    virtual void visit(const AstFuncFParams&) = 0;
    virtual void visit(const AstFuncFParam&) = 0;
    virtual void visit(const AstBlock&) = 0;
    virtual void visit(const AstBlockItem&) = 0;
    virtual void visit(const AstAssignStmt&) = 0;
    virtual void visit(const AstExpStmt&) = 0;
    virtual void visit(const AstBlockStmt&) = 0;
    virtual void visit(const AstIfStmt&) = 0;
    virtual void visit(const AstWhileStmt&) = 0;
    virtual void visit(const AstBreakStmt&) = 0;
    virtual void visit(const AstContinueStmt&) = 0;
    virtual void visit(const AstReturnStmt&) = 0;
    virtual void visit(const AstExp&) = 0;
    virtual void visit(const AstCond&) = 0;
    virtual void visit(const AstLVal&) = 0;
    virtual void visit(const AstPrimaryExp&) = 0;
    virtual void visit(const AstNumber&) = 0;
    virtual void visit(const AstBinaryExp&) = 0;
    virtual void visit(const AstUnaryExp&) = 0;
    virtual void visit(const AstFuncRParams&) = 0;
    virtual void visit(const AstFuncCall&) = 0;

    void ret(const std::any& ret) { _ret = ret; }

    template <class RetT>
    RetT& result() { return std::any_cast<RetT&>(_ret); }

    template <class RetT, class AstT>
    RetT& visitRet(const AstT& node) {
        node.accept(*this);
        return result<RetT>();
    }

   private:
    std::any _ret;
};