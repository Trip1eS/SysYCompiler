#include "IrGenerator.hpp"

IrGenerator::IrGenerator(AstNodePtrVector compUnits)
    : _compUnits(std::move(compUnits)),
      _context(new llvm::LLVMContext),
      _builder(new llvm::IRBuilder<>(*_context)),
      _module(new llvm::Module("SysY", *_context)),
      _fpm(new llvm::legacy::FunctionPassManager(_module.get())),
      _ret(nullptr) {
}

llvm::AllocaInst* IrGenerator::createEntryBlockAlloca(llvm::Function* func, const std::string& varName = "") const {
    llvm::IRBuilder<> tmpB(&func->getEntryBlock(),
                           func->getEntryBlock().begin());
    return tmpB.CreateAlloca(llvm::Type::getInt32Ty(*_context), nullptr, varName);
}

void IrGenerator::codegen() {
    for (auto& compUnit : _compUnits) {
        auto value = codegen(*compUnit);
    }
}

#define RETURN(x) return ret(x)

void IrGenerator::visit(const AstCompUnit& node) {
    RETURN(codegen(*node.next()));
}

void IrGenerator::visit(const AstDecl& node) {
    RETURN(codegen(*node.decl()));
}

void IrGenerator::visit(const AstBType& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstVarDecl& node) {
    auto func = _builder->GetInsertBlock()->getParent();
    llvm::Value* lastStore = nullptr;
    for (auto& def : node.varDefs()) {
        llvm::Value* initVal;
        if (def->initVal() != nullptr) {
            initVal = codegen(*def->initVal()->exp());
        } else {
            initVal = llvm::ConstantInt::get(*_context, llvm::APInt(32, 0, true));
        }
        auto allocaInst = createEntryBlockAlloca(func, def->id());
        lastStore = _builder->CreateStore(initVal, allocaInst);
        _namedValues[def->id()] = allocaInst;
    }
    RETURN(lastStore);
}

void IrGenerator::visit(const AstVarDef& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstInitVal& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstFuncDef& node) {
    std::vector<llvm::Type*> params(node.params() == nullptr ? 0 : node.params()->params().size(),
                                    llvm::Type::getInt32Ty(*_context));

    auto retType = node.funcType()->type() == FuncType::INT
                       ? llvm::Type::getInt32Ty(*_context)
                       : llvm::Type::getVoidTy(*_context);
    auto funcType = llvm::FunctionType::get(retType, params, false);
    auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.id(), _module.get());
    size_t idx = 0;
    for (auto& arg : func->args()) {
        arg.setName(node.params()->params()[idx++]->id());
    }

    auto bb = llvm::BasicBlock::Create(*_context, "entry", func);
    _builder->SetInsertPoint(bb);
    _retBB = llvm::BasicBlock::Create(*_context, "exit");

    _namedValues.clear();
    if (node.funcType()->type() == FuncType::INT) {
        _retAlloca = createEntryBlockAlloca(func);
    } else {
        _retAlloca = nullptr;
    }
    for (auto& arg : func->args()) {
        auto allocaInst = createEntryBlockAlloca(func, static_cast<std::string>(arg.getName()));
        _builder->CreateStore(&arg, allocaInst);
        _namedValues[static_cast<std::string>(arg.getName())] = allocaInst;
    }

    codegen(*node.block());

    _builder->CreateBr(_retBB);
    func->getBasicBlockList().push_back(_retBB);
    _builder->SetInsertPoint(_retBB);
    auto retV = _builder->CreateLoad(_retAlloca->getAllocatedType(), _retAlloca);
    _builder->CreateRet(retV);

    verifyFunction(*func);
    RETURN(func);

    // func->eraseFromParent();
    // RETURN(nullptr);
}

void IrGenerator::visit(const AstFuncType& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstFuncFParams& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstFuncFParam& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstBlock& node) {
    llvm::Value* retVal = nullptr;
    for (auto& item : node.items()) {
        retVal = codegen(*item);
    }
    RETURN(retVal);
}

void IrGenerator::visit(const AstBlockItem& node) {
    RETURN(codegen(*node.declOrStmt()));
}

void IrGenerator::visit(const AstAssignStmt& node) {
    auto val = codegen(*node.exp());
    auto variable = _namedValues[node.lVal()->id()];
    if (!variable) throw std::runtime_error("unknown variable name");
    auto assign = _builder->CreateStore(val, variable);
    RETURN(assign);
}

void IrGenerator::visit(const AstExpStmt& node) {
    RETURN(codegen(*node.exp()));
}

void IrGenerator::visit(const AstBlockStmt& node) {
    auto oldBindings = _namedValues;
    auto block = codegen(*node.block());
    _namedValues = oldBindings;
    RETURN(block);
}

