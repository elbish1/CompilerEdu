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

using namespace edu;

static void PauseForUser() {
  std::cout << "\n----------------------------------------\n";
  std::cout << "Press Enter to return to the main menu...";
  std::cout.flush();
  std::string dummy;
  std::getline(std::cin, dummy);
  if (!std::cin && !std::cin.eof()) std::cin.clear();
  std::cout << "\n";
}

static std::string ReadMultilineProgram() {
  std::cout << "Paste/type source code now.\n";
  std::cout << "Finish input by writing a single line: END\n";
  std::string line;
  std::ostringstream oss;
  while (true) {
    std::getline(std::cin, line);
    if (!std::cin) break;
    if (edu::Trim(line) == "END") break;
    oss << line << "\n";
  }
  return oss.str();
}

static void PrintTokens(const std::vector<compiler::Token>& toks) {
  std::cout << "Tokens:\n";
  for (const auto& t : toks) {
    if (t.type == compiler::TokenType::End) break;
    std::cout << "  (" << compiler::ToString(t.type) << ", '" << t.lexeme << "')"
              << " @(" << t.pos.line << "," << t.pos.col << ")\n";
  }
}

static void PrintSymbolTable(const std::unordered_map<std::string, compiler::SymbolInfo>& syms) {
  std::vector<std::string> names;
  names.reserve(syms.size());
  for (const auto& kv : syms) names.push_back(kv.first);
  std::sort(names.begin(), names.end());

  std::cout << "Symbol Table:\n";
  std::cout << "  Name\tType\tDeclared\n";
  for (const auto& n : names) {
    const auto& s = syms.at(n);
    std::cout << "  " << s.name << "\t" << s.type << "\t" << (s.declared ? "yes" : "no") << "\n";
  }
}

static void PrintLiteralTable(const std::vector<int64_t>& lits) {
  std::cout << "Literal Table:\n";
  for (size_t i = 0; i < lits.size(); ++i) {
    std::cout << "  [" << i << "] " << lits[i] << "\n";
  }
}

static void RunLexOnly() {
  std::string src = ReadMultilineProgram();
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();

  PrintTokens(toks);
  PrintSymbolTable(lex.SymbolTable());
  PrintLiteralTable(lex.LiteralTable());
}

static void RunCompilerPipeline(bool lexOnly = false) {
  std::string src = ReadMultilineProgram();

  // 1) Lexical
  std::cout << "\n========== 1) Lexical Analysis ==========\n";
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();
  PrintTokens(toks);
  PrintSymbolTable(lex.SymbolTable());
  PrintLiteralTable(lex.LiteralTable());

  if (lexOnly) return;

  // 2) Syntax
  std::cout << "\n========== 2) Syntax Analysis (Parse Tree) ==========\n";
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  if (!parser.Errors().empty()) {
    for (const auto& e : parser.Errors()) std::cout << e << "\n";
  }
  compiler::PrintTree(ast.get());

  // 3) Semantic
  std::cout << "\n========== 3) Semantic Analysis (Annotated Tree) ==========\n";
  compiler::SemanticAnalyzer sem;
  auto semRes = sem.Analyze(ast.get(), lex.SymbolTable());
  if (!semRes.errors.empty()) {
    for (const auto& e : semRes.errors) std::cout << e << "\n";
  }
  PrintSymbolTable(semRes.symbols);
  compiler::PrintTree(ast.get());

  // 4) IR
  std::cout << "\n========== 4) Intermediate Code (Three-Address Code) ==========\n";
  compiler::IRGen irg;
  auto ir = irg.Generate(ast.get());
  compiler::PrintIR(ir);

  // 5) Optimizations
  std::cout << "\n========== 5) Code Optimization ==========\n";
  auto ir1 = compiler::Optimizer::ConstantFolding(ir);
  auto ir2 = compiler::Optimizer::DeadCodeElim(ir1);
  auto ir3 = compiler::Optimizer::RemoveRedundant(ir2);
  std::cout << "Optimized IR:\n";
  compiler::PrintIR(ir3);

  // 6) Target code
  std::cout << "\n========== 6) Target Code Generation (Pseudo Machine Code) ==========\n";
  auto target = compiler::CodeGen::Generate(ir3);
  compiler::PrintTarget(target);
}

static void RunSyntaxSemanticOnly() {
  std::string src = ReadMultilineProgram();
  compiler::Lexer lex(src);
  auto toks = lex.Tokenize();

  std::cout << "\n========== Tokens ==========\n";
  PrintTokens(toks);

  std::cout << "\n========== Parse Tree ==========\n";
  compiler::Parser parser(toks);
  auto ast = parser.ParseProgram();
  for (const auto& e : parser.Errors()) std::cout << e << "\n";
  compiler::PrintTree(ast.get());

  std::cout << "\n========== Semantic Analysis ==========\n";
  compiler::SemanticAnalyzer sem;
  auto res = sem.Analyze(ast.get(), lex.SymbolTable());
  for (const auto& e : res.errors) std::cout << e << "\n";
  PrintSymbolTable(res.symbols);
  std::cout << "Annotated Tree:\n";
  compiler::PrintTree(ast.get());
}

