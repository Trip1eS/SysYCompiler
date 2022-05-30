#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <variant>
#include "AstNodesVisitor.hpp"

enum class FuncType {
    VOID,
    INT
};

enum class BType {
    INT
};

enum class BinaryOp {
    PLUS,
    SUB,
    MUL,
    DIV,
    MOD,
    LESS,
    GREATER,
    LESSEQ,
    GREATEREQ,
    EQUAL,
    NEQUAL,
    LOGICAND,
    LOGICOR,
    SINGLE
};

enum class UnaryOp {
    PLUS,
    MINUS,
    NOT,
    SINGLE
};

#define AST_NODE \
   public:       \
    virtual void accept(AstNodesVisitor &visitor) const override { visitor.visit(*this); }

class AstNodeBase {
   public:
    virtual ~AstNodeBase() = default;
    virtual void accept(AstNodesVisitor &visitor) const = 0;
};

using AstNodePtr = std::unique_ptr<AstNodeBase>;
using AstNodePtrVector = std::vector<AstNodePtr>;

class AstCompUnit : public AstNodeBase {
    AST_NODE
   public:
    AstCompUnit(AstNodePtr next) : _next(std::move(next)) {}

    const auto &next() const { return _next; }

   private:
    AstNodePtr _next;
};

class AstDecl : public AstNodeBase {
    AST_NODE
   public:
    AstDecl(AstNodePtr decl)
        : _decl(std::move(decl)) {}

    const auto &decl() const { return _decl; }

   private:
    AstNodePtr _decl;
};

// class AstConstDecl : public AstNodeBase {};

class AstBType : public AstNodeBase {
    AST_NODE
   public:
    AstBType(BType type)
        : _type(type) {}

    BType type() const { return _type; }

   private:
    BType _type;
};

// class AstConstDef : public AstNodeBase {};
// class AstConstInitVal : public AstNodeBase {};

class AstVarDecl : public AstNodeBase {
    AST_NODE
   public:
    AstVarDecl(AstBTypePtr type,
               std::vector<AstVarDefPtr> varDefs)
        : _type(std::move(type)), _varDefs(std::move(varDefs)) {}

    const auto &type() const { return _type; }
    const auto &varDefs() const { return _varDefs; }

   private:
    AstBTypePtr _type;
    std::vector<AstVarDefPtr> _varDefs;
};

class AstVarDef : public AstNodeBase {
    AST_NODE
   public:
    AstVarDef(std::string id,
              std::vector<AstExpPtr> arrLens,
              AstInitValPtr initVal)
        : _id(std::move(id)), _arrLens(std::move(arrLens)), _initVal(std::move(initVal)) {}

    const auto &id() const { return _id; }
    const auto &arrLens() const { return _arrLens; }
    const auto &initVal() const { return _initVal; }

   private:
    std::string _id;
    std::vector<AstExpPtr> _arrLens;
    AstInitValPtr _initVal;
};

class AstInitVal : public AstNodeBase {
    AST_NODE
   public:
    AstInitVal(std::vector<AstInitValPtr> initVals)
        : _initVals(std::move(initVals)) {}
    AstInitVal(AstExpPtr exp)
        : _exp(std::move(exp)) {}

    const auto &initVals() { return _initVals; }
    const auto &exp() const { return _exp; }

   private:
    std::vector<AstInitValPtr> _initVals;
    AstExpPtr _exp;
};

class AstFuncDef : public AstNodeBase {
    AST_NODE
   public:
    AstFuncDef(AstFuncTypePtr funcType,
               std::string id,
               AstFuncFParamsPtr params,
               AstBlockPtr block)
        : _funcType(std::move(funcType)), _id(id), _params(std::move(params)), _block(std::move(block)) {}

    const auto &funcType() const { return _funcType; }
    const auto &params() const { return _params; }
    const auto &block() const { return _block; }
    const auto &id() const { return _id; }

   private:
    AstFuncTypePtr _funcType;
    AstFuncFParamsPtr _params;
    AstBlockPtr _block;
    std::string _id;
};

class AstFuncType : public AstNodeBase {
    AST_NODE
   public:
    AstFuncType(FuncType type)
        : _type(type) {}

    FuncType type() const { return _type; }

   private:
    FuncType _type;
};

class AstFuncFParams : public AstNodeBase {
    AST_NODE
   public:
    AstFuncFParams(std::vector<AstFuncFParamPtr> params)
        : _params(std::move(params)) {}

    const auto &params() const { return _params; }

   private:
    std::vector<AstFuncFParamPtr> _params;
};

class AstFuncFParam : public AstNodeBase {
    AST_NODE
   public:
    AstFuncFParam(AstNodePtr type, std::string id)
        : _type(std::move(type)), _id(id) {}
    const auto &type() const { return _type; }
    const auto &id() const { return _id; }

   private:
    AstNodePtr _type;
    std::string _id;
};

class AstBlock : public AstNodeBase {
    AST_NODE
   public:
    AstBlock(AstNodePtrVector items)
        : _items(std::move(items)) {}
    const auto &items() const { return _items; }

   private:
    AstNodePtrVector _items;
};

class AstBlockItem : public AstNodeBase {
    AST_NODE
   public:
    AstBlockItem(AstNodePtr declOrStmt)
        : _declOrStmt(std::move(declOrStmt)) {}
    const auto &declOrStmt() const { return _declOrStmt; }

   private:
    AstNodePtr _declOrStmt;
};

