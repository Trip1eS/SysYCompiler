#pragma once
#include "AstNodesVisitor.hpp"
#include "AstNodes.hpp"
#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

class IrGenerator : public AstNodesVisitor {
   public:
    IrGenerator(AstNodePtrVector compUnits)
        : _compUnits(std::move(compUnits)),
          _context(),
          _builder(_context) {}

    void codegen();

   public:  // visitor methods
    virtual void visit(const AstCompUnit&) override;
    virtual void visit(const AstDecl&) override;
    virtual void visit(const AstBType&) override;
    virtual void visit(const AstVarDecl&) override;
    virtual void visit(const AstVarDef&) override;
    virtual void visit(const AstInitVal&) override;
    virtual void visit(const AstFuncDef&) override;
    virtual void visit(const AstFuncType&) override;
    virtual void visit(const AstFuncFParams&) override;
    virtual void visit(const AstFuncFParam&) override;
    virtual void visit(const AstBlock&) override;
    virtual void visit(const AstBlockItem&) override;
    virtual void visit(const AstAssignStmt&) override;
    virtual void visit(const AstExpStmt&) override;
    virtual void visit(const AstBlockStmt&) override;
    virtual void visit(const AstIfStmt&) override;
    virtual void visit(const AstWhileStmt&) override;
    virtual void visit(const AstBreakStmt&) override;
    virtual void visit(const AstContinueStmt&) override;
    virtual void visit(const AstReturnStmt&) override;
    virtual void visit(const AstExp&) override;
    virtual void visit(const AstCond&) override;
    virtual void visit(const AstLVal&) override;
    virtual void visit(const AstPrimaryExp&) override;
    virtual void visit(const AstNumber&) override;
    virtual void visit(const AstBinaryExp&) override;
    virtual void visit(const AstUnaryExp&) override;
    virtual void visit(const AstFuncRParams&) override;
    virtual void visit(const AstFuncCall&) override;

   private:
    AstNodePtrVector _compUnits;
    llvm::LLVMContext _context;
    llvm::IRBuilder<> _builder;
    std::unique_ptr<llvm::Module> _module;
    std::map<std::string, llvm::AllocaInst*> _namedValues;
    std::unique_ptr<llvm::legacy::FunctionPassManager> _fpm;
};