void IrGenerator::visit(const AstIfStmt& node) {
    auto condV = codegen(*node.cond());
    condV = _builder->CreateFCmpONE(condV, llvm::ConstantInt::get(*_context, llvm::APInt(32, 0, true)), "ifcond");
    auto func = _builder->GetInsertBlock()->getParent();

    auto thenBB = llvm::BasicBlock::Create(*_context, "then", func);
    auto elseBB = llvm::BasicBlock::Create(*_context, "else");
    auto mergeBB = llvm::BasicBlock::Create(*_context, "ifcont");

    _builder->CreateCondBr(condV, thenBB, elseBB);

    _builder->SetInsertPoint(thenBB);
    auto thenV = codegen(*node.stmt());
    _builder->CreateBr(mergeBB);
    thenBB = _builder->GetInsertBlock();

    func->getBasicBlockList().push_back(elseBB);
    _builder->SetInsertPoint(elseBB);
    auto elseV = codegen(*node.elseStmt());
    _builder->CreateBr(mergeBB);
    elseBB = _builder->GetInsertBlock();

    func->getBasicBlockList().push_back(mergeBB);
    _builder->SetInsertPoint(mergeBB);
    RETURN(mergeBB);
}

void IrGenerator::visit(const AstWhileStmt& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstBreakStmt& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstContinueStmt& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstReturnStmt& node) {
    if (node.exp() == nullptr) {
        // ret void
        _builder->CreateBr(_retBB);
    } else {
        auto retV = codegen(*node.exp());
        _builder->CreateStore(retV, _retAlloca);
        _builder->CreateBr(_retBB);
    }
}

void IrGenerator::visit(const AstExp& node) {
    RETURN(codegen(*node.addExp()));
}

void IrGenerator::visit(const AstCond& node) {
    RETURN(codegen(*node.lOrExp()));
}

void IrGenerator::visit(const AstLVal& node) {
    auto a = _namedValues[node.id()];
    if (!a) throw std::runtime_error("unknown variable name");
    RETURN(_builder->CreateLoad(a->getAllocatedType(), a, node.id().c_str()));
}

void IrGenerator::visit(const AstPrimaryExp& node) {
    RETURN(codegen(*node.exp()));
}

void IrGenerator::visit(const AstNumber& node) {
    RETURN(llvm::ConstantInt::get(*_context, llvm::APInt(32, node.val(), true)));
}

void IrGenerator::visit(const AstBinaryExp& node) {
    auto lhs = codegen(*node.lhs());
    if (node.op() == BinaryOp::SINGLE) {
        RETURN(lhs);
    }
    auto rhs = codegen(*node.rhs());
    switch (node.op()) {
        case BinaryOp::ADD:
            RETURN(_builder->CreateAdd(lhs, rhs));
        case BinaryOp::SUB:
            RETURN(_builder->CreateSub(lhs, rhs));
        case BinaryOp::MUL:
            RETURN(_builder->CreateMul(lhs, rhs));
        case BinaryOp::DIV:
            RETURN(_builder->CreateSDiv(lhs, rhs));
        case BinaryOp::MOD:
            RETURN(_builder->CreateSRem(lhs, rhs));
        case BinaryOp::LESS:
            RETURN(_builder->CreateICmpSLT(lhs, rhs));
        case BinaryOp::GREATER:
            RETURN(_builder->CreateICmpSGT(lhs, rhs));
        case BinaryOp::LESSEQ:
            RETURN(_builder->CreateICmpSLE(lhs, rhs));
        case BinaryOp::GREATEREQ:
            RETURN(_builder->CreateICmpSGE(lhs, rhs));
        case BinaryOp::EQUAL:
            RETURN(_builder->CreateICmpEQ(lhs, rhs));
        case BinaryOp::NEQUAL:
            RETURN(_builder->CreateICmpNE(lhs, rhs));
        case BinaryOp::LOGICAND:
            RETURN(_builder->CreateLogicalAnd(lhs, rhs));
        case BinaryOp::LOGICOR:
            RETURN(_builder->CreateLogicalOr(lhs, rhs));
        case BinaryOp::SINGLE:
            break;
    }
}

void IrGenerator::visit(const AstUnaryExp& node) {
    auto exp = codegen(*node.exp());
    switch (node.op()) {
        case UnaryOp::PLUS:
            // no effect
            RETURN(exp);
        case UnaryOp::MINUS:
            RETURN(_builder->CreateMul(exp, llvm::ConstantInt::get(*_context, llvm::APInt(32, -1, true))));
        case UnaryOp::NOT:
            RETURN(_builder->CreateNot(exp));
        case UnaryOp::SINGLE:
            // func call / primary exp
            RETURN(exp);
    }
}

void IrGenerator::visit(const AstFuncRParams& node) {
    RETURN(nullptr);
}

void IrGenerator::visit(const AstFuncCall& node) {
    auto func = _module->getFunction(node.id());
    if (!func) throw std::runtime_error("Unknown function reference");
    if (node.params().size() != func->arg_size()) throw std::runtime_error("Incorrent arguments passed");

    std::vector<llvm::Value*> argsVals;
    for (auto& param : node.params()) {
        argsVals.push_back(codegen(*param));
    }
    return ret(_builder->CreateCall(func, argsVals, "calltmp"));
}
