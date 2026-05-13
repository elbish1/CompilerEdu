// api_main.cpp  –  JSON API mode for the educational compiler.
// Usage:  compiler_api <command> <input_file>
//
// Commands:
//   lexical       – lexer only
//   syntax        – lexer + parser
//   semantic      – lexer + parser + semantic
//   intermediate  – lexer + parser + semantic + IR
//   optimize      – full pipeline through optimizer
//   codegen       – full pipeline through code generation
//   pipeline      – all phases combined
//   regex2nfa     – regex → NFA
//   nfa2dfa       – regex → NFA → DFA
//   grammar       – regex → NFA → regular grammar
//   derivation    – regex → NFA → derivation (file has 2 lines: regex, string)
//   evaluate      – expression evaluator
//
// The input is read from the file whose path is the second argument.
// All output is JSON to stdout.

#include "automata/DFA.h"
#include "automata/Grammar.h"
#include "automata/NFA.h"
#include "automata/Regex.h"
#include "compiler/CodeGen.h"
#include "compiler/IR.h"
#include "compiler/Lexer.h"
#include "compiler/Optimizer.h"
#include "compiler/Parser.h"
#include "compiler/Semantic.h"
#include "tools/ExpressionEval.h"
#include "tools/StringTools.h"

#include <fstream>

using namespace edu;

// ── JSON helpers ──────────────────────────────────────────────────────

static std::string Esc(const std::string& s) {
  std::string o;
  o.reserve(s.size() + 8);
  for (char c : s) {
    switch (c) {
      case '"':  o += "\\\""; break;
      case '\\': o += "\\\\"; break;
      case '\n': o += "\\n";  break;
      case '\r': o += "\\r";  break;
      case '\t': o += "\\t";  break;
      default:
        if (static_cast<unsigned char>(c) < 0x20) {
          char buf[8];
          snprintf(buf, sizeof(buf), "\\u%04x", (unsigned)(unsigned char)c);
          o += buf;
        } else {
          o += c;
        }
    }
  }
  return o;
}

static std::string ReadFile(const std::string& path) {
  std::ifstream f(path);
  if (!f) return "";
  std::ostringstream oss;
  oss << f.rdbuf();
  return oss.str();
}

// ── AST to JSON ──────────────────────────────────────────────────────

enum class ASTJsonMode { SyntaxStructure, SemanticAnnotated };

static std::string ASTToJSON(const compiler::ASTNode* n, ASTJsonMode mode) {
  if (!n) return "null";
  std::ostringstream o;
  o << "{";
  o << "\"kind\":\"" << compiler::ToString(n->kind) << "\"";
  if (!n->token.lexeme.empty())
    o << ",\"lexeme\":\"" << Esc(n->token.lexeme) << "\"";
  o << ",\"tokenType\":\"" << compiler::ToString(n->token.type) << "\"";
  o << ",\"line\":" << n->token.pos.line;
  o << ",\"col\":" << n->token.pos.col;
  if (mode == ASTJsonMode::SyntaxStructure) {
    o << ",\"layer\":\"syntax\"";
  } else {
    o << ",\"layer\":\"semantic\"";
    if (!n->type.name.empty()) {
      o << ",\"type\":\"" << Esc(n->type.name) << "\"";
      o << ",\"typeOk\":" << (n->type.isValid ? "true" : "false");
    }
    if (n->isConst && n->constValue) {
      o << ",\"isConst\":true";
      o << ",\"constValue\":" << *n->constValue;
    }
    o << ",\"scopeDepth\":" << n->scopeDepth;
    if (n->kind == compiler::NodeKind::Ident && !n->token.lexeme.empty()) {
      o << ",\"symbolKnownInitialized\":" << (n->symbolKnownInitialized ? "true" : "false");
    }
  }
  if (!n->children.empty()) {
    o << ",\"children\":[";
    for (size_t i = 0; i < n->children.size(); ++i) {
      if (i) o << ",";
      o << ASTToJSON(n->children[i].get(), mode);
    }
    o << "]";
  }
  o << "}";
  return o.str();
}

// ── Tokens to JSON ───────────────────────────────────────────────────

