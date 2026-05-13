"use client";

import { useState, type ReactNode } from "react";
import Editor from "@monaco-editor/react";
import { AnimatePresence } from "framer-motion";
import { isAxiosError } from "axios";
import { Play, Loader2, RefreshCw, LayoutTemplate, TerminalSquare, FastForward } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { useTheme } from "next-themes";
import api from "@/lib/api";
import { toast } from "sonner";
import { ScrollArea } from "@/components/ui/scroll-area";
import { Card } from "@/components/ui/card";

const DEFAULT_CODE = `int a = 2;
int b = 3;
int c;
c = a + b * (10 - 4);
print(c);
`;

interface AstJsonNode {
  kind: string;
  lexeme?: string;
  tokenType?: string;
  line?: number;
  col?: number;
  layer?: string;
  type?: string;
  typeOk?: boolean;
  scopeDepth?: number;
  symbolKnownInitialized?: boolean;
  isConst?: boolean;
  constValue?: number | null;
  children?: AstJsonNode[];
}

interface TokenRow {
  type: string;
  lexeme: string;
  line: number;
  col: number;
}

interface SymbolRow {
  name: string;
  type: string;
  declared: boolean;
  initialized?: boolean;
}

interface IrInstrRow {
  op: string;
  res: string;
  arg1: string;
  arg2: string;
}

interface PipelineResponseBody {
  tokens?: TokenRow[];
  symbolTable?: SymbolRow[];
  literalTable?: number[];
  parseTree?: AstJsonNode | null;
  annotatedAst?: AstJsonNode | null;
  parseErrors?: string[];
  semanticErrors?: string[];
  ir?: IrInstrRow[];
  optimizedIr?: IrInstrRow[];
  targetCode?: string[];
  detail?: string;
  error?: string;
}