static void RunNFADFA() {
  std::cout << "Enter regex (example: (a|b)*abb)\n> ";
  std::string re;
  std::getline(std::cin, re);

  auto nfa = automata::RegexToNFA(re);
  std::cout << "\n========== Regex -> NFA ==========\n";
  automata::PrintNFA(nfa);

  std::cout << "\n========== NFA -> DFA ==========\n";
  auto dfa = automata::ConvertToDFA(nfa);
  automata::PrintDFA(dfa);

  std::cout << "\n========== Regular Grammar from NFA ==========\n";
  auto g = automata::NfaToRegularGrammar(nfa);
  automata::PrintGrammar(g);

  std::cout << "\nEnter a string to derive (must be accepted to show full derivation)\n> ";
  std::string s;
  std::getline(std::cin, s);

  auto left = automata::LeftmostDerivation(nfa, s);
  auto right = automata::RightmostDerivation(nfa, s);

  std::cout << "\nLeftmost derivation:\n";
  if (left.empty()) std::cout << "  (no accepting path found)\n";
  else for (size_t i = 0; i < left.size(); ++i) std::cout << "  " << i << ": " << left[i] << "\n";

  std::cout << "\nRightmost derivation:\n";
  if (right.empty()) std::cout << "  (no accepting path found)\n";
  else for (size_t i = 0; i < right.size(); ++i) std::cout << "  " << i << ": " << right[i] << "\n";
}

static void RunExpressionEvaluator() {
  std::cout << "Enter expression (example: 2 + 3*(10-4))\n> ";
  std::string s;
  std::getline(std::cin, s);
  auto r = tools::EvaluateExpression(s);
  if (!r.ok) std::cout << "Error: " << r.error << "\n";
  else std::cout << "Value = " << r.value << "\n";
}

static void RunStringAnalyzer() {
  std::cout << "Enter string:\n> ";
  std::string s;
  std::getline(std::cin, s);
  auto a = tools::AnalyzeString(s);
  std::cout << "Length = " << a.length << "\n";
  std::cout << "Even/Odd = " << (a.isEven ? "Even" : "Odd") << "\n";
}

static void RunTokenClassifier() {
  std::cout << "Enter a word/token to classify:\n> ";
  std::string w;
  std::getline(std::cin, w);
  auto t = compiler::Lexer::ClassifyWord(w);
  std::cout << "Token type: " << compiler::ToString(t) << "\n";

  auto a = tools::AnalyzeString(w);
  std::cout << "String length: " << a.length << " (" << (a.isEven ? "Even" : "Odd") << ")\n";
}

int main() {
  while (true) {
    std::cout << "\n========================\n";
    std::cout << "Educational Compiler Project\n";
    std::cout << "========================\n";
    std::cout << "1. Run Full Compiler Pipeline\n";
    std::cout << "2. Lexical Analysis Only\n";
    std::cout << "3. Syntax + Semantic Analysis\n";
    std::cout << "4. NFA/DFA Module\n";
    std::cout << "5. Expression Evaluator\n";
    std::cout << "6. String Analyzer\n";
    std::cout << "7. Token Classification\n";
    std::cout << "0. Exit\n";
    std::cout << "> ";

    std::string choice;
    std::getline(std::cin, choice);
    if (!std::cin) {
      // If user sent EOF (Ctrl+Z then Enter, or input redirected) we exit.
      // Otherwise recover and keep the app alive.
      if (std::cin.eof()) break;
      std::cin.clear();
      continue;
    }
    choice = edu::Trim(choice);
    if (choice.empty()) continue;

    if (choice == "0") break;
    if (choice == "1") { RunCompilerPipeline(false); PauseForUser(); continue; }
    if (choice == "2") { RunLexOnly(); PauseForUser(); continue; }
    if (choice == "3") { RunSyntaxSemanticOnly(); PauseForUser(); continue; }
    if (choice == "4") { RunNFADFA(); PauseForUser(); continue; }
    if (choice == "5") { RunExpressionEvaluator(); PauseForUser(); continue; }
    if (choice == "6") { RunStringAnalyzer(); PauseForUser(); continue; }
    if (choice == "7") { RunTokenClassifier(); PauseForUser(); continue; }

    std::cout << "Unknown choice.\n";
  }

  std::cout << "Goodbye.\n";
  return 0;
}