static std::string TokensToJSON(const std::vector<compiler::Token>& toks) {
  std::ostringstream o;
  o << "[";
  bool first = true;
  for (const auto& t : toks) {
    if (t.type == compiler::TokenType::End) break;
    if (!first) o << ",";
    first = false;
    o << "{\"type\":\"" << compiler::ToString(t.type) << "\""
      << ",\"lexeme\":\"" << Esc(t.lexeme) << "\""
      << ",\"line\":" << t.pos.line
      << ",\"col\":" << t.pos.col << "}";
  }
  o << "]";
  return o.str();
}

// ── Symbol table to JSON ─────────────────────────────────────────────

static std::string SymbolTableToJSON(
    const std::unordered_map<std::string, compiler::SymbolInfo>& syms) {
  std::vector<std::string> names;
  for (const auto& kv : syms) names.push_back(kv.first);
  std::sort(names.begin(), names.end());

  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < names.size(); ++i) {
    if (i) o << ",";
    const auto& s = syms.at(names[i]);
    o << "{\"name\":\"" << Esc(s.name) << "\""
      << ",\"type\":\"" << Esc(s.type) << "\""
      << ",\"declared\":" << (s.declared ? "true" : "false")
      << ",\"initialized\":" << (s.initialized ? "true" : "false") << "}";
  }
  o << "]";
  return o.str();
}

// ── Literal table to JSON ────────────────────────────────────────────

static std::string LiteralTableToJSON(const std::vector<int64_t>& lits) {
  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < lits.size(); ++i) {
    if (i) o << ",";
    o << lits[i];
  }
  o << "]";
  return o.str();
}

// ── IR to JSON ───────────────────────────────────────────────────────

static std::string IRToJSON(const std::vector<compiler::IRInstr>& ir) {
  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < ir.size(); ++i) {
    if (i) o << ",";
    const auto& in = ir[i];
    o << "{\"op\":\"" << compiler::ToString(in.op) << "\""
      << ",\"res\":\"" << Esc(in.res) << "\""
      << ",\"arg1\":\"" << Esc(in.arg1) << "\""
      << ",\"arg2\":\"" << Esc(in.arg2) << "\""
      << ",\"hasSideEffect\":" << (in.HasSideEffect() ? "true" : "false")
      << "}";
  }
  o << "]";
  return o.str();
}

// ── Target code to JSON ──────────────────────────────────────────────

static std::string TargetCodeToJSON(const std::vector<std::string>& code) {
  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < code.size(); ++i) {
    if (i) o << ",";
    o << "\"" << Esc(code[i]) << "\"";
  }
  o << "]";
  return o.str();
}

// ── NFA to JSON ──────────────────────────────────────────────────────

static std::string NFAToJSON(const automata::NFA& nfa) {
  std::ostringstream o;
  o << "{\"start\":" << nfa.start
    << ",\"stateCount\":" << nfa.stateCount
    << ",\"accept\":[";
  {
    std::vector<int> acc(nfa.accept.begin(), nfa.accept.end());
    std::sort(acc.begin(), acc.end());
    for (size_t i = 0; i < acc.size(); ++i) {
      if (i) o << ",";
      o << acc[i];
    }
  }
  o << "],\"transitions\":[";
  bool first = true;
  for (int s = 0; s < nfa.stateCount; ++s) {
    auto itFrom = nfa.trans.find(s);
    if (itFrom == nfa.trans.end()) continue;
    for (auto it = itFrom->second.begin(); it != itFrom->second.end(); ++it) {
      char sym = it->first;
      for (int t : it->second) {
        if (!first) o << ",";
        first = false;
        o << "{\"from\":" << s << ",\"to\":" << t << ",\"symbol\":";
        if (sym == automata::EPS)
          o << "\"ε\"";
        else
          o << "\"" << sym << "\"";
        o << "}";
      }
    }
  }
  o << "]}";
  return o.str();
}

// ── DFA to JSON ──────────────────────────────────────────────────────

