#pragma once

#include "compiler/AST.h"
#include "compiler/Lexer.h"

namespace edu::compiler {

struct SemanticResult {
  std::vector<std::string> errors;
  std::unordered_map<std::string, SymbolInfo> symbols;  // declared symbols
};

class SemanticAnalyzer {
 public:
  SemanticResult Analyze(ASTNode* root, const std::unordered_map<std::string, SymbolInfo>& lexSymbols);

 private:
  void Error(const Token& t, const std::string& msg);
  void Visit(ASTNode* n, int depth);
  void VisitStmt(ASTNode* n, int depth);
  void VisitExpr(ASTNode* n, int depth);

  std::unordered_map<std::string, SymbolInfo> symbols_;
  std::vector<std::string> errors_;
};

}  // namespace edu::compiler

