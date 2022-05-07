#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <variant>

enum class FuncType {
    VOID,
    INT
};

enum class BType {
    INT
};

enum class BinaryOp {
    ADD,
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

class AstNodeBase {
   public:
    virtual ~AstNodeBase() = default;
};

using AstNodePtr = std::unique_ptr<AstNodeBase>;
using AstNodePtrVector = std::vector<AstNodePtr>;

class AstCompUnit : public AstNodeBase {
   public:
    AstCompUnit(AstNodePtr next) : _next(std::move(next)) {}

   private:
    AstNodePtr _next;
};

class AstDecl : public AstNodeBase {
   public:
    AstDecl(AstNodePtr decl)
        : _decl(std::move(decl)) {}

   private:
    AstNodePtr _decl;
};

class AstConstDecl : public AstNodeBase {};

class AstBType : public AstNodeBase {
   public:
    AstBType(BType type)
        : _type(type) {}

   private:
    BType _type;
};

class AstConstDef : public AstNodeBase {};
class AstConstInitVal : public AstNodeBase {};

class AstVarDecl : public AstNodeBase {
   public:
    AstVarDecl(AstNodePtr type, AstNodePtrVector varDefs)
        : _type(std::move(type)), _varDefs(std::move(varDefs)) {}

   private:
    AstNodePtr _type;
    AstNodePtrVector _varDefs;
};

class AstVarDef : public AstNodeBase {
   public:
    AstVarDef(std::string id, AstNodePtrVector arrLens, AstNodePtr initVal)
        : _id(id), _arrLens(std::move(arrLens)), _initVal(std::move(initVal)) {}

   private:
    std::string _id;
    AstNodePtrVector _arrLens;
    AstNodePtr _initVal;
};

class AstInitVal : public AstNodeBase {
   public:
    AstInitVal(AstNodePtrVector initVals)
        : _initVals(std::move(initVals)) {}
    AstInitVal(AstNodePtr exp)
        : _exp(std::move(exp)) {}

   private:
    AstNodePtrVector _initVals;
    AstNodePtr _exp;
};

class AstFuncDef : public AstNodeBase {
   public:
    AstFuncDef(AstNodePtr funcType, std::string id, AstNodePtr params, AstNodePtr block)
        : _funcType(std::move(funcType)), _id(id), _params(std::move(params)), _block(std::move(block)) {}

   private:
    AstNodePtr _funcType, _params, _block;
    std::string _id;
};

class AstFuncType : public AstNodeBase {
   public:
    AstFuncType(FuncType type)
        : _type(type) {}

   private:
    FuncType _type;
};

class AstFuncFParams : public AstNodeBase {
   public:
    AstFuncFParams(AstNodePtrVector params)
        : _params(std::move(params)) {}

   private:
    AstNodePtrVector _params;
};

class AstFuncFParam : public AstNodeBase {
   public:
    AstFuncFParam(AstNodePtr type, std::string id)
        : _type(std::move(type)), _id(id) {}

   private:
    AstNodePtr _type;
    std::string _id;
};

class AstBlock : public AstNodeBase {
   public:
    AstBlock(AstNodePtrVector items)
        : _items(std::move(items)) {}

   private:
    AstNodePtrVector _items;
};

class AstBlockItem : public AstNodeBase {
   public:
    AstBlockItem(AstNodePtr declOrStmt)
        : _declOrStmt(std::move(declOrStmt)) {}

   private:
    AstNodePtr _declOrStmt;
};

class AstStmt : public AstNodeBase {};

class AstAssignStmt : public AstStmt {
   public:
    AstAssignStmt(AstNodePtr lVal, AstNodePtr exp)
        : _lVal(std::move(lVal)), _exp(std::move(exp)) {}

   private:
    AstNodePtr _lVal, _exp;
};

class AstExpStmt : public AstStmt {
   public:
    AstExpStmt(AstNodePtr exp = nullptr)
        : _exp(std::move(exp)) {}

   private:
    AstNodePtr _exp;
};

class AstBlockStmt : public AstStmt {
   public:
    AstBlockStmt(AstNodePtr block)
        : _block(std::move(block)) {}

   private:
    AstNodePtr _block;
};

class AstIfStmt : public AstStmt {
   public:
    AstIfStmt(AstNodePtr cond, AstNodePtr stmt, AstNodePtr elseStmt = nullptr)
        : _cond(std::move(cond)), _stmt(std::move(stmt)), _elseStmt(std::move(elseStmt)) {}

   private:
    AstNodePtr _cond, _stmt, _elseStmt;
};

class AstWhileStmt : public AstStmt {
   public:
    AstWhileStmt(AstNodePtr cond, AstNodePtr stmt)
        : _cond(std::move(cond)), _stmt(std::move(stmt)) {}

   private:
    AstNodePtr _cond, _stmt;
};

class AstBreakStmt : public AstStmt {};
class AstContinueStmt : public AstStmt {};
class AstReturnStmt : public AstStmt {
   public:
    AstReturnStmt(AstNodePtr exp = nullptr)
        : _exp(std::move(exp)) {}

   private:
    AstNodePtr _exp;
};

class AstExp : public AstNodeBase {
   public:
    AstExp(AstNodePtr addExp)
        : _addExp(std::move(addExp)) {}

   private:
    AstNodePtr _addExp;
};

class AstCond : public AstNodeBase {
   public:
    AstCond(AstNodePtr lOrExp)
        : _lOrExp(std::move(lOrExp)) {}

   private:
    AstNodePtr _lOrExp;
};

class AstLVal : public AstNodeBase {
   public:
    AstLVal(std::string id, AstNodePtrVector indices)
        : _id(id), _indices(std::move(indices)) {}

   private:
    std::string _id;
    AstNodePtrVector _indices;
};

class AstPrimaryExp : public AstNodeBase {
   public:
    AstPrimaryExp(AstNodePtr exp)
        : _exp(std::move(exp)) {}

   private:
    AstNodePtr _exp;
};

class AstNumber : public AstNodeBase {
   public:
    AstNumber(int val)
        : _val(val) {}

   private:
    int _val;
};

class AstBinaryExp : public AstNodeBase {
   public:
    AstBinaryExp(AstNodePtr lhs, BinaryOp op, AstNodePtr rhs)
        : _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) {}
    AstBinaryExp(AstNodePtr lhs)
        : _lhs(std::move(lhs)), _op(BinaryOp::SINGLE) {}
    AstBinaryExp() = default;

    // void setLhs(AstNodePtr lhs) { _lhs = std::move(lhs); }
    // void setRhs(AstNodePtr rhs) { _rhs = std::move(rhs); }
    // void setOp(BinaryOp op) { _op = op; }

   private:
    AstNodePtr _lhs, _rhs;
    BinaryOp _op;
};

class AstUnaryExp : public AstNodeBase {
   public:
    AstUnaryExp(UnaryOp op, AstNodePtr exp)
        : _op(op), _exp(std::move(exp)) {}
    AstUnaryExp(AstNodePtr exp)
        : _op(UnaryOp::SINGLE), _exp(std::move(exp)) {}

   private:
    AstNodePtr _exp;
    UnaryOp _op;
};

class AstFuncRParams : public AstNodeBase {
   public:
    AstFuncRParams(AstNodePtrVector exps)
        : _exps(std::move(exps)) {}

   private:
    AstNodePtrVector _exps;
};

class AstFuncCall : public AstNodeBase {
   public:
    AstFuncCall(std::string id, AstNodePtrVector params)
        : _id(id), _params(std::move(params)) {}

   private:
    std::string _id;
    AstNodePtrVector _params;
};