static std::string DFAToJSON(const automata::DFA& dfa) {
  std::ostringstream o;
  o << "{\"start\":" << dfa.start
    << ",\"stateCount\":" << dfa.stateCount
    << ",\"accept\":[";
  {
    std::vector<int> acc(dfa.accept.begin(), dfa.accept.end());
    std::sort(acc.begin(), acc.end());
    for (size_t i = 0; i < acc.size(); ++i) {
      if (i) o << ",";
      o << acc[i];
    }
  }
  o << "],\"alphabet\":[";
  for (size_t i = 0; i < dfa.alphabet.size(); ++i) {
    if (i) o << ",";
    o << "\"" << dfa.alphabet[i] << "\"";
  }
  o << "],\"transitions\":[";
  bool first = true;
  for (int s = 0; s < dfa.stateCount; ++s) {
    auto it = dfa.trans.find(s);
    if (it == dfa.trans.end()) continue;
    for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
      if (!first) o << ",";
      first = false;
      o << "{\"from\":" << s << ",\"to\":" << it2->second
        << ",\"symbol\":\"" << it2->first << "\"}";
    }
  }
  o << "],\"subsets\":{";
  first = true;
  for (int s = 0; s < dfa.stateCount; ++s) {
    auto it = dfa.subset.find(s);
    if (it == dfa.subset.end()) continue;
    if (!first) o << ",";
    first = false;
    std::vector<int> v(it->second.begin(), it->second.end());
    std::sort(v.begin(), v.end());
    o << "\"" << s << "\":[";
    for (size_t i = 0; i < v.size(); ++i) {
      if (i) o << ",";
      o << v[i];
    }
    o << "]";
  }
  o << "}}";
  return o.str();
}

// ── Grammar to JSON ──────────────────────────────────────────────────

static std::string GrammarToJSON(const automata::RegularGrammar& g) {
  std::ostringstream o;
  o << "{\"startSymbol\":\"" << Esc(g.startSymbol) << "\""
    << ",\"productions\":[";
  for (size_t i = 0; i < g.prods.size(); ++i) {
    if (i) o << ",";
    o << "{\"lhs\":\"" << Esc(g.prods[i].lhs) << "\""
      << ",\"rhs\":\"" << Esc(g.prods[i].rhs) << "\"}";
  }
  o << "]}";
  return o.str();
}

// ── Errors to JSON ───────────────────────────────────────────────────

static std::string ErrorsToJSON(const std::vector<std::string>& errs) {
  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < errs.size(); ++i) {
    if (i) o << ",";
    o << "\"" << Esc(errs[i]) << "\"";
  }
  o << "]";
  return o.str();
}

// ── Commands ─────────────────────────────────────────────────────────

static void CmdLexical(const std::string& src) {
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  std::cout << "{\"tokens\":" << TokensToJSON(toks)
            << ",\"symbolTable\":" << SymbolTableToJSON(lex.SymbolTable())
            << ",\"literalTable\":" << LiteralTableToJSON(lex.LiteralTable())
            << "}" << std::endl;
}

static void CmdSyntax(const std::string& src) {
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  std::cout << "{\"tokens\":" << TokensToJSON(toks)
            << ",\"symbolTable\":" << SymbolTableToJSON(lex.SymbolTable())
            << ",\"literalTable\":" << LiteralTableToJSON(lex.LiteralTable())
            << ",\"parseTree\":" << ASTToJSON(ast.get(), ASTJsonMode::SyntaxStructure)
            << ",\"parseErrors\":" << ErrorsToJSON(parser.Errors())
            << "}" << std::endl;
}

static void CmdSemantic(const std::string& src) {
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  std::string syntaxTree = ASTToJSON(ast.get(), ASTJsonMode::SyntaxStructure);
  compiler::SemanticAnalyzer sem;
  auto res = sem.Analyze(ast.get(), lex.SymbolTable());
  std::cout << "{\"tokens\":" << TokensToJSON(toks)
            << ",\"symbolTable\":" << SymbolTableToJSON(res.symbols)
            << ",\"literalTable\":" << LiteralTableToJSON(lex.LiteralTable())
            << ",\"parseTree\":" << syntaxTree
            << ",\"annotatedAst\":" << ASTToJSON(ast.get(), ASTJsonMode::SemanticAnnotated)
            << ",\"parseErrors\":" << ErrorsToJSON(parser.Errors())
            << ",\"semanticErrors\":" << ErrorsToJSON(res.errors)
            << "}" << std::endl;
}

