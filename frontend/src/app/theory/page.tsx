"use client";

import { motion } from "framer-motion";
import { BookOpen, TerminalSquare, Network } from "lucide-react";
import Link from "next/link";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";

export default function TheoryPage() {
  const sections = [
    {
      title: "Introduction",
      description: "What is a compiler? Why do we need it? And what is the overall pipeline?",
      details: "A high-level overview of language translation, interpreting vs. compiling, and the phases of modern compilers.",
      icon: BookOpen,
      slug: "introduction",
    },
    {
      title: "Lexical Analysis",
      description: "The first phase of a compiler. It reads the raw source code characters and groups them into meaningful sequences called tokens.",
      details: "It ignores whitespace and comments, and builds a symbol table of identifiers and a literal table of constants.",
      icon: TerminalSquare,
      slug: "lexical-analysis",
    },
    {
      title: "Automata Theory",
      description: "The mathematical foundation (NFA and DFA) that powers lexical analysis.",
      details: "How regular expressions are converted into state machines that execute blazingly fast.",
      icon: Network,
      slug: "automata",
    },
    {
      title: "Syntax Analysis (Parsing)",
      description: "Takes the token stream and verifies that it follows the grammatical rules of the language.",
      details: "It builds a Parse Tree or Abstract Syntax Tree (AST) that represents the hierarchical syntactic structure of the program.",
      icon: BookOpen,
      slug: "syntax-analysis",
    },
    {
      title: "Semantic Analysis",
      description: "Checks the AST for semantic errors to ensure the program's meaning is valid.",
      details: "Checks include type checking, verifying variables are declared before use, and identifying unreachable code. It annotates the AST with this information.",
      icon: BookOpen,
      slug: "semantic-analysis",
    },
    {
      title: "Intermediate Representation (IR)",
      description: "Translates the annotated AST into a lower-level, machine-independent representation.",
      details: "Common forms include Three-Address Code (TAC), which breaks complex expressions into simple instructions with at most three operands.",
      icon: TerminalSquare,
      slug: "intermediate-representation",
    },
    {
      title: "Optimization",
      description: "Improves the IR to make the final code run faster or use less memory, without changing its meaning.",
      details: "Techniques include constant folding, dead code elimination, and loop unrolling.",
      icon: Network,
      slug: "optimization",
    },
    {
      title: "Code Generation",
      description: "The final phase. Translates the optimized IR into target machine code or assembly language.",
      details: "It handles register allocation and specific machine instruction selection.",
      icon: TerminalSquare,
      slug: "code-generation",
    },
  ];

  return (
    <div className="container mx-auto px-4 py-12">
      <motion.div
        initial={{ opacity: 0, y: 20 }}
        animate={{ opacity: 1, y: 0 }}
        className="max-w-4xl mx-auto"
      >
        <div className="text-center mb-16">
          <h1 className="text-4xl md:text-5xl font-bold mb-4 tracking-tight">Compiler Theory</h1>
          <p className="text-lg text-muted-foreground">
            Understand the journey of source code from plain text to executable machine instructions.
          </p>
        </div>

        <div className="space-y-8 relative before:absolute before:inset-0 before:ml-5 before:-translate-x-px md:before:mx-auto md:before:translate-x-0 before:h-full before:w-0.5 before:bg-gradient-to-b before:from-transparent before:via-primary/50 before:to-transparent">
          {sections.map((section, index) => (
            <div key={index} className="relative flex items-center justify-between md:justify-normal md:odd:flex-row-reverse group is-active">
              {/* Timeline dot */}
              <div className="flex items-center justify-center w-10 h-10 rounded-full border-4 border-background bg-primary text-primary-foreground shadow shrink-0 md:order-1 md:group-odd:-translate-x-1/2 md:group-even:translate-x-1/2 z-10">
                <section.icon className="w-4 h-4" />
              </div>
              
              {/* Card */}
              <Card className="w-[calc(100%-4rem)] md:w-[calc(50%-2.5rem)] glass-card glow-on-hover p-0 overflow-hidden flex flex-col h-full">
                 <div className="h-1 w-full bg-gradient-to-r from-primary to-fuchsia-500"></div>
                 <CardHeader className="flex-1">
                  <CardTitle className="flex items-center gap-2">
                    <span className="text-primary font-mono text-sm opacity-80">0{index + 1}</span>
                    {section.title}
                  </CardTitle>
                  <CardDescription>{section.description}</CardDescription>
                </CardHeader>
                <CardContent className="flex flex-col gap-4">
                  <p className="text-sm text-muted-foreground">{section.details}</p>
                  <Link href={`/theory/${section.slug}`} className="w-full">
                    <Button variant="secondary" className="w-full mt-2 group-hover:bg-primary group-hover:text-primary-foreground transition-colors">
                      Read Full Guide
                    </Button>
                  </Link>
                </CardContent>
              </Card>
            </div>
          ))}
        </div>
      </motion.div>
    </div>
  );
}
