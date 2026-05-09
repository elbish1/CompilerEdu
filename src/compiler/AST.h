#pragma once

#include "compiler/Token.h"

namespace edu::compiler {

enum class NodeKind {
  Program,
  Decl,       // int x (= expr)? ;
  Assign,     // x = expr ;
  Print,      // print(expr) ;
  Number,
  Ident,
  BinaryOp,   // + - * /
};

inline const char* ToString(NodeKind k) {
  switch (k) {
    case NodeKind::Program: return "Program";
    case NodeKind::Decl: return "Decl";
    case NodeKind::Assign: return "Assign";
    case NodeKind::Print: return "Print";
    case NodeKind::Number: return "Number";
    case NodeKind::Ident: return "Ident";
    case NodeKind::BinaryOp: return "BinaryOp";
  }
  return "Unknown";
}

struct TypeInfo {
  std::string name = "int";   // this project uses only "int"
  bool isValid = true;
};

struct ASTNode {
  NodeKind kind = NodeKind::Program;
  Token token{};  // operator token, identifier token, or number token (when relevant)

  // Semantic annotation (filled by semantic analysis)
  TypeInfo type{};
  bool isConst = false;
  edu::Optional<int64_t> constValue;

  std::vector<std::unique_ptr<ASTNode>> children;
};

inline std::unique_ptr<ASTNode> MakeNode(NodeKind k, Token t = {}) {
  auto n = std::make_unique<ASTNode>();
  n->kind = k;
  n->token = std::move(t);
  return n;
}

inline void PrintTree(const ASTNode* n, const std::string& indent = "", bool last = true) {
  if (!n) return;
  std::cout << indent << (last ? "└─" : "├─") << ToString(n->kind);
  if (!n->token.lexeme.empty()) std::cout << " [" << n->token.lexeme << "]";
  if (!n->type.name.empty()) std::cout << " : " << n->type.name;
  if (n->isConst && n->constValue) std::cout << " (const=" << *n->constValue << ")";
  std::cout << "\n";

  std::string nextIndent = indent + (last ? "  " : "│ ");
  for (size_t i = 0; i < n->children.size(); ++i) {
    PrintTree(n->children[i].get(), nextIndent, i + 1 == n->children.size());
  }
}

}  // namespace edu::compiler

