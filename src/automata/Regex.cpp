#include "automata/Regex.h"

namespace edu::automata {

static bool IsOperator(char c) {
  return c == '|' || c == '*' || c == '(' || c == ')' || c == '.';
}

static bool IsLiteralChar(char c) {
  if (std::isspace(static_cast<unsigned char>(c))) return false;
  return !IsOperator(c);
}

static int Prec(char op) {
  switch (op) {
    case '*': return 3;
    case '.': return 2;  // concat
    case '|': return 1;
    default: return 0;
  }
}

static std::string InsertConcat(const std::string& re) {
  std::string out;
  auto isAtomEnd = [](char c) {
    return IsLiteralChar(c) || c == ')' || c == '*';
  };
  auto isAtomStart = [](char c) {
    return IsLiteralChar(c) || c == '(';
  };
  for (size_t i = 0; i < re.size(); ++i) {
    char c1 = re[i];
    if (std::isspace(static_cast<unsigned char>(c1))) continue;
    out.push_back(c1);
    if (i + 1 >= re.size()) continue;
    char c2 = re[i + 1];
    if (std::isspace(static_cast<unsigned char>(c2))) continue;
    if (isAtomEnd(c1) && isAtomStart(c2)) out.push_back('.');
  }
  return out;
}

static std::string ToPostfix(const std::string& re) {
  std::string in = InsertConcat(re);
  std::string out;
  std::vector<char> st;

  for (size_t i = 0; i < in.size(); ++i) {
    char c = in[i];
    if (IsLiteralChar(c)) {
      out.push_back(c);
      continue;
    }
    if (c == '(') { st.push_back(c); continue; }
    if (c == ')') {
      while (!st.empty() && st.back() != '(') { out.push_back(st.back()); st.pop_back(); }
      if (!st.empty() && st.back() == '(') st.pop_back();
      continue;
    }
    if (c == '*') {
      // postfix unary: directly append
      out.push_back(c);
      continue;
    }
    // binary ops: | or .
    while (!st.empty() && st.back() != '(' && Prec(st.back()) >= Prec(c)) {
      out.push_back(st.back());
      st.pop_back();
    }
    st.push_back(c);
  }
  while (!st.empty()) { out.push_back(st.back()); st.pop_back(); }
  return out;
}

struct Frag {
  int start;
  int accept;
};

NFA RegexToNFA(const std::string& regex) {
  std::string post = ToPostfix(regex);
  NFA nfa;
  nfa.stateCount = 0;

  std::vector<Frag> st;
  st.reserve(post.size());

  auto lit = [&](char c) {
    int s = nfa.NewState();
    int t = nfa.NewState();
    nfa.AddEdge(s, t, c);
    st.push_back(Frag{s, t});
  };

  auto concat = [&]() {
    Frag b = st.back(); st.pop_back();
    Frag a = st.back(); st.pop_back();
    nfa.AddEdge(a.accept, b.start, EPS);
    st.push_back(Frag{a.start, b.accept});
  };

  auto uni = [&]() {
    Frag b = st.back(); st.pop_back();
    Frag a = st.back(); st.pop_back();
    int s = nfa.NewState();
    int t = nfa.NewState();
    nfa.AddEdge(s, a.start, EPS);
    nfa.AddEdge(s, b.start, EPS);
    nfa.AddEdge(a.accept, t, EPS);
    nfa.AddEdge(b.accept, t, EPS);
    st.push_back(Frag{s, t});
  };

  auto star = [&]() {
    Frag a = st.back(); st.pop_back();
    int s = nfa.NewState();
    int t = nfa.NewState();
    nfa.AddEdge(s, a.start, EPS);
    nfa.AddEdge(s, t, EPS);
    nfa.AddEdge(a.accept, a.start, EPS);
    nfa.AddEdge(a.accept, t, EPS);
    st.push_back(Frag{s, t});
  };

  for (char c : post) {
    if (IsLiteralChar(c)) { lit(c); continue; }
    if (c == '.') { concat(); continue; }
    if (c == '|') { uni(); continue; }
    if (c == '*') { star(); continue; }
  }

  if (st.empty()) {
    // empty regex => accept empty string
    int s = nfa.NewState();
    int t = nfa.NewState();
    nfa.AddEdge(s, t, EPS);
    nfa.start = s;
    nfa.accept.insert(t);
    return nfa;
  }

  Frag f = st.back();
  nfa.start = f.start;
  nfa.accept.insert(f.accept);
  return nfa;
}

}  // namespace edu::automata

