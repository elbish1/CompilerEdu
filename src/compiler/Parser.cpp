#include "compiler/Parser.h"

namespace edu::compiler {

const Token& Parser::Peek(size_t off) const {
  size_t idx = i_ + off;
  if (idx >= toks_.size()) return toks_.back();
  return toks_[idx];
}

bool Parser::Is(TokenType t, size_t off) const {
  return Peek(off).type == t;
}

const Token& Parser::Consume() {
  const Token& t = Peek();
  if (i_ < toks_.size()) ++i_;
  return t;
}

bool Parser::Match(TokenType t) {
  if (Is(t)) { Consume(); return true; }
  return false;
}

void Parser::ErrorHere(const std::string& msg) {
  const Token& t = Peek();
  std::ostringstream oss;
  oss << "Parse error at line " << t.pos.line << ", col " << t.pos.col << ": " << msg;
  errors_.push_back(oss.str());
}

bool Parser::Expect(TokenType t, const std::string& what) {
  if (Is(t)) { Consume(); return true; }
  std::ostringstream oss;
  oss << "Expected " << what << " but found '" << Peek().lexeme << "' (" << ToString(Peek().type) << ")";
  ErrorHere(oss.str());
  return false;
}

std::unique_ptr<ASTNode> Parser::ParseProgram() {
  auto prog = MakeNode(NodeKind::Program);
  while (!Is(TokenType::End)) {
    if (Is(TokenType::KwInt)) {
      auto d = ParseDecl();
      if (d) prog->children.push_back(std::move(d));
      else Consume();  // recover
      continue;
    }
    auto s = ParseStmt();
    if (s) prog->children.push_back(std::move(s));
    else Consume();  // recover
  }
  return prog;
}

std::unique_ptr<ASTNode> Parser::ParseDecl() {
  // "int" Identifier ("=" expr)? ";"
  Token kw = Consume();  // int
  (void)kw;

  if (!Is(TokenType::Identifier)) {
    ErrorHere("Declaration must name an identifier");
    return nullptr;
  }
  Token id = Consume();

  auto decl = MakeNode(NodeKind::Decl, id);
  if (Match(TokenType::Assign)) {
    auto e = ParseExpr();
    if (!e) return nullptr;
    decl->children.push_back(std::move(e));
  }
  if (!Expect(TokenType::Semicolon, "';'")) return nullptr;
  return decl;
}

std::unique_ptr<ASTNode> Parser::ParseStmt() {
  if (Is(TokenType::Identifier)) {
    // Identifier "=" expr ";"
    Token id = Consume();
    if (!Expect(TokenType::Assign, "'='")) return nullptr;
    auto e = ParseExpr();
    if (!e) return nullptr;
    if (!Expect(TokenType::Semicolon, "';'")) return nullptr;

    auto asg = MakeNode(NodeKind::Assign, id);
    asg->children.push_back(std::move(e));
    return asg;
  }
  if (Is(TokenType::KwPrint)) {
    Token kw = Consume();
    auto p = MakeNode(NodeKind::Print, kw);
    if (!Expect(TokenType::LParen, "'('")) return nullptr;
    auto e = ParseExpr();
    if (!e) return nullptr;
    if (!Expect(TokenType::RParen, "')'")) return nullptr;
    if (!Expect(TokenType::Semicolon, "';'")) return nullptr;
    p->children.push_back(std::move(e));
    return p;
  }

  ErrorHere("Unknown statement (expected assignment or print)");
  return nullptr;
}

std::unique_ptr<ASTNode> Parser::ParseExpr() {
  auto left = ParseTerm();
  if (!left) return nullptr;
  while (Is(TokenType::Plus) || Is(TokenType::Minus)) {
    Token op = Consume();
    auto right = ParseTerm();
    if (!right) return nullptr;
    auto bin = MakeNode(NodeKind::BinaryOp, op);
    bin->children.push_back(std::move(left));
    bin->children.push_back(std::move(right));
    left = std::move(bin);
  }
  return left;
}

std::unique_ptr<ASTNode> Parser::ParseTerm() {
  auto left = ParseFactor();
  if (!left) return nullptr;
  while (Is(TokenType::Star) || Is(TokenType::Slash)) {
    Token op = Consume();
    auto right = ParseFactor();
    if (!right) return nullptr;
    auto bin = MakeNode(NodeKind::BinaryOp, op);
    bin->children.push_back(std::move(left));
    bin->children.push_back(std::move(right));
    left = std::move(bin);
  }
  return left;
}

std::unique_ptr<ASTNode> Parser::ParseFactor() {
  if (Is(TokenType::Number)) {
    Token n = Consume();
    return MakeNode(NodeKind::Number, n);
  }
  if (Is(TokenType::Identifier)) {
    Token id = Consume();
    return MakeNode(NodeKind::Ident, id);
  }
  if (Match(TokenType::LParen)) {
    auto e = ParseExpr();
    if (!e) return nullptr;
    if (!Expect(TokenType::RParen, "')'")) return nullptr;
    return e;
  }
  ErrorHere("Expected number, identifier, or parenthesized expression");
  return nullptr;
}

}  // namespace edu::compiler

