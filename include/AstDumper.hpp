#pragma once
#include "AstNodesVisitor.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include "AstNodesDecls.hpp"
#include "Logger.hpp"
#include "AstNodes.hpp"

class AstDumper : public AstNodesVisitor {
   public:
    void dumpAll(const AstNodePtrVector& nodes, const std::string& filePath);

   protected:
    virtual void visit(const AstCompUnit& node) override;
    virtual void visit(const AstDecl& node) override;
    virtual void visit(const AstBType& node) override;
    virtual void visit(const AstVarDecl& node) override;
    virtual void visit(const AstVarDef& node) override;
    virtual void visit(const AstInitVal& node) override;
    virtual void visit(const AstFuncDef& node) override;
    virtual void visit(const AstFuncType& node) override;
    virtual void visit(const AstFuncFParams& node) override;
    virtual void visit(const AstFuncFParam& node) override;
    virtual void visit(const AstBlock& node) override;
    virtual void visit(const AstBlockItem& node) override;
    virtual void visit(const AstAssignStmt& node) override;
    virtual void visit(const AstExpStmt& node) override;
    virtual void visit(const AstBlockStmt& node) override;
    virtual void visit(const AstIfStmt& node) override;
    virtual void visit(const AstWhileStmt& node) override;
    virtual void visit(const AstBreakStmt& node) override;
    virtual void visit(const AstContinueStmt& node) override;
    virtual void visit(const AstReturnStmt& node) override;
    virtual void visit(const AstExp& node) override;
    virtual void visit(const AstCond& node) override;
    virtual void visit(const AstLVal& node) override;
    virtual void visit(const AstPrimaryExp& node) override;
    virtual void visit(const AstNumber& node) override;
    virtual void visit(const AstBinaryExp& node) override;
    virtual void visit(const AstUnaryExp& node) override;
    virtual void visit(const AstFuncRParams& node) override;
    virtual void visit(const AstFuncCall& node) override;

   private:
    template <class NodeT>
    void dump(const NodeT& node) {
        node.accept(*this);
    }
    void begin(const std::string& name);
    void end();
    template <typename... Args>
    void output(const std::string& str, Args... args) {
        for (int i = 0; i < _depth; i++) {
            *_os << "  ";
        }
        *_os << stringFormat(str, args...) << "\n";
    }

   private:
    std::ostream* _os;
    int _depth;
};