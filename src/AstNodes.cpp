#include "AstNodes.hpp"

/**
 * TODO: Convertion from AST Nodes to DOT language for better visualization of AST Tree
 */

// #define BEGIN_TO_DOT(label)                            \
//     int curIndex = index;                              \
//     do {                                               \
//         os << curIndex << " [label=\"" label "\"];\n"; \
//     } while (0)

// #define NODE_LABEL(index, label) os << index << " [label=\"" << label << "\"];\n"
// #define EDGE(from, to) os << from << " -- " << to << "\n"

// #define DOT_CHILD(next)         \
//     do {                        \
//         if (!next) break;       \
//         ++index;                \
//         EDGE(curIndex, index);  \
//         next->toDOT(os, index); \
//     } while (0)

// #define DOT_CHILD_LITERAL(str)  \
//     do {                        \
//         ++index;                \
//         EDGE(curIndex, index);  \
//         NODE_LABEL(index, str); \
//     } while (0)

// #define DOT_CHILD_VECTOR(vec)       \
//     do {                            \
//         if (vec.empty()) break;     \
//         for (auto& elem : vec) {    \
//             ++index;                \
//             EDGE(curIndex, index);  \
//             elem->toDOT(os, index); \
//         }                           \
//     } while (0)

// #define DOT_CHILD_VECTOR_LABEL(label, vec) \
//     do {                                   \
//         if (vec.empty()) break;            \
//         DOT_CHILD_LITERAL(label);          \
//         int pa = index;                    \
//         for (auto& elem : vec) {           \
//             ++index;                       \
//             EDGE(pa, index);               \
//             elem->toDOT(os, index);        \
//         }                                  \
//     } while (0)

// void AstCompUnit::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("CompUnit");
//     DOT_CHILD(_next);
// }

// void AstBType::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("BType");
//     DOT_CHILD_LITERAL("int");
// }

// void AstVarDecl::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("VarDecl");
//     DOT_CHILD(_type);
//     DOT_CHILD_VECTOR_LABEL("VarDefs", _varDefs);
// }

// void AstVarDef::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("VarDef");
//     DOT_CHILD_LITERAL(_id);
//     DOT_CHILD_VECTOR_LABEL("ArrLens", _arrLens);
//     DOT_CHILD(_initVal);
// }

// void AstInitVal::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("InitVal");
//     if (_exp)
//         DOT_CHILD(_exp);
//     else
//         DOT_CHILD_VECTOR_LABEL("InitVals", _initVals);
// }

// void AstFuncDef::toDOT(std::ostream& os, int& index) {
// }

// void AstFuncType::toDOT(std::ostream& os, int& index) {
// }

// void AstFuncFParams::toDOT(std::ostream& os, int& index) {
// }

// void AstBlock::toDOT(std::ostream& os, int& index) {
// }

// void AstBlockItem::toDOT(std::ostream& os, int& index) {
// }

// void AstStmt::toDOT(std::ostream& os, int& index) {
// }

// void AstExp::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("Exp");
//     DOT_CHILD(_addExp);
// }

// void AstCond::toDOT(std::ostream& os, int& index) {
// }

// void AstLVal::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("LVal");
//     DOT_CHILD_LITERAL(_id);
//     if (!_indices.empty()) DOT_CHILD_VECTOR_LABEL("Indices", _indices);
// }

// void AstNumber::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("Number");
//     DOT_CHILD_LITERAL(_val);
// }

// void AstBinaryExp::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("BinaryExp");
//     DOT_CHILD(_lhs);
//     switch (_op) {
//         case BinaryOp::ADD:
//             DOT_CHILD_LITERAL("+");
//             break;
//         case BinaryOp::DIV:
//             DOT_CHILD_LITERAL("/");
//             break;
//         case BinaryOp::EQUAL:
//             DOT_CHILD_LITERAL("==");
//             break;
//         case BinaryOp::GREATER:
//             DOT_CHILD_LITERAL(">");
//             break;
//         case BinaryOp::GREATEREQ:
//             DOT_CHILD_LITERAL(">=");
//             break;
//         case BinaryOp::LESS:
//             DOT_CHILD_LITERAL("<");
//             break;
//         case BinaryOp::LESSEQ:
//             DOT_CHILD_LITERAL("<=");
//             break;
//         case BinaryOp::LOGICAND:
//             DOT_CHILD_LITERAL("&&");
//             break;
//         case BinaryOp::LOGICOR:
//             DOT_CHILD_LITERAL("||");
//             break;
//         case BinaryOp::MOD:
//             DOT_CHILD_LITERAL("%");
//             break;
//         case BinaryOp::MUL:
//             DOT_CHILD_LITERAL("*");
//             break;
//         case BinaryOp::NEQUAL:
//             DOT_CHILD_LITERAL("!=");
//             break;
//         case BinaryOp::SUB:
//             DOT_CHILD_LITERAL("-");
//             break;
//     }
//     DOT_CHILD(_rhs);
// }

// void AstUnaryExp::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("UnaryExp");
//     switch (_op) {
//         case UnaryOp::PLUS:
//             DOT_CHILD_LITERAL("+");
//             break;
//         case UnaryOp::NOT:
//             DOT_CHILD_LITERAL("!");
//             break;
//         case UnaryOp::MINUS:
//             DOT_CHILD_LITERAL("-");
//             break;
//     }
//     DOT_CHILD(_exp);
// }

// void AstFuncRParams::toDOT(std::ostream& os, int& index) {
// }

// void AstFuncCall::toDOT(std::ostream& os, int& index) {
//     BEGIN_TO_DOT("FuncCall");
//     DOT_CHILD_LITERAL(_id);
//     DOT_CHILD_VECTOR_LABEL("Params", _params);
// }
