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
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <map>
#include <memory>

class IrGenerator : public AstNodesVisitor {
   public:
    explicit IrGenerator(AstNodePtrVector compUnits);

    void codegen();

    void printModule() const {
        _module->print(llvm::outs(), nullptr);
    }

    void output(const std::string& path, llvm::CodeGenFileType fileType);

    void addExternFunction(const char* name, llvm::Type* retType, const std::vector<llvm::Type*>& params);

   public:  // visitor methods
    void visit(const AstCompUnit&) override;
    void visit(const AstDecl&) override;
    void visit(const AstBType&) override;
    void visit(const AstVarDecl&) override;
    void visit(const AstVarDef&) override;
    void visit(const AstInitVal&) override;
    void visit(const AstFuncDef&) override;
    void visit(const AstFuncType&) override;
    void visit(const AstFuncFParams&) override;
    void visit(const AstFuncFParam&) override;
    void visit(const AstBlock&) override;
    void visit(const AstBlockItem&) override;
    void visit(const AstAssignStmt&) override;
    void visit(const AstExpStmt&) override;
    void visit(const AstBlockStmt&) override;
    void visit(const AstIfStmt&) override;
    void visit(const AstWhileStmt&) override;
    void visit(const AstBreakStmt&) override;
    void visit(const AstContinueStmt&) override;
    void visit(const AstReturnStmt&) override;
    void visit(const AstExp&) override;
    void visit(const AstCond&) override;
    void visit(const AstLVal&) override;
    void visit(const AstPrimaryExp&) override;
    void visit(const AstNumber&) override;
    void visit(const AstBinaryExp&) override;
    void visit(const AstUnaryExp&) override;
    void visit(const AstFuncRParams&) override;
    void visit(const AstFuncCall&) override;

   private:
    template <class AstT>
    llvm::Value* codegen(const AstT& node) {
        node.accept(*this);
        return _ret;
    }

    void ret(llvm::Value* val) { _ret = val; }
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func,
                                             const std::string& varName) const;

   private:
    AstNodePtrVector _compUnits;
    std::unique_ptr<llvm::LLVMContext> _context;
    std::unique_ptr<llvm::IRBuilder<>> _builder;
    std::unique_ptr<llvm::Module> _module;
    std::map<std::string, llvm::AllocaInst*> _namedValues;
    std::unique_ptr<llvm::legacy::FunctionPassManager> _fpm;
    llvm::Value* _ret;
    llvm::BasicBlock* _retBB;
    llvm::AllocaInst* _retAlloca;
};
