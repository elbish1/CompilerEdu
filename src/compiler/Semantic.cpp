#include "compiler/Semantic.h"

namespace edu::compiler {

void SemanticAnalyzer::Error(const Token& t, const std::string& msg) {
  std::ostringstream oss;
  oss << "Semantic error at line " << t.pos.line << ", col " << t.pos.col << ": " << msg;
  errors_.push_back(oss.str());
}

SemanticResult SemanticAnalyzer::Analyze(ASTNode* root, const std::unordered_map<std::string, SymbolInfo>& lexSymbols) {
  symbols_ = lexSymbols;  // identifiers seen by lexer
  errors_.clear();
  Visit(root);

  SemanticResult r;
  r.errors = errors_;
  r.symbols = symbols_;
  return r;
}

void SemanticAnalyzer::Visit(ASTNode* n) {
  if (!n) return;
  switch (n->kind) {
    case NodeKind::Program:
      for (auto& ch : n->children) Visit(ch.get());
      break;
    case NodeKind::Decl:
    case NodeKind::Assign:
    case NodeKind::Print:
      VisitStmt(n);
      break;
    case NodeKind::BinaryOp:
    case NodeKind::Ident:
    case NodeKind::Number:
      VisitExpr(n);
      break;
  }
}

void SemanticAnalyzer::VisitStmt(ASTNode* n) {
  if (!n) return;
  if (n->kind == NodeKind::Decl) {
    const std::string& name = n->token.lexeme;
    auto it = symbols_.find(name);
    if (it == symbols_.end()) {
      symbols_.insert({name, SymbolInfo{name, "int", true}});
    } else {
      if (it->second.declared) {
        Error(n->token, "Redeclaration of variable '" + name + "'");
      }
      it->second.declared = true;
      it->second.type = "int";
    }

    n->type = {"int", true};
    if (!n->children.empty()) {
      VisitExpr(n->children[0].get());
      if (n->children[0]->type.name != "int") {
        Error(n->token, "Type mismatch in initialization of '" + name + "'");
        n->type.isValid = false;
      }
    }
    return;
  }

  if (n->kind == NodeKind::Assign) {
    const std::string& name = n->token.lexeme;
    auto it = symbols_.find(name);
    if (it == symbols_.end() || !it->second.declared) {
      Error(n->token, "Assignment to undeclared variable '" + name + "'");
      symbols_[name] = SymbolInfo{name, "int", false};
    }

    VisitExpr(n->children[0].get());
    if (n->children[0]->type.name != "int") {
      Error(n->token, "Type mismatch in assignment to '" + name + "'");
    }
    n->type = {"int", true};
    return;
  }

  if (n->kind == NodeKind::Print) {
    VisitExpr(n->children[0].get());
    n->type = {"int", true};
    return;
  }
}

void SemanticAnalyzer::VisitExpr(ASTNode* n) {
  if (!n) return;
  if (n->kind == NodeKind::Number) {
    n->type = {"int", true};
    n->isConst = true;
    try {
      n->constValue = std::stoll(n->token.lexeme);
    } catch (...) {
      n->constValue = 0;
    }
    return;
  }
  if (n->kind == NodeKind::Ident) {
    const std::string& name = n->token.lexeme;
    auto it = symbols_.find(name);
    if (it == symbols_.end() || !it->second.declared) {
      Error(n->token, "Use of undeclared variable '" + name + "'");
      symbols_[name] = SymbolInfo{name, "int", false};
      n->type = {"int", false};
    } else {
      n->type = {it->second.type, true};
    }
    n->isConst = false;
    n->constValue = edu::Nullopt;
    return;
  }
  if (n->kind == NodeKind::BinaryOp) {
    VisitExpr(n->children[0].get());
    VisitExpr(n->children[1].get());
    n->type = {"int", n->children[0]->type.isValid && n->children[1]->type.isValid};

    // Constant propagation for the annotated tree
    if (n->children[0]->isConst && n->children[1]->isConst &&
        n->children[0]->constValue && n->children[1]->constValue) {
      int64_t a = *n->children[0]->constValue;
      int64_t b = *n->children[1]->constValue;
      int64_t r = 0;
      bool ok = true;
      switch (n->token.type) {
        case TokenType::Plus: r = a + b; break;
        case TokenType::Minus: r = a - b; break;
        case TokenType::Star: r = a * b; break;
        case TokenType::Slash:
          if (b == 0) { ok = false; Error(n->token, "Division by zero in constant expression"); }
          else r = a / b;
          break;
        default: ok = false; break;
      }
      if (ok) {
        n->isConst = true;
        n->constValue = r;
      }
    }
    return;
  }
}

}  // namespace edu::compiler

