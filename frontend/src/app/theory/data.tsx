import React from "react";
import type { LucideIcon } from "lucide-react";
import { BookOpen, Network, Cpu, FileCode2, Layers, SearchCheck, Zap, Scissors } from "lucide-react";

export const theoryData: Record<string, { title: string; icon: LucideIcon; content: React.ReactNode }> = {
  "introduction": {
    title: "Introduction to Compilers",
    icon: BookOpen,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Imagine you only speak English, but your friend only speaks Japanese. To communicate, you need an interpreter or a translator. A compiler is exactly that—a translator for computers.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">What is a Compiler?</h3>
        <p>You write code in a high-level language like C++, Java, or Python because it's readable by humans. However, a computer's processor (CPU) only understands "machine code"—a series of 1s and 0s (binary).</p>
        <p>A compiler is a specialized software program that takes your human-readable source code and translates it entirely into machine code *before* the program is executed. It does this through a pipeline of careful, well-defined phases.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Why is this important?</h3>
        <ul className="list-disc pl-6 space-y-2">
          <li><strong>Performance:</strong> Because the code is translated in advance, the resulting program runs incredibly fast.</li>
          <li><strong>Error Checking:</strong> The compiler acts as a strict proofreader. If you make a syntax error, the compiler catches it before you even try to run the program.</li>
          <li><strong>Optimization:</strong> A good compiler doesn't just translate; it improves your code. It can rearrange instructions so the CPU processes them faster without changing what the program actually does.</li>
        </ul>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">The Compiler Pipeline</h3>
        <p>Translating a complex program is too hard to do in one step. Instead, the compiler breaks the translation down into a sequence of phases:</p>
        <ol className="list-decimal pl-6 space-y-2 mb-6">
          <li><strong>Lexical Analysis:</strong> Reading the characters and grouping them into words (Tokens).</li>
          <li><strong>Syntax Analysis:</strong> Checking if the words form valid sentences (Parse Tree).</li>
          <li><strong>Semantic Analysis:</strong> Checking if the sentences actually make sense (Type Checking).</li>
          <li><strong>Intermediate Representation (IR):</strong> Translating to a middle-ground language.</li>
          <li><strong>Optimization:</strong> Making the middle-ground language faster.</li>
          <li><strong>Code Generation:</strong> Translating to the final 1s and 0s.</li>
        </ol>
        
        <div className="bg-primary/10 border border-primary/20 p-6 rounded-xl mt-8">
          <h4 className="font-bold text-primary mb-2 flex items-center gap-2"><Zap className="w-5 h-5" /> The Analogy</h4>
          <p className="text-sm">If you were translating an English book into French: First you identify the words (Lexical), then you check the grammar (Syntax), then you make sure the sentences are logical (Semantic), then you write a rough draft (IR), then you edit the draft to sound poetic (Optimization), and finally you publish the printed French book (Code Generation).</p>
        </div>
      </div>
    )
  },
  "lexical-analysis": {
    title: "Lexical Analysis (Scanning)",
    icon: SearchCheck,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Lexical Analysis is the very first step of the compiler. Its job is to read your code character by character and group them into meaningful "words" called Tokens.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">How does it work?</h3>
        <p>When you write <code>int a = 5;</code>, the computer just sees a long string of characters: <code>'i', 'n', 't', ' ', 'a', ' ', '=', ' ', '5', ';'</code>.</p>
        <p>The Lexical Analyzer (or Lexer) scans this string and throws away things that don't matter (like spaces and comments). It then groups the remaining characters into categories:</p>
        
        <div className="bg-muted p-4 rounded-lg font-mono text-sm my-4 border">
          <div className="grid grid-cols-2 gap-4">
            <div><strong>Code:</strong> <code>int</code></div><div>→ <strong>Token:</strong> Keyword(INT)</div>
            <div><strong>Code:</strong> <code>a</code></div><div>→ <strong>Token:</strong> Identifier("a")</div>
            <div><strong>Code:</strong> <code>=</code></div><div>→ <strong>Token:</strong> Operator(ASSIGN)</div>
            <div><strong>Code:</strong> <code>5</code></div><div>→ <strong>Token:</strong> Literal(5)</div>
            <div><strong>Code:</strong> <code>;</code></div><div>→ <strong>Token:</strong> Punctuation(SEMICOLON)</div>
          </div>
        </div>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Why is it important?</h3>
        <p>By converting raw text into Tokens, the Lexer makes the job of the next phase (the Parser) much easier. Instead of dealing with individual letters and spaces, the Parser gets a clean, organized list of meaningful symbols.</p>
        <p>The Lexer also creates the <strong>Symbol Table</strong>, which is like a dictionary where the compiler keeps track of all the variable names you create.</p>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">How is it built?</h3>
        <p>Lexers are built using <strong>Regular Expressions</strong> and <strong>Finite Automata (DFA/NFA)</strong>. A regular expression defines the pattern for a token (e.g., an identifier is a letter followed by letters or numbers). The DFA is the state machine that efficiently reads characters to match these patterns.</p>
      </div>
    )
  },
  "automata": {
    title: "Automata Theory (NFA & DFA)",
    icon: Network,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Automata Theory is the mathematical foundation behind how a Lexer recognizes patterns. It uses "State Machines" to read characters and decide what token they form.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">What is a State Machine?</h3>
        <p>Imagine a turnstile at a subway. It has two states: <strong>Locked</strong> and <strong>Unlocked</strong>. If you insert a coin, it transitions to Unlocked. If you push it, it transitions back to Locked. This is a finite automaton!</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Regular Expressions to Automata</h3>
        <p>When you define a pattern, like <code>[a-z]+</code> (one or more lowercase letters), the compiler converts this pattern into a graph called an Automaton.</p>
        
        <ul className="list-disc pl-6 space-y-4 my-4">
          <li>
            <strong className="text-foreground">NFA (Nondeterministic Finite Automaton):</strong><br/>
            An NFA is a flexible graph. It can have "Epsilon (ε) transitions," meaning it can jump from one state to another without reading any character. It's very easy for a computer to generate an NFA from a Regular Expression, but it's slow to execute because it has to guess which path to take.
          </li>
          <li>
            <strong className="text-foreground">DFA (Deterministic Finite Automaton):</strong><br/>
            A DFA is a strict graph. For every state and every character, there is exactly one path to follow. No guessing, no epsilon transitions. It is incredibly fast to execute!
          </li>
        </ul>

        <div className="bg-primary/10 border border-primary/20 p-6 rounded-xl mt-8">
          <h4 className="font-bold text-primary mb-2 flex items-center gap-2"><Zap className="w-5 h-5" /> The Workflow</h4>
          <p className="text-sm">In compiler design, we write a pattern as a Regular Expression. The compiler converts it to an NFA (using Thompson's Construction), then converts that NFA into a DFA (using Subset Construction). The DFA is what actually reads your source code at lightning speed!</p>
        </div>
      </div>
    )
  },
  "syntax-analysis": {
    title: "Syntax Analysis (Parsing)",
    icon: Layers,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Syntax Analysis (or Parsing) is the second phase. It takes the list of Tokens from the Lexer and checks if they form grammatically correct sentences.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Checking the Grammar</h3>
        <p>In English, "The dog barks" is grammatically correct (Noun Verb), but "Barks dog the" is wrong. Programming languages have strict grammar rules called <strong>Context-Free Grammars</strong>.</p>
        <p>If you write <code>int a = 5 + * 3;</code>, the Lexer will perfectly extract the tokens. But the Parser will throw a Syntax Error, because you cannot have a `+` directly followed by a `*`.</p>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">The Abstract Syntax Tree (AST)</h3>
        <p>If the grammar is correct, the Parser builds a tree structure called the Abstract Syntax Tree. This tree represents the hierarchical structure of your code.</p>
        
        <div className="bg-muted p-4 rounded-lg font-mono text-sm my-4 border">
          <div>Code: <code>z = x + y * 2</code></div>
          <div className="mt-4 pl-4 border-l-2 border-primary">
            <div>= (Assignment)</div>
            <div className="pl-4">├── z (Identifier)</div>
            <div className="pl-4">└── + (Add)</div>
            <div className="pl-8">├── x (Identifier)</div>
            <div className="pl-8">└── * (Multiply)</div>
            <div className="pl-12">├── y (Identifier)</div>
            <div className="pl-12">└── 2 (Literal)</div>
          </div>
        </div>

        <p>Notice how the tree naturally handles the order of operations (PEMDAS). The multiplication `y * 2` is lower in the tree, meaning it will be calculated first.</p>
      </div>
    )
  },
  "semantic-analysis": {
    title: "Semantic Analysis",
    icon: SearchCheck,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Semantic Analysis is where the compiler checks if your grammatically correct code actually makes logical sense.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Grammar vs. Meaning</h3>
        <p>In English, "The green idea sleeps furiously" is grammatically perfect, but it makes zero sense. The same happens in code.</p>
        
        <p>Consider this code:</p>
        <div className="bg-muted p-4 rounded-lg font-mono text-sm my-2 border">
          <code>
            int a = 5;<br/>
            string b = "hello";<br/>
            int c = a + b;
          </code>
        </div>
        
        <p>The Parser says this is perfectly fine syntax: <code>Variable = Variable + Variable</code>. But the Semantic Analyzer catches the error: You cannot add an integer to a string!</p>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">What does it check?</h3>
        <ul className="list-disc pl-6 space-y-2">
          <li><strong>Type Checking:</strong> Ensuring data types match (like the integer and string example).</li>
          <li><strong>Variable Declaration:</strong> Ensuring you don't use a variable that hasn't been declared yet.</li>
          <li><strong>Scope Resolution:</strong> Making sure variables are only used within their allowed blocks of code.</li>
        </ul>
        
        <p className="mt-4">If it passes, the AST is "annotated" with type information and is ready for translation.</p>
      </div>
    )
  },
  "intermediate-representation": {
    title: "Intermediate Representation (IR)",
    icon: FileCode2,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Intermediate Representation (IR) is a middle-ground language. It is lower-level than your source code, but higher-level than machine code.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Why do we need a middle ground?</h3>
        <p>Imagine writing a compiler that translates C++, Python, and Java into Windows, Mac, and Linux machine code. If you translate directly, you need 3 × 3 = 9 different translators.</p>
        <p>Instead, all languages translate into ONE common IR. Then, that ONE IR translates into the 3 operating systems. Now you only need 3 + 3 = 6 translators! This modularity is how modern compilers like LLVM work.</p>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Three-Address Code (TAC)</h3>
        <p>The most common form of IR is Three-Address Code. It breaks complex math into simple, bite-sized instructions that use at most three variables.</p>
        
        <div className="bg-muted p-4 rounded-lg font-mono text-sm my-4 border">
          <div><strong>Original Code:</strong> <code>x = a + b * c;</code></div>
          <div className="mt-4"><strong>Three-Address Code:</strong></div>
          <div className="pl-4 border-l-2 border-primary">
            <code>t1 = b * c</code><br/>
            <code>t2 = a + t1</code><br/>
            <code>x = t2</code>
          </div>
        </div>
        <p>Here, `t1` and `t2` are temporary variables generated by the compiler. This simple format is incredibly easy for the Optimizer to read and improve.</p>
      </div>
    )
  },
  "optimization": {
    title: "Optimization",
    icon: Scissors,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">Optimization is the magic phase. The compiler takes your IR and rewrites it to be faster and use less memory, all without changing the final output of your program.</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Common Techniques</h3>
        
        <div className="space-y-6">
          <div>
            <h4 className="font-bold text-foreground">1. Constant Folding</h4>
            <p className="text-sm mt-1 mb-2">If you write math that uses only numbers, the compiler solves it for you beforehand.</p>
            <div className="bg-muted p-3 rounded-md font-mono text-xs border">
              <span className="text-red-400 block">Before: x = 24 * 60 * 60;</span>
              <span className="text-green-500 block">After:  x = 86400;</span>
            </div>
          </div>

          <div>
            <h4 className="font-bold text-foreground">2. Dead Code Elimination</h4>
            <p className="text-sm mt-1 mb-2">The compiler deletes code that will never be executed.</p>
            <div className="bg-muted p-3 rounded-md font-mono text-xs border">
              <span className="text-red-400 block">Before: <br/>if (false) &#123; print("Hello"); &#125;</span>
              <span className="text-green-500 block">After:  <br/>{"// completely removed"}</span>
            </div>
          </div>
          
          <div>
            <h4 className="font-bold text-foreground">3. Copy Propagation</h4>
            <p className="text-sm mt-1 mb-2">Removing useless temporary variables.</p>
            <div className="bg-muted p-3 rounded-md font-mono text-xs border">
              <span className="text-red-400 block">Before: <br/>y = x;<br/>z = y + 1;</span>
              <span className="text-green-500 block">After:  <br/>z = x + 1;</span>
            </div>
          </div>
        </div>

        <p className="mt-6">A great optimizer is what makes languages like C and C++ run incredibly fast. It does the heavy lifting so programmers can write readable code without worrying about micro-optimizations.</p>
      </div>
    )
  },
  "code-generation": {
    title: "Code Generation",
    icon: Cpu,
    content: (
      <div className="space-y-6 text-muted-foreground leading-relaxed">
        <p className="text-lg text-foreground font-medium">This is the final destination. The Code Generator takes the Optimized IR and translates it into the actual machine code or assembly language for a specific CPU architecture (like x86 for Intel or ARM for Apple Silicon).</p>
        
        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Register Allocation</h3>
        <p>A CPU has "Registers"—tiny, lightning-fast memory slots built directly into the processor. However, there are very few of them (maybe 16 or 32).</p>
        <p>The IR code assumes we have an infinite number of temporary variables (t1, t2, t3...). The hardest job of the Code Generator is deciding which variables get to live in the fast CPU registers and which ones are pushed out to the slower RAM. This is a complex mathematical puzzle often solved using "Graph Coloring."</p>

        <h3 className="text-xl font-bold text-foreground mt-8 mb-4">Instruction Selection</h3>
        <p>Different CPUs have different special instructions. If you want to multiply a number by 2, a smart code generator won't use the standard `MUL` (multiply) instruction. Instead, it will use a `SHL` (bit-shift left) instruction, because on most CPUs, bit-shifting is significantly faster than multiplication.</p>

        <div className="bg-primary/10 border border-primary/20 p-6 rounded-xl mt-8">
          <h4 className="font-bold text-primary mb-2">The End Result</h4>
          <p className="text-sm">Once this phase is done, you have an executable file (`.exe` on Windows). You can run this file, and the computer's processor will blindly execute the 1s and 0s at billions of cycles per second, perfectly replicating the logic you typed into your readable source code.</p>
        </div>
      </div>
    )
  }
};

export const theoryOrder = [
  "introduction",
  "lexical-analysis",
  "automata",
  "syntax-analysis",
  "semantic-analysis",
  "intermediate-representation",
  "optimization",
  "code-generation"
];
