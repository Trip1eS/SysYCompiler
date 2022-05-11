#pragma once
#include <memory>

#define DECL_NODE(name) \
    class name;         \
    using name##Ptr = std::unique_ptr<name>;

DECL_NODE(AstNodeBase)
DECL_NODE(AstCompUnit)
DECL_NODE(AstDecl)
DECL_NODE(AstBType)
DECL_NODE(AstVarDecl)
DECL_NODE(AstVarDef)
DECL_NODE(AstInitVal)
DECL_NODE(AstFuncDef)
DECL_NODE(AstFuncType)
DECL_NODE(AstFuncFParams)
DECL_NODE(AstFuncFParam)
DECL_NODE(AstBlock)
DECL_NODE(AstBlockItem)
DECL_NODE(AstAssignStmt)
DECL_NODE(AstExpStmt)
DECL_NODE(AstBlockStmt)
DECL_NODE(AstIfStmt)
DECL_NODE(AstWhileStmt)
DECL_NODE(AstBreakStmt)
DECL_NODE(AstContinueStmt)
DECL_NODE(AstReturnStmt)
DECL_NODE(AstExp)
DECL_NODE(AstCond)
DECL_NODE(AstLVal)
DECL_NODE(AstPrimaryExp)
DECL_NODE(AstNumber)
DECL_NODE(AstBinaryExp)
DECL_NODE(AstUnaryExp)
DECL_NODE(AstFuncRParams)
DECL_NODE(AstFuncCall)

using AstNodePtr = std::unique_ptr<AstNodeBase>;