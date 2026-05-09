#include "compiler/Lexer.h"

namespace edu::compiler {

char Lexer::Peek(size_t off) const {
  size_t idx = i_ + off;
  if (idx >= input_.size()) return '\0';
  return input_[idx];
}

bool Lexer::Eof() const {
  return i_ >= input_.size();
}

char Lexer::Get() {
  if (Eof()) return '\0';
  char c = input_[i_++];
  if (c == '\n') {
    pos_.line++;
    pos_.col = 1;
  } else {
    pos_.col++;
  }
  return c;
}

void Lexer::SkipWhitespace() {
  while (!Eof()) {
    char c = Peek();
    if (std::isspace(static_cast<unsigned char>(c))) {
      Get();
      continue;
    }
    // Simple single-line comment: // ...
    if (c == '/' && Peek(1) == '/') {
      while (!Eof() && Peek() != '\n') Get();
      continue;
    }
    break;
  }
}

Token Lexer::MakeToken(TokenType t, std::string lexeme, SourcePos pos) {
  Token tok;
  tok.type = t;
  tok.lexeme = std::move(lexeme);
  tok.pos = pos;
  return tok;
}

Token Lexer::LexNumber() {
  SourcePos start = pos_;
  std::string num;
  while (!Eof() && std::isdigit(static_cast<unsigned char>(Peek()))) {
    num.push_back(Get());
  }
  int64_t v = 0;
  try {
    v = std::stoll(num);
  } catch (...) {
    // keep v=0
  }
  literals_.push_back(v);
  return MakeToken(TokenType::Number, num, start);
}

Token Lexer::LexIdentifierOrKeyword() {
  SourcePos start = pos_;
  std::string s;
  s.push_back(Get());
  while (!Eof() && edu::IsIdentChar(Peek())) s.push_back(Get());

  if (s == "int") return MakeToken(TokenType::KwInt, s, start);
  if (s == "print") return MakeToken(TokenType::KwPrint, s, start);

  // Record identifier in symbol table (as "seen"; declaration happens in semantic/parse stages)
  if (!symbols_.count(s)) {
    symbols_.insert({s, SymbolInfo{s, "int", false}});
  }
  return MakeToken(TokenType::Identifier, s, start);
}

std::vector<Token> Lexer::Tokenize() {
  std::vector<Token> out;
  while (true) {
    SkipWhitespace();
    if (Eof()) break;

    SourcePos start = pos_;
    char c = Peek();

    if (std::isdigit(static_cast<unsigned char>(c))) {
      out.push_back(LexNumber());
      continue;
    }
    if (edu::IsIdentStart(c)) {
      out.push_back(LexIdentifierOrKeyword());
      continue;
    }

    switch (c) {
      case '+': Get(); out.push_back(MakeToken(TokenType::Plus, "+", start)); break;
      case '-': Get(); out.push_back(MakeToken(TokenType::Minus, "-", start)); break;
      case '*': Get(); out.push_back(MakeToken(TokenType::Star, "*", start)); break;
      case '/': Get(); out.push_back(MakeToken(TokenType::Slash, "/", start)); break;
      case '=': Get(); out.push_back(MakeToken(TokenType::Assign, "=", start)); break;
      case ';': Get(); out.push_back(MakeToken(TokenType::Semicolon, ";", start)); break;
      case ',': Get(); out.push_back(MakeToken(TokenType::Comma, ",", start)); break;
      case '(': Get(); out.push_back(MakeToken(TokenType::LParen, "(", start)); break;
      case ')': Get(); out.push_back(MakeToken(TokenType::RParen, ")", start)); break;
      default: {
        std::string u;
        u.push_back(Get());
        out.push_back(MakeToken(TokenType::Unknown, u, start));
      } break;
    }
  }

  out.push_back(MakeToken(TokenType::End, "", pos_));
  return out;
}

TokenType Lexer::ClassifyWord(const std::string& word) {
  std::string w = edu::Trim(word);
  if (w.empty()) return TokenType::Unknown;
  if (w == "int") return TokenType::KwInt;
  if (w == "print") return TokenType::KwPrint;
  if (w.size() == 1) {
    switch (w[0]) {
      case '+': return TokenType::Plus;
      case '-': return TokenType::Minus;
      case '*': return TokenType::Star;
      case '/': return TokenType::Slash;
      case '=': return TokenType::Assign;
      case ';': return TokenType::Semicolon;
      case ',': return TokenType::Comma;
      case '(': return TokenType::LParen;
      case ')': return TokenType::RParen;
      default: break;
    }
  }
  bool allDigits = true;
  for (char c : w) {
    if (!std::isdigit(static_cast<unsigned char>(c))) { allDigits = false; break; }
  }
  if (allDigits) return TokenType::Number;

  if (!edu::IsIdentStart(w[0])) return TokenType::Unknown;
  for (size_t i = 1; i < w.size(); ++i) {
    if (!edu::IsIdentChar(w[i])) return TokenType::Unknown;
  }
  return TokenType::Identifier;
}

}  // namespace edu::compiler