static void CmdIntermediate(const std::string& src) {
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  std::string syntaxTree = ASTToJSON(ast.get(), ASTJsonMode::SyntaxStructure);
  compiler::SemanticAnalyzer sem;
  auto res = sem.Analyze(ast.get(), lex.SymbolTable());
  compiler::IRGen irg;
  auto ir = irg.Generate(ast.get());
  std::cout << "{\"tokens\":" << TokensToJSON(toks)
            << ",\"symbolTable\":" << SymbolTableToJSON(res.symbols)
            << ",\"literalTable\":" << LiteralTableToJSON(lex.LiteralTable())
            << ",\"parseTree\":" << syntaxTree
            << ",\"annotatedAst\":" << ASTToJSON(ast.get(), ASTJsonMode::SemanticAnnotated)
            << ",\"parseErrors\":" << ErrorsToJSON(parser.Errors())
            << ",\"semanticErrors\":" << ErrorsToJSON(res.errors)
            << ",\"ir\":" << IRToJSON(ir)
            << "}" << std::endl;
}

static void CmdOptimize(const std::string& src) {
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  std::string syntaxTree = ASTToJSON(ast.get(), ASTJsonMode::SyntaxStructure);
  compiler::SemanticAnalyzer sem;
  auto res = sem.Analyze(ast.get(), lex.SymbolTable());
  compiler::IRGen irg;
  auto ir = irg.Generate(ast.get());
  auto ir1 = compiler::Optimizer::ConstantFolding(ir);
  auto ir2 = compiler::Optimizer::DeadCodeElim(ir1);
  auto ir3 = compiler::Optimizer::RemoveRedundant(ir2);
  std::cout << "{\"tokens\":" << TokensToJSON(toks)
            << ",\"symbolTable\":" << SymbolTableToJSON(res.symbols)
            << ",\"literalTable\":" << LiteralTableToJSON(lex.LiteralTable())
            << ",\"parseTree\":" << syntaxTree
            << ",\"annotatedAst\":" << ASTToJSON(ast.get(), ASTJsonMode::SemanticAnnotated)
            << ",\"parseErrors\":" << ErrorsToJSON(parser.Errors())
            << ",\"semanticErrors\":" << ErrorsToJSON(res.errors)
            << ",\"ir\":" << IRToJSON(ir)
            << ",\"optimizedIr\":" << IRToJSON(ir3)
            << "}" << std::endl;
}

static void CmdCodegen(const std::string& src) {
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  std::string syntaxTree = ASTToJSON(ast.get(), ASTJsonMode::SyntaxStructure);
  compiler::SemanticAnalyzer sem;
  auto res = sem.Analyze(ast.get(), lex.SymbolTable());
  compiler::IRGen irg;
  auto ir = irg.Generate(ast.get());
  auto ir1 = compiler::Optimizer::ConstantFolding(ir);
  auto ir2 = compiler::Optimizer::DeadCodeElim(ir1);
  auto ir3 = compiler::Optimizer::RemoveRedundant(ir2);
  auto target = compiler::CodeGen::Generate(ir3);
  std::cout << "{\"tokens\":" << TokensToJSON(toks)
            << ",\"symbolTable\":" << SymbolTableToJSON(res.symbols)
            << ",\"literalTable\":" << LiteralTableToJSON(lex.LiteralTable())
            << ",\"parseTree\":" << syntaxTree
            << ",\"annotatedAst\":" << ASTToJSON(ast.get(), ASTJsonMode::SemanticAnnotated)
            << ",\"parseErrors\":" << ErrorsToJSON(parser.Errors())
            << ",\"semanticErrors\":" << ErrorsToJSON(res.errors)
            << ",\"ir\":" << IRToJSON(ir)
            << ",\"optimizedIr\":" << IRToJSON(ir3)
            << ",\"targetCode\":" << TargetCodeToJSON(target)
            << "}" << std::endl;
}

static void CmdPipeline(const std::string& src) {
  CmdCodegen(src);  // same output as full codegen
}

