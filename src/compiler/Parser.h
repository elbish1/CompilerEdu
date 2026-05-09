#pragma once

#include "compiler/AST.h"
#include "compiler/Token.h"

namespace edu::compiler {

class Parser {
 public:
  explicit Parser(std::vector<Token> tokens) : toks_(std::move(tokens)) {}

  std::unique_ptr<ASTNode> ParseProgram();

  const std::vector<std::string>& Errors() const { return errors_; }

 private:
  const Token& Peek(size_t off = 0) const;
  bool Is(TokenType t, size_t off = 0) const;
  const Token& Consume();
  bool Match(TokenType t);
  bool Expect(TokenType t, const std::string& what);

  void ErrorHere(const std::string& msg);

  // Grammar:
  // program := (decl | stmt)* End
  // decl    := "int" Identifier ("=" expr)? ";"
  // stmt    := Identifier "=" expr ";" | "print" "(" expr ")" ";"
  // expr    := term (("+"|"-") term)*
  // term    := factor (("*"|"/") factor)*
  // factor  := Number | Identifier | "(" expr ")"

  std::unique_ptr<ASTNode> ParseDecl();
  std::unique_ptr<ASTNode> ParseStmt();
  std::unique_ptr<ASTNode> ParseExpr();
  std::unique_ptr<ASTNode> ParseTerm();
  std::unique_ptr<ASTNode> ParseFactor();

  std::vector<Token> toks_;
  size_t i_ = 0;
  std::vector<std::string> errors_;
};

}  // namespace edu::compiler

