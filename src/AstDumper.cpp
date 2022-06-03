#include "AstDumper.hpp"
#include "AstNodes.hpp"
#include "Logger.hpp"

void AstDumper::begin(const std::string& name) {
    output(name);
    _depth++;
}

void AstDumper::end() {
    _depth--;
}

void AstDumper::dumpAll(const AstNodePtrVector& nodes, const std::string& filePath) {
    std::ofstream of(filePath, std::ios::out);
    _os = &of;
    _depth = 0;
    for (auto& node : nodes) {
        dump(*node);
    }
    _os = nullptr;
}

void AstDumper::visit(const AstCompUnit& node) {
    begin("CompUnit");
    dump(*node.next());
    end();
}

void AstDumper::visit(const AstDecl& node) {
    begin("Decl");
    dump(*node.decl());
    end();
}

void AstDumper::visit(const AstBType& node) {
    begin("BType");
    output("Type: int");
    end();
}

void AstDumper::visit(const AstVarDecl& node) {
    begin("VarDecl");
    dump(*node.type());
    for (auto& varDef : node.varDefs()) {
        dump(*varDef);
    }
    end();
}

void AstDumper::visit(const AstVarDef& node) {
    begin("VarDef");
    output("ID: %s", node.id().c_str());
    if (!node.arrLens().empty()) {
        begin("ArrLens");
        for (auto& exp : node.arrLens()) {
            dump(*exp);
        }
        end();
    }
    dump(*node.initVal());
    end();
}

void AstDumper::visit(const AstInitVal& node) {
    begin("InitVal");
    if (node.exp()) {
        // single exp init
        dump(*node.exp());
    } else {
        for (auto& initVal : node.initVals()) {
            dump(*initVal);
        }
    }
    end();
}

void AstDumper::visit(const AstFuncDef& node) {
    begin("FuncDef");
    dump(*node.funcType());
    output("ID: %s", node.id().c_str());
    if (node.params()) dump(*node.params());
    dump(*node.block());
    end();
}

void AstDumper::visit(const AstFuncType& node) {
    begin("FuncType");
    switch (node.type()) {
        case FuncType::INT:
            output("INT: int");
            break;
        case FuncType::VOID:
            output("VOID: void");
            break;
    }
    end();
}

void AstDumper::visit(const AstFuncFParams& node) {
    begin("FuncFParams");
    for (auto& param : node.params()) {
        dump(*param);
    }
    end();
}

void AstDumper::visit(const AstFuncFParam& node) {
    begin("FuncFParam");
    dump(*node.type());
    output("ID: %s", node.id().c_str());
    end();
}

void AstDumper::visit(const AstBlock& node) {
    begin("Block");
    for (auto& item : node.items()) {
        dump(*item);
    }
    end();
}

void AstDumper::visit(const AstBlockItem& node) {
    begin("BlockItem");
    dump(*node.declOrStmt());
    end();
}

void AstDumper::visit(const AstAssignStmt& node) {
    begin("AssignStmt");
    dump(*node.lVal());
    dump(*node.exp());
    end();
}

void AstDumper::visit(const AstExpStmt& node) {
    begin("ExpStmt");
    dump(*node.exp());
    end();
}

void AstDumper::visit(const AstBlockStmt& node) {
    begin("BlockStmt");
    dump(*node.block());
    end();
}

void AstDumper::visit(const AstIfStmt& node) {
    begin("IfStmt");
    begin("Cond");
    dump(*node.cond());
    end();
    begin("Then");
    dump(*node.stmt());
    end();
    if (node.elseStmt()) {
        begin("Else");
        dump(*node.elseStmt());
        end();
    }
    end();
}

void AstDumper::visit(const AstWhileStmt& node) {
    begin("WhileStmt");
    begin("Cond");
    dump(*node.cond());
    end();
    begin("Body");
    dump(*node.stmt());
    end();
    end();
}

void AstDumper::visit(const AstBreakStmt& node) {
    begin("BreakStmt");
    end();
}

void AstDumper::visit(const AstContinueStmt& node) {
    begin("ContinueStmt");
    end();
}

void AstDumper::visit(const AstReturnStmt& node) {
    begin("ReturnStmt");
    if (node.exp()) {
        dump(*node.exp());
    }
    end();
}

void AstDumper::visit(const AstExp& node) {
    begin("Exp");
    dump(*node.addExp());
    end();
}

void AstDumper::visit(const AstCond& node) {
    begin("Cond");
    dump(*node.lOrExp());
    end();
}

void AstDumper::visit(const AstLVal& node) {
    begin("LVal");
    output("ID: %s", node.id().c_str());
    if (!node.indices().empty()) {
        begin("Indices");
        for (auto& index : node.indices()) {
            dump(*index);
        }
        end();
    }
    end();
}

void AstDumper::visit(const AstPrimaryExp& node) {
    begin("PrimaryExp");
    dump(*node.exp());
    end();
}

void AstDumper::visit(const AstNumber& node) {
    begin("Number");
    output("INTCON: %d", node.val());
    end();
}

void AstDumper::visit(const AstBinaryExp& node) {
    begin("BinaryExp");
    dump(*node.lhs());
    switch (node.op()) {
        case BinaryOp::PLUS:
            output("+");
            break;
        case BinaryOp::SUB:
            output("-");
            break;
        case BinaryOp::MUL:
            output("*");
            break;
        case BinaryOp::DIV:
            output("/");
            break;
        case BinaryOp::MOD:
            output("%");
            break;
        case BinaryOp::LESS:
            output("<");
            break;
        case BinaryOp::GREATER:
            output(">");
            break;
        case BinaryOp::LESSEQ:
            output("<=");
            break;
        case BinaryOp::GREATEREQ:
            output(">=");
            break;
        case BinaryOp::EQUAL:
            output("==");
            break;
        case BinaryOp::NEQUAL:
            output("!=");
            break;
        case BinaryOp::LOGICAND:
            output("&&");
            break;
        case BinaryOp::LOGICOR:
            output("||");
            break;
    }
    if (node.rhs()) dump(*node.rhs());
    end();
}

void AstDumper::visit(const AstUnaryExp& node) {
    begin("UnaryExp");
    switch (node.op()) {
        case UnaryOp::PLUS:
            output("+");
            break;
        case UnaryOp::MINUS:
            output("-");
            break;
        case UnaryOp::NOT:
            output("!");
            break;
    }
    dump(*node.exp());
    end();
}

void AstDumper::visit(const AstFuncRParams& node) {
    begin("FuncRParams");
    for (auto& param : node.exps()) {
        dump(*param);
    }
    end();
}

void AstDumper::visit(const AstFuncCall& node) {
    begin("FuncCall");
    output("ID: %s", node.id().c_str());
    begin("Params");
    for (auto& param : node.params()) {
        dump(*param);
    }
    end();
    end();
}