static void CmdRegex2NFA(const std::string& regex) {
  auto nfa = automata::RegexToNFA(regex);
  std::cout << "{\"nfa\":" << NFAToJSON(nfa) << "}" << std::endl;
}

static void CmdNFA2DFA(const std::string& regex) {
  auto nfa = automata::RegexToNFA(regex);
  auto dfa = automata::ConvertToDFA(nfa);
  std::cout << "{\"nfa\":" << NFAToJSON(nfa)
            << ",\"dfa\":" << DFAToJSON(dfa) << "}" << std::endl;
}

static void CmdGrammar(const std::string& regex) {
  auto nfa = automata::RegexToNFA(regex);
  auto dfa = automata::ConvertToDFA(nfa);
  auto g   = automata::NfaToRegularGrammar(nfa);
  std::cout << "{\"nfa\":" << NFAToJSON(nfa)
            << ",\"dfa\":" << DFAToJSON(dfa)
            << ",\"grammar\":" << GrammarToJSON(g)
            << "}" << std::endl;
}

static void CmdDerivation(const std::string& input) {
  // input is two lines: regex\nstring
  std::istringstream iss(input);
  std::string regex, str;
  std::getline(iss, regex);
  std::getline(iss, str);
  regex = edu::Trim(regex);
  str = edu::Trim(str);

  auto nfa = automata::RegexToNFA(regex);
  auto dfa = automata::ConvertToDFA(nfa);
  auto g   = automata::NfaToRegularGrammar(nfa);
  auto left  = automata::LeftmostDerivation(nfa, str);
  auto right = automata::RightmostDerivation(nfa, str);

  std::ostringstream o;
  o << "{\"nfa\":" << NFAToJSON(nfa)
    << ",\"dfa\":" << DFAToJSON(dfa)
    << ",\"grammar\":" << GrammarToJSON(g)
    << ",\"leftmostDerivation\":[";
  for (size_t i = 0; i < left.size(); ++i) {
    if (i) o << ",";
    o << "\"" << Esc(left[i]) << "\"";
  }
  o << "],\"rightmostDerivation\":[";
  for (size_t i = 0; i < right.size(); ++i) {
    if (i) o << ",";
    o << "\"" << Esc(right[i]) << "\"";
  }
  o << "]}";
  std::cout << o.str() << std::endl;
}

static void CmdEvaluate(const std::string& expr) {
  auto r = tools::EvaluateExpression(expr);
  if (r.ok)
    std::cout << "{\"ok\":true,\"value\":" << r.value << "}" << std::endl;
  else
    std::cout << "{\"ok\":false,\"error\":\"" << Esc(r.error) << "\"}" << std::endl;
}

// ── Main ─────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "{\"error\":\"Usage: compiler_api <command> <input_file>\"}" << std::endl;
    return 1;
  }

  std::string cmd = argv[1];
  std::string inputFile = argv[2];
  std::string input = ReadFile(inputFile);

  try {
    if      (cmd == "lexical")      CmdLexical(input);
    else if (cmd == "syntax")       CmdSyntax(input);
    else if (cmd == "semantic")     CmdSemantic(input);
    else if (cmd == "intermediate") CmdIntermediate(input);
    else if (cmd == "optimize")     CmdOptimize(input);
    else if (cmd == "codegen")      CmdCodegen(input);
    else if (cmd == "pipeline")     CmdPipeline(input);
    else if (cmd == "regex2nfa")    CmdRegex2NFA(edu::Trim(input));
    else if (cmd == "nfa2dfa")      CmdNFA2DFA(edu::Trim(input));
    else if (cmd == "grammar")      CmdGrammar(edu::Trim(input));
    else if (cmd == "derivation")   CmdDerivation(input);
    else if (cmd == "evaluate")     CmdEvaluate(edu::Trim(input));
    else {
      std::cout << "{\"error\":\"Unknown command: " << Esc(cmd) << "\"}" << std::endl;
      return 1;
    }
  } catch (const std::exception& e) {
    std::cout << "{\"error\":\"" << Esc(e.what()) << "\"}" << std::endl;
    return 1;
  } catch (...) {
    std::cout << "{\"error\":\"Unknown exception\"}" << std::endl;
    return 1;
  }

  return 0;
}