export default function PlaygroundPage() {
  const { theme } = useTheme();
  const [code, setCode] = useState(DEFAULT_CODE);
  const [isRunning, setIsRunning] = useState(false);
  const [result, setResult] = useState<PipelineResponseBody | null>(null);
  const [activeTab, setActiveTab] = useState("tokens");

  const handleRun = async () => {
    setIsRunning(true);
    setResult(null);
    try {
      const response = await api.post("/compiler/pipeline", { source_code: code });
      setResult(response.data as PipelineResponseBody);
      toast.success("Compilation successful");
    } catch (error: unknown) {
      if (isAxiosError(error)) {
        const data = error.response?.data as PipelineResponseBody | { detail?: string } | undefined;
        const detail =
          data && typeof data === "object" && "detail" in data && typeof data.detail === "string"
            ? data.detail
            : "Compilation failed";
        toast.error(detail);
        const body =
          data && typeof data === "object"
            ? (data as PipelineResponseBody)
            : ({ error: "Unknown error" } satisfies PipelineResponseBody);
        setResult(body);
        if (body.parseErrors?.length || body.semanticErrors?.length) {
          if (body.semanticErrors?.length) setActiveTab("semantic");
          else setActiveTab("syntax");
        }
      } else {
        toast.error("Compilation failed");
        setResult({ error: "Unknown error" });
      }
    } finally {
      setIsRunning(false);
    }
  };

  const renderTree = (
    node: AstJsonNode,
    depth = 0,
    prefix = "",
    isLast = true,
    childIndex = 0
  ): ReactNode => {
    if (!node) return null;
    const branch = depth === 0 ? "" : isLast ? "└─ " : "├─ ";
    const nextPrefix = depth === 0 ? "" : prefix + (isLast ? "   " : "│  ");
    const key = `${node.kind}-${node.line ?? 0}-${node.col ?? 0}-${depth}-${childIndex}`;
    const isSemantic = node.layer === "semantic";
    return (
      <div key={key} className="py-0.5 font-mono text-sm leading-relaxed">
        <div className="flex flex-wrap items-baseline gap-x-2 gap-y-1">
          <span className="text-muted-foreground whitespace-pre select-none">
            {prefix}
            {branch}
          </span>
          <span className="font-semibold text-primary">{node.kind}</span>
          {node.lexeme ? (
            <span className="bg-muted px-1.5 py-0.5 rounded text-xs">[{node.lexeme}]</span>
          ) : null}
          {node.type ? (
            <span className="text-xs text-violet-400">:{node.type}</span>
          ) : null}
          {isSemantic && node.typeOk === false ? (
            <span className="text-xs text-destructive">type error</span>
          ) : null}
          {isSemantic && typeof node.scopeDepth === "number" ? (
            <span className="text-xs text-sky-500/90">scope={node.scopeDepth}</span>
          ) : null}
          {isSemantic && node.kind === "Ident" && typeof node.symbolKnownInitialized === "boolean" ? (
            <span className={node.symbolKnownInitialized ? "text-xs text-emerald-500" : "text-xs text-amber-500"}>
              {node.symbolKnownInitialized ? "init✓" : "uninit"}
            </span>
          ) : null}
          {node.isConst && node.constValue != null ? (
            <span className="text-xs text-green-500">const={node.constValue}</span>
          ) : null}
          {node.layer === "syntax" ? (
            <span className="text-xs text-muted-foreground/80">syntax</span>
          ) : null}
        </div>
        {(node.children ?? []).map((child, i) =>
          renderTree(child, depth + 1, nextPrefix, i === (node.children?.length ?? 0) - 1, i)
        )}
      </div>
    );
  };

  return (
    <div className="flex flex-col h-[calc(100vh-4rem)] p-4 gap-4 overflow-hidden">
      <div className="flex items-center justify-between pb-2 border-b">
        <div>
          <h1 className="text-2xl font-bold tracking-tight flex items-center gap-2">
            <TerminalSquare className="w-6 h-6 text-primary" /> 
            Playground
          </h1>
          <p className="text-sm text-muted-foreground">Write C-like code and visualize every compiler phase.</p>
        </div>
        <div className="flex items-center gap-2">
           <Button variant="outline" size="sm" onClick={() => setCode(DEFAULT_CODE)}>
             <RefreshCw className="w-4 h-4 mr-2" /> Reset
           </Button>
           <Button 
             onClick={handleRun} 
             disabled={isRunning || !code.trim()}
             className="bg-primary hover:bg-primary/90 text-white min-w-[120px] shadow-lg shadow-primary/20"
           >
             {isRunning ? <Loader2 className="w-4 h-4 mr-2 animate-spin" /> : <Play className="w-4 h-4 mr-2 fill-current" />}
             {isRunning ? "Compiling..." : "Run Pipeline"}
           </Button>
        </div>
      </div>

      <div className="flex-1 flex flex-col lg:flex-row gap-4 min-h-0">
        {/* Editor Panel */}
        <Card className="flex-1 lg:w-1/2 flex flex-col overflow-hidden border-border/50 shadow-md">
          <div className="h-10 border-b bg-muted/30 flex items-center px-4 justify-between">
            <span className="text-sm font-mono text-muted-foreground flex items-center gap-2">
              <LayoutTemplate className="w-4 h-4" /> source.txt
            </span>
          </div>
          <div className="flex-1 min-h-0">
            <Editor
              height="100%"
              defaultLanguage="cpp"
              theme={theme === "dark" ? "vs-dark" : "light"}
              value={code}
              onChange={(value) => setCode(value || "")}
              options={{
                minimap: { enabled: false },
                fontSize: 14,
                fontFamily: "var(--font-mono)",
                lineHeight: 1.6,
                padding: { top: 16 },
                scrollBeyondLastLine: false,
                smoothScrolling: true,
                cursorBlinking: "smooth",
              }}
            />
          </div>
        </Card>

        {/* Output Panel */}
        <Card className="flex-1 lg:w-1/2 flex flex-col overflow-hidden border-border/50 shadow-md">
           {result ? (
            <Tabs value={activeTab} onValueChange={setActiveTab} className="flex-1 flex flex-col overflow-hidden">
              <div className="border-b px-2 overflow-x-auto custom-scrollbar">
                <TabsList className="h-12 bg-transparent justify-start w-max">
                  <TabsTrigger value="tokens" className="data-[state=active]:bg-muted/50 data-[state=active]:shadow-none rounded-none border-b-2 border-transparent data-[state=active]:border-primary transition-all">Tokens</TabsTrigger>
                  <TabsTrigger value="syntax" className="data-[state=active]:bg-muted/50 data-[state=active]:shadow-none rounded-none border-b-2 border-transparent data-[state=active]:border-primary transition-all">Syntax (AST)</TabsTrigger>
                  <TabsTrigger value="semantic" className="data-[state=active]:bg-muted/50 data-[state=active]:shadow-none rounded-none border-b-2 border-transparent data-[state=active]:border-primary transition-all">Semantic AST</TabsTrigger>
                  <TabsTrigger value="ir" className="data-[state=active]:bg-muted/50 data-[state=active]:shadow-none rounded-none border-b-2 border-transparent data-[state=active]:border-primary transition-all">IR Code</TabsTrigger>
                  <TabsTrigger value="target" className="data-[state=active]:bg-muted/50 data-[state=active]:shadow-none rounded-none border-b-2 border-transparent data-[state=active]:border-primary transition-all">Target Code</TabsTrigger>
                </TabsList>
              </div>

              <div className="flex-1 overflow-hidden relative bg-muted/10">
                <AnimatePresence mode="popLayout">
                  <ScrollArea className="h-full w-full">
                    <div className="p-4 font-mono text-sm leading-relaxed">
                      
                      {/* Tokens Tab */}
                      <TabsContent value="tokens" className="m-0 focus-visible:outline-none">
                         <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
                            <div>
                               <h3 className="font-bold mb-3 border-b pb-2">Tokens</h3>
                               {result.tokens?.map((t, i) => (
                                 <div key={i} className="flex justify-between py-1 border-b border-border/50 hover:bg-muted/50 px-2 rounded">
                                   <span className="text-primary">{t.type}</span>
                                   <span className="text-muted-foreground">{`"${t.lexeme}"`}</span>
                                 </div>
                               ))}
                            </div>
                            <div className="space-y-6">
                               <div>
                                  <h3 className="font-bold mb-3 border-b pb-2">Symbol Table</h3>
                                  {result.symbolTable?.map((s, i) => (
                                    <div key={i} className="flex justify-between py-1 border-b border-border/50 hover:bg-muted/50 px-2 rounded">
                                      <span className="font-semibold">{s.name}</span>
                                      <div className="flex flex-wrap gap-2 justify-end text-xs">
                                        <span className="text-blue-400">{s.type}</span>
                                        <span className={s.declared ? "text-green-500" : "text-red-500"}>{s.declared ? "declared" : "undeclared"}</span>
                                        <span className={s.initialized ? "text-emerald-500" : "text-amber-600"}>{s.initialized ? "initialized" : "not init"}</span>
                                      </div>
                                    </div>
                                  ))}
                               </div>
                               <div>
                                  <h3 className="font-bold mb-3 border-b pb-2">Literal Table</h3>
                                  <div className="flex flex-wrap gap-2">
                                    {result.literalTable?.map((l, i) => (
                                      <span key={i} className="bg-orange-500/10 text-orange-500 px-2 py-1 rounded">
                                        [{i}] {l}
                                      </span>
                                    ))}
                                  </div>
                               </div>
                            </div>
                         </div>
                      </TabsContent>

                      {/* Syntax / AST (pre-semantic snapshot from engine) */}
                      <TabsContent value="syntax" className="m-0 focus-visible:outline-none">
                         {Array.isArray(result.parseErrors) && result.parseErrors.length > 0 ? (
                            <div className="bg-destructive/10 text-destructive p-4 rounded-lg mb-4 border border-destructive/20">
                              <h3 className="font-bold mb-2">Syntax Errors:</h3>
                              <ul className="list-disc pl-5">
                                {result.parseErrors.map((e: string, i: number) => <li key={i}>{e}</li>)}
                              </ul>
                            </div>
                         ) : null}
                         <p className="text-xs text-muted-foreground mb-3">
                           Abstract syntax tree from the parser. Nodes are tagged <code className="text-[11px]">layer: syntax</code>
                           — structure and token metadata only (no types or symbol checks yet). Expressions are nested under statements.
                         </p>
                         {result.parseTree && (
                           <div className="bg-card p-4 rounded-lg border overflow-x-auto">
                             {renderTree(result.parseTree)}
                           </div>
                         )}
                      </TabsContent>

                      {/* Semantic / annotated AST */}
                      <TabsContent value="semantic" className="m-0 focus-visible:outline-none">
                         {Array.isArray(result.semanticErrors) && result.semanticErrors.length > 0 ? (
                            <div className="bg-destructive/10 text-destructive p-4 rounded-lg mb-4 border border-destructive/20">
                              <h3 className="font-bold mb-2">Semantic Errors:</h3>
                              <ul className="list-disc pl-5">
                                {result.semanticErrors.map((e: string, i: number) => <li key={i}>{e}</li>)}
                              </ul>
                            </div>
                         ) : (
                            <div className="bg-green-500/10 text-green-500 p-4 rounded-lg mb-4 border border-green-500/20">
                               Semantic analysis passed successfully. No errors found.
                            </div>
                         )}
                         <p className="text-xs text-muted-foreground mb-3">
                           Same tree shape as syntax, after semantic analysis. Nodes use <code className="text-[11px]">layer: semantic</code>
                           with inferred types, <span className="text-sky-500/90">scope depth</span>, identifier{" "}
                           <span className="text-emerald-500">init✓</span>/<span className="text-amber-500">uninit</span> state, and constant folding where possible.
                           Compare with the <strong>Syntax (AST)</strong> tab — the structure matches; annotations differ.
                         </p>
                         {(() => {
                           const tree = result.annotatedAst ?? result.parseTree;
                           if (!tree) return null;
                           return (
                           <div className="bg-card p-4 rounded-lg border overflow-x-auto">
                             <h3 className="font-bold mb-3 border-b pb-2 text-muted-foreground">Annotated AST</h3>
                             {renderTree(tree)}
                           </div>
                           );
                         })()}
                      </TabsContent>

                      {/* IR Tab */}
                      <TabsContent value="ir" className="m-0 focus-visible:outline-none">
                         <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
                            <div>
                               <h3 className="font-bold mb-3 border-b pb-2 text-muted-foreground">Original TAC</h3>
                               <div className="bg-card p-4 rounded-lg border">
                                  {result.ir?.map((ir, i) => (
                                     <div key={i} className="py-1 text-muted-foreground hover:text-foreground transition-colors">
                                        <span className="w-6 inline-block text-right mr-4 opacity-50">{i}:</span>
                                        {ir.op === 'PRINT' ? (
                                           <span className="text-blue-400">print <span className="text-foreground">{ir.arg1}</span></span>
                                        ) : ir.op === 'MOV' ? (
                                           <span><span className="text-foreground font-bold">{ir.res}</span> = {ir.arg1}</span>
                                        ) : (
                                           <span><span className="text-foreground font-bold">{ir.res}</span> = {ir.arg1} <span className="text-primary">{ir.op}</span> {ir.arg2}</span>
                                        )}
                                     </div>
                                  ))}
                               </div>
                            </div>
                            <div>
                               <h3 className="font-bold mb-3 border-b pb-2 text-green-500">Optimized TAC</h3>
                               <div className="bg-card border-green-500/20 border p-4 rounded-lg relative overflow-hidden">
                                  <div className="absolute top-0 right-0 p-2 opacity-10">
                                     <FastForward className="w-16 h-16 text-green-500" />
                                  </div>
                                  {result.optimizedIr?.map((ir, i) => (
                                     <div key={i} className="py-1 hover:text-foreground transition-colors relative z-10">
                                        <span className="w-6 inline-block text-right mr-4 opacity-50 text-green-500/50">{i}:</span>
                                        {ir.op === 'PRINT' ? (
                                           <span className="text-blue-400">print <span className="text-foreground">{ir.arg1}</span></span>
                                        ) : ir.op === 'MOV' ? (
                                           <span><span className="text-foreground font-bold">{ir.res}</span> = {ir.arg1}</span>
                                        ) : (
                                           <span><span className="text-foreground font-bold">{ir.res}</span> = {ir.arg1} <span className="text-green-500">{ir.op}</span> {ir.arg2}</span>
                                        )}
                                     </div>
                                  ))}
                               </div>
                            </div>
                         </div>
                      </TabsContent>

                      {/* Target Code Tab */}
                      <TabsContent value="target" className="m-0 focus-visible:outline-none">
                         <div className="bg-card p-6 rounded-lg border flex flex-col gap-2">
                           {result.targetCode?.map((code: string, i: number) => (
                             <div key={i} className="flex gap-4 font-mono group">
                                <span className="text-muted-foreground/50 w-8 text-right select-none group-hover:text-muted-foreground">{i}</span>
                                <span className="text-orange-400 group-hover:text-orange-300 transition-colors">
                                  {code.split(' ')[0]}
                                </span>
                                <span className="text-foreground">
                                  {code.split(' ').slice(1).join(' ')}
                                </span>
                             </div>
                           ))}
                         </div>
                      </TabsContent>

                    </div>
                  </ScrollArea>
                </AnimatePresence>
              </div>
            </Tabs>
           ) : (
             <div className="flex-1 flex flex-col items-center justify-center text-muted-foreground p-8 text-center animate-in fade-in duration-700">
               <div className="w-16 h-16 rounded-2xl bg-muted/50 flex items-center justify-center mb-4">
                 <Play className="w-8 h-8 opacity-20" />
               </div>
               <h3 className="text-lg font-semibold mb-2 text-foreground">Awaiting Execution</h3>
               <p className="max-w-sm">
                 Write your code in the editor and click &quot;Run Pipeline&quot; to see the compiler phases in action.
               </p>
             </div>
           )}
        </Card>
      </div>
    </div>
  );
}
