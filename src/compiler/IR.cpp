#include "compiler/IR.h"

namespace edu::compiler {

std::string IRGen::NewTemp() {
  return "t" + std::to_string(++tempId_);
}

std::string IRGen::GenExpr(const ASTNode* n) {
  if (!n) return "0";
  switch (n->kind) {
    case NodeKind::Number:
      return n->token.lexeme;
    case NodeKind::Ident:
      return n->token.lexeme;
    case NodeKind::BinaryOp: {
      std::string a = GenExpr(n->children[0].get());
      std::string b = GenExpr(n->children[1].get());
      std::string t = NewTemp();
      IROp op = IROp::Add;
      switch (n->token.type) {
        case TokenType::Plus: op = IROp::Add; break;
        case TokenType::Minus: op = IROp::Sub; break;
        case TokenType::Star: op = IROp::Mul; break;
        case TokenType::Slash: op = IROp::Div; break;
        default: break;
      }
      out_.push_back(IRInstr{op, t, a, b});
      return t;
    }
    default:
      return "0";
  }
}

void IRGen::GenStmt(const ASTNode* n) {
  if (!n) return;
  if (n->kind == NodeKind::Decl) {
    // Decl token is identifier
    if (!n->children.empty()) {
      std::string rhs = GenExpr(n->children[0].get());
      out_.push_back(IRInstr{IROp::Mov, n->token.lexeme, rhs, ""});
    }
    return;
  }
  if (n->kind == NodeKind::Assign) {
    std::string rhs = GenExpr(n->children[0].get());
    out_.push_back(IRInstr{IROp::Mov, n->token.lexeme, rhs, ""});
    return;
  }
  if (n->kind == NodeKind::Print) {
    std::string v = GenExpr(n->children[0].get());
    out_.push_back(IRInstr{IROp::Print, "", v, ""});
    return;
  }
}

std::vector<IRInstr> IRGen::Generate(const ASTNode* root) {
  out_.clear();
  tempId_ = 0;
  if (!root) return out_;
  for (const auto& st : root->children) GenStmt(st.get());
  return out_;
}

}  // namespace edu::compiler