class AstAssignStmt : public AstNodeBase {
    AST_NODE
   public:
    AstAssignStmt(AstLValPtr lVal, AstNodePtr exp)
        : _lVal(std::move(lVal)), _exp(std::move(exp)) {}
    const auto &lVal() const { return _lVal; }
    const auto &exp() const { return _exp; }

   private:
    AstLValPtr _lVal;
    AstNodePtr _exp;
};

class AstExpStmt : public AstNodeBase {
    AST_NODE
   public:
    AstExpStmt(AstNodePtr exp = nullptr)
        : _exp(std::move(exp)) {}
    const auto &exp() const { return _exp; }

   private:
    AstNodePtr _exp;
};

class AstBlockStmt : public AstNodeBase {
    AST_NODE
   public:
    AstBlockStmt(AstNodePtr block)
        : _block(std::move(block)) {}
    const auto &block() const { return _block; }

   private:
    AstNodePtr _block;
};

class AstIfStmt : public AstNodeBase {
    AST_NODE
   public:
    AstIfStmt(AstNodePtr cond, AstNodePtr stmt, AstNodePtr elseStmt = nullptr)
        : _cond(std::move(cond)), _stmt(std::move(stmt)), _elseStmt(std::move(elseStmt)) {}
    const auto &cond() const { return _cond; }
    const auto &stmt() const { return _stmt; }
    const auto &elseStmt() const { return _elseStmt; }

   private:
    AstNodePtr _cond, _stmt, _elseStmt;
};

class AstWhileStmt : public AstNodeBase {
    AST_NODE
   public:
    AstWhileStmt(AstNodePtr cond, AstNodePtr stmt)
        : _cond(std::move(cond)), _stmt(std::move(stmt)) {}
    const auto &cond() const { return _cond; }
    const auto &stmt() const { return _stmt; }

   private:
    AstNodePtr _cond, _stmt;
};

class AstBreakStmt : public AstNodeBase {
    AST_NODE
};

class AstContinueStmt : public AstNodeBase {
    AST_NODE
};

class AstReturnStmt : public AstNodeBase {
    AST_NODE
   public:
    AstReturnStmt(AstNodePtr exp = nullptr)
        : _exp(std::move(exp)) {}
    const auto &exp() const { return _exp; }

   private:
    AstNodePtr _exp;
};

class AstExp : public AstNodeBase {
    AST_NODE
   public:
    AstExp(AstNodePtr addExp)
        : _addExp(std::move(addExp)) {}
    const auto &addExp() const { return _addExp; }

   private:
    AstNodePtr _addExp;
};

class AstCond : public AstNodeBase {
    AST_NODE
   public:
    AstCond(AstNodePtr lOrExp)
        : _lOrExp(std::move(lOrExp)) {}
    const auto &lOrExp() const { return _lOrExp; }

   private:
    AstNodePtr _lOrExp;
};

class AstLVal : public AstNodeBase {
    AST_NODE
   public:
    AstLVal(std::string id, AstNodePtrVector indices)
        : _id(id), _indices(std::move(indices)) {}
    const auto &id() const { return _id; }
    const auto &indices() const { return _indices; }

   private:
    std::string _id;
    AstNodePtrVector _indices;
};

class AstPrimaryExp : public AstNodeBase {
    AST_NODE
   public:
    AstPrimaryExp(AstNodePtr exp)
        : _exp(std::move(exp)) {}
    const auto &exp() const { return _exp; }

   private:
    AstNodePtr _exp;
};

class AstNumber : public AstNodeBase {
    AST_NODE
   public:
    AstNumber(int val)
        : _val(val) {}
    int val() const { return _val; }

   private:
    int _val;
};

class AstBinaryExp : public AstNodeBase {
    AST_NODE
   public:
    AstBinaryExp(AstNodePtr lhs, BinaryOp op, AstNodePtr rhs)
        : _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) {}
    AstBinaryExp(AstNodePtr lhs)
        : _lhs(std::move(lhs)), _op(BinaryOp::SINGLE) {}
    AstBinaryExp() = default;
    const auto &lhs() const { return _lhs; }
    const auto &rhs() const { return _rhs; }
    BinaryOp op() const { return _op; }

   private:
    AstNodePtr _lhs, _rhs;
    BinaryOp _op;
};

class AstUnaryExp : public AstNodeBase {
    AST_NODE
   public:
    AstUnaryExp(UnaryOp op, AstNodePtr exp)
        : _op(op), _exp(std::move(exp)) {}
    AstUnaryExp(AstNodePtr exp)
        : _op(UnaryOp::SINGLE), _exp(std::move(exp)) {}
    const auto &exp() const { return _exp; }
    UnaryOp op() const { return _op; }

   private:
    AstNodePtr _exp;
    UnaryOp _op;
};

class AstFuncRParams : public AstNodeBase {
    AST_NODE
   public:
    AstFuncRParams(AstNodePtrVector exps)
        : _exps(std::move(exps)) {}
    const auto &exps() const { return _exps; }

   private:
    AstNodePtrVector _exps;
};

class AstFuncCall : public AstNodeBase {
    AST_NODE
   public:
    AstFuncCall(std::string id, AstNodePtrVector params)
        : _id(id), _params(std::move(params)) {}
    const auto &id() const { return _id; }
    const auto &params() const { return _params; }

   private:
    std::string _id;
    AstNodePtrVector _params;
};