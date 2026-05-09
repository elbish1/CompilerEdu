"use client";

import { motion } from "framer-motion";
import { Button } from "@/components/ui/button";
import Link from "next/link";
import { ArrowRight, Code2, Network, BookOpen, TerminalSquare, Layers, Cpu, FastForward } from "lucide-react";

export default function Home() {
  const features = [
    {
      title: "Interactive Playground",
      description: "Write code in a VS Code-style editor and see the compiler pipeline execute in real-time.",
      icon: TerminalSquare,
      href: "/playground",
      color: "text-blue-500",
      bg: "bg-blue-500/10",
    },
    {
      title: "Automata Visualizer",
      description: "Convert Regex to NFA and DFA with beautiful, animated interactive graphs.",
      icon: Network,
      href: "/automata",
      color: "text-purple-500",
      bg: "bg-purple-500/10",
    },
    {
      title: "Deep Theory",
      description: "Learn the inner workings of compilers from Lexical Analysis to Target Code generation.",
      icon: BookOpen,
      href: "/theory",
      color: "text-green-500",
      bg: "bg-green-500/10",
    },
  ];

  const pipeline = [
    { name: "Lexical Analysis", icon: Code2 },
    { name: "Syntax Analysis", icon: Layers },
    { name: "Semantic Analysis", icon: BookOpen },
    { name: "IR Generation", icon: TerminalSquare },
    { name: "Optimization", icon: FastForward },
    { name: "Code Generation", icon: Cpu },
  ];

  return (
    <div className="flex flex-col min-h-screen">
      {/* Hero Section */}
      <section className="relative pt-32 pb-20 md:pt-40 md:pb-28 overflow-hidden">
        {/* Background Gradients */}
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[800px] h-[800px] bg-primary/20 rounded-full blur-[120px] opacity-50 pointer-events-none" />
        <div className="absolute top-0 right-0 w-[500px] h-[500px] bg-fuchsia-500/20 rounded-full blur-[100px] opacity-40 pointer-events-none" />
        
        <div className="container mx-auto px-4 relative z-10">
          <div className="flex flex-col items-center text-center max-w-4xl mx-auto">
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5 }}
              className="inline-flex items-center gap-2 px-3 py-1 rounded-full bg-muted/50 border border-border mb-8 backdrop-blur-sm"
            >
              <span className="flex h-2 w-2 rounded-full bg-primary animate-pulse" />
              <span className="text-sm font-medium">Powered by C++ Engine • Built with FastAPI & Next.js</span>
            </motion.div>

            <motion.h1
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: 0.1 }}
              className="text-5xl md:text-7xl font-extrabold tracking-tight mb-6"
            >
              Master Compilers <br className="hidden md:block" />
              <span className="text-gradient">Visually</span>
            </motion.h1>

            <motion.p
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: 0.2 }}
              className="text-lg md:text-xl text-muted-foreground mb-10 max-w-2xl"
            >
              An interactive, premium educational platform to understand how a compiler thinks. 
              Write code, visualize the AST, step through IR optimization, and simulate DFA state machines.
            </motion.p>

            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: 0.3 }}
              className="flex flex-col sm:flex-row gap-4 w-full sm:w-auto"
            >
              <Link href="/playground" className="w-full sm:w-auto">
                <Button size="lg" className="w-full sm:w-auto text-base h-12 px-8 rounded-full shadow-lg shadow-primary/25">
                  Open Playground <ArrowRight className="ml-2 w-4 h-4" />
                </Button>
              </Link>
              <Link href="/automata" className="w-full sm:w-auto">
                <Button variant="outline" size="lg" className="w-full sm:w-auto text-base h-12 px-8 rounded-full glass">
                  Automata Visualizer
                </Button>
              </Link>
            </motion.div>
          </div>
        </div>
      </section>

      {/* Features Section */}
      <section className="py-20 bg-muted/30 border-y relative">
        <div className="container mx-auto px-4">
          <div className="text-center mb-16">
            <h2 className="text-3xl md:text-4xl font-bold mb-4">Everything you need to learn</h2>
            <p className="text-muted-foreground max-w-2xl mx-auto">
              We've transformed the classic compiler design pipeline into a beautiful, interactive web experience.
            </p>
          </div>

          <div className="grid md:grid-cols-3 gap-8 max-w-5xl mx-auto">
            {features.map((feature, i) => {
              const Icon = feature.icon;
              return (
                <motion.div
                  key={feature.title}
                  initial={{ opacity: 0, y: 20 }}
                  whileInView={{ opacity: 1, y: 0 }}
                  viewport={{ once: true }}
                  transition={{ duration: 0.5, delay: i * 0.1 }}
                  className="glass-card p-6 rounded-2xl glow-on-hover group"
                >
                  <div className={`w-12 h-12 rounded-xl flex items-center justify-center mb-6 ${feature.bg}`}>
                    <Icon className={`w-6 h-6 ${feature.color}`} />
                  </div>
                  <h3 className="text-xl font-bold mb-3">{feature.title}</h3>
                  <p className="text-muted-foreground mb-6 line-clamp-3">
                    {feature.description}
                  </p>
                  <Link href={feature.href} className="inline-flex items-center text-sm font-semibold text-primary hover:text-primary/80 transition-colors">
                    Explore <ArrowRight className="ml-1 w-4 h-4 transition-transform group-hover:translate-x-1" />
                  </Link>
                </motion.div>
              );
            })}
          </div>
        </div>
      </section>

      {/* Compiler Pipeline Visualizer Preview */}
      <section className="py-24 overflow-hidden">
        <div className="container mx-auto px-4">
          <div className="flex flex-col lg:flex-row items-center gap-16">
            <div className="lg:w-1/2">
              <h2 className="text-3xl md:text-4xl font-bold mb-6">Experience the Pipeline</h2>
              <p className="text-lg text-muted-foreground mb-8">
                Watch your source code transform step-by-step. From raw characters to tokens, to a parse tree, through optimization, and finally to target machine code.
              </p>
              
              <div className="space-y-4">
                {pipeline.map((phase, i) => (
                  <motion.div 
                    key={phase.name}
                    initial={{ opacity: 0, x: -20 }}
                    whileInView={{ opacity: 1, x: 0 }}
                    viewport={{ once: true }}
                    transition={{ duration: 0.4, delay: i * 0.1 }}
                    className="flex items-center gap-4 p-3 rounded-xl hover:bg-muted/50 transition-colors"
                  >
                    <div className="bg-background border shadow-sm w-10 h-10 rounded-lg flex items-center justify-center shrink-0 text-primary">
                      <phase.icon className="w-5 h-5" />
                    </div>
                    <span className="font-semibold text-lg">{phase.name}</span>
                  </motion.div>
                ))}
              </div>
            </div>
            
            <div className="lg:w-1/2 w-full">
              <motion.div 
                initial={{ opacity: 0, scale: 0.9 }}
                whileInView={{ opacity: 1, scale: 1 }}
                viewport={{ once: true }}
                transition={{ duration: 0.6 }}
                className="relative aspect-square md:aspect-video lg:aspect-square rounded-2xl border bg-card shadow-2xl overflow-hidden glass-card"
              >
                {/* Simulated IDE window */}
                <div className="absolute top-0 w-full h-10 border-b bg-muted/50 flex items-center px-4 gap-2">
                  <div className="w-3 h-3 rounded-full bg-red-500/80"></div>
                  <div className="w-3 h-3 rounded-full bg-yellow-500/80"></div>
                  <div className="w-3 h-3 rounded-full bg-green-500/80"></div>
                  <div className="mx-auto text-xs font-mono text-muted-foreground">compiler_app.exe</div>
                </div>
                <div className="pt-14 p-6 font-mono text-sm">
                  <div className="text-blue-400">int</div> <span className="text-foreground">a</span> <span className="text-primary">=</span> <span className="text-orange-400">2</span><span className="text-foreground">;</span><br/>
                  <div className="text-blue-400">int</div> <span className="text-foreground">b</span> <span className="text-primary">=</span> <span className="text-orange-400">3</span><span className="text-foreground">;</span><br/>
                  <div className="text-blue-400">print</div><span className="text-foreground">(a</span> <span className="text-primary">+</span> <span className="text-foreground">b);</span>
                  
                  <div className="mt-8 pt-6 border-t border-dashed opacity-70">
                    <div className="text-green-400 mb-2">// Intermediate Code (IR)</div>
                    <div>MOV a = 2</div>
                    <div>MOV b = 3</div>
                    <div>ADD t1 = a b</div>
                    <div>PRINT t1</div>
                  </div>
                </div>
              </motion.div>
            </div>
          </div>
        </div>
      </section>
    </div>
  );
}
