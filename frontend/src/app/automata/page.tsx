"use client";

import { useState } from "react";
import { Network, Play, RefreshCw, Layers } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Tabs, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Card } from "@/components/ui/card";
import { ScrollArea } from "@/components/ui/scroll-area";
import api from "@/lib/api";
import { toast } from "sonner";
import { isAxiosError } from "axios";
import {
  ReactFlow,
  Controls,
  Background,
  useNodesState,
  useEdgesState,
  MarkerType,
  Panel,
  Node,
  Edge,
} from "@xyflow/react";
import "@xyflow/react/dist/style.css";
import Dagre from "@dagrejs/dagre";

interface AutomatonTransition {
  from: number;
  to: number;
  symbol: string;
}

interface AutomatonGraph {
  start: number;
  stateCount: number;
  accept: number[];
  transitions: AutomatonTransition[];
}

interface DfaGraph extends AutomatonGraph {
  subsets?: Record<string, number[]>;
}

interface GrammarProduction {
  lhs: string;
  rhs: string;
}

interface Grammar {
  startSymbol: string;
  productions: GrammarProduction[];
}

interface DerivationsResponse {
  nfa: AutomatonGraph;
  dfa: DfaGraph;
  grammar?: Grammar;
  leftmostDerivation?: string[];
  rightmostDerivation?: string[];
}

interface EdgeAgg {
  source: string;
  target: string;
  symbols: string[];
  isEpsilon: boolean;
}

export default function AutomataPage() {
  const [regex, setRegex] = useState("(a|b)*abb");
  const [testString, setTestString] = useState("ababb");
  const [isRunning, setIsRunning] = useState(false);
  const [result, setResult] = useState<DerivationsResponse | null>(null);
  const [activeTab, setActiveTab] = useState("nfa");

  // React Flow state
  const [nodes, setNodes, onNodesChange] = useNodesState<Node>([]);
  const [edges, setEdges, onEdgesChange] = useEdgesState<Edge>([]);

  const generateLayout = (nodesData: Node[], edgesData: Edge[]) => {
    const g = new Dagre.graphlib.Graph().setDefaultEdgeLabel(() => ({}));
    g.setGraph({ rankdir: 'LR', nodesep: 50, ranksep: 100, marginx: 50, marginy: 50 });

    nodesData.forEach((node) => {
      g.setNode(node.id, { width: 50, height: 50 });
    });

    edgesData.forEach((edge) => {
      g.setEdge(edge.source, edge.target);
    });

    Dagre.layout(g);

    return nodesData.map((node) => {
      const nodeWithPosition = g.node(node.id);
      return {
        ...node,
        position: { x: nodeWithPosition.x - 25, y: nodeWithPosition.y - 25 },
      };
    });
  };

  const processAutomata = (automata: AutomatonGraph) => {
    if (!automata) return;

    const initialNodes = Array.from({ length: automata.stateCount }, (_, i) => {
      const isStart = i === automata.start;
      const isAccept = automata.accept.includes(i);
      
      let borderClass = "!bg-card !text-foreground border-border";
      if (isStart) borderClass = "!bg-primary/10 border-primary border-2 !text-primary";
      if (isAccept) borderClass = "!bg-green-500/10 border-green-500 border-4 !text-green-500";
      if (isStart && isAccept) borderClass = "!bg-fuchsia-500/10 border-fuchsia-500 border-4 !text-fuchsia-500";

      return {
        id: `${i}`,
        data: { label: `${i}` },
        position: { x: 0, y: 0 },
        className: `rounded-full w-12 h-12 flex items-center justify-center font-bold shadow-md ${borderClass}`,
      };
    });

    const edgeMap = new Map<string, EdgeAgg>();

    automata.transitions.forEach((t) => {
      const source = `${t.from}`;
      const target = `${t.to}`;
      const key = `${source}-${target}`;
      const sym = t.symbol === "ε" ? "ε" : t.symbol;

      const existing = edgeMap.get(key);
      if (existing) {
        existing.symbols.push(sym);
      } else {
        edgeMap.set(key, { source, target, symbols: [sym], isEpsilon: sym === "ε" });
      }
    });

    const initialEdges = Array.from(edgeMap.values()).map((e, i) => {
      const label = e.symbols.join(", ");
      return {
        id: `e${i}`,
        source: e.source,
        target: e.target,
        label: label,
        animated: e.isEpsilon,
        type: 'smoothstep',
        style: { stroke: e.isEpsilon ? 'var(--muted-foreground)' : 'var(--primary)', strokeWidth: 2, strokeDasharray: e.isEpsilon ? '5,5' : 'none' },
        labelStyle: { fill: 'var(--foreground)', fontWeight: 600, fontSize: 14 },
        labelBgStyle: { fill: 'var(--background)', fillOpacity: 0.8 },
        markerEnd: { type: MarkerType.ArrowClosed, color: e.isEpsilon ? 'var(--muted-foreground)' : 'var(--primary)' },
      };
    });

    const layoutedNodes = generateLayout(initialNodes as Node[], initialEdges as Edge[]);
    setNodes(layoutedNodes);
    setEdges(initialEdges);
  };

  const handleRun = async () => {
    setIsRunning(true);
    setResult(null);
    try {
      const response = await api.post("/automata/derivations", { regex, test_string: testString });
      const data = response.data as DerivationsResponse;
      setResult(data);
      processAutomata(data.nfa);
      setActiveTab("nfa");
      toast.success("Automata generated");
    } catch (error: unknown) {
      const msg =
        isAxiosError(error) &&
        error.response?.data &&
        typeof (error.response.data as { detail?: string }).detail === "string"
          ? (error.response.data as { detail: string }).detail
          : "Failed to generate automata";
      toast.error(msg);
    } finally {
      setIsRunning(false);
    }
  };

  const switchGraph = (tab: string) => {
    setActiveTab(tab);
    if (!result) return;
    if (tab === "nfa" && result.nfa) processAutomata(result.nfa);
    if (tab === "dfa" && result.dfa) processAutomata(result.dfa);
  };

  return (
    <div className="flex flex-col h-[calc(100vh-4rem)] p-4 gap-4 overflow-hidden">
      <div className="flex flex-col md:flex-row items-start md:items-center justify-between pb-4 border-b gap-4">
        <div>
          <h1 className="text-2xl font-bold tracking-tight flex items-center gap-2">
            <Network className="w-6 h-6 text-primary" /> 
            Automata Visualizer
          </h1>
          <p className="text-sm text-muted-foreground">Convert Regular Expressions to NFA, DFA, and Grammars.</p>
        </div>
        
        <div className="flex flex-wrap items-end gap-3 bg-muted/30 p-2 rounded-xl border w-full md:w-auto">
          <div className="grid w-full max-w-xs items-center gap-1.5">
            <Label htmlFor="regex" className="text-xs font-semibold px-1">Regular Expression</Label>
            <Input id="regex" value={regex} onChange={e => setRegex(e.target.value)} placeholder="(a|b)*abb" className="h-9 bg-background" />
          </div>
          <div className="grid w-full max-w-[150px] items-center gap-1.5">
            <Label htmlFor="string" className="text-xs font-semibold px-1">Test String</Label>
            <Input id="string" value={testString} onChange={e => setTestString(e.target.value)} placeholder="ababb" className="h-9 bg-background" />
          </div>
          <Button 
            onClick={handleRun} 
            disabled={isRunning || !regex.trim()}
            className="h-9 w-full sm:w-auto bg-primary hover:bg-primary/90 text-white shadow-lg shadow-primary/20"
          >
            {isRunning ? <RefreshCw className="w-4 h-4 mr-2 animate-spin" /> : <Play className="w-4 h-4 mr-2 fill-current" />}
            Generate
          </Button>
        </div>
      </div>

      <div className="flex-1 flex flex-col lg:flex-row gap-4 min-h-0">
        
        {/* Graph Area */}
        <Card className="flex-1 lg:w-[65%] flex flex-col overflow-hidden border-border/50 shadow-md">
           {result ? (
             <Tabs value={activeTab} onValueChange={switchGraph} className="flex-1 flex flex-col overflow-hidden">
                <div className="border-b px-2 z-10 bg-card">
                  <TabsList className="h-12 bg-transparent justify-start w-full">
                    <TabsTrigger value="nfa" className="data-[state=active]:bg-muted/50 rounded-none border-b-2 border-transparent data-[state=active]:border-primary px-6">NFA</TabsTrigger>
                    <TabsTrigger value="dfa" className="data-[state=active]:bg-muted/50 rounded-none border-b-2 border-transparent data-[state=active]:border-primary px-6">DFA</TabsTrigger>
                  </TabsList>
                </div>
                <div className="flex-1 bg-muted/10 relative">
                   <ReactFlow
                      nodes={nodes}
                      edges={edges}
                      onNodesChange={onNodesChange}
                      onEdgesChange={onEdgesChange}
                      fitView
                      minZoom={0.2}
                      maxZoom={4}
                   >
                     <Background color="var(--muted-foreground)" gap={20} size={1} className="opacity-20" />
                     <Controls className="bg-background border border-border shadow-md rounded-md overflow-hidden" />
                     <Panel position="bottom-center" className="bg-background/80 backdrop-blur border p-2 rounded-lg text-xs flex gap-4 text-muted-foreground font-medium mb-4">
                       <div className="flex items-center gap-1"><div className="w-3 h-3 rounded-full bg-primary/20 border border-primary"></div> Start</div>
                       <div className="flex items-center gap-1"><div className="w-3 h-3 rounded-full bg-green-500/20 border-2 border-green-500"></div> Accept</div>
                       <div className="flex items-center gap-1"><div className="w-4 h-0 border-t-2 border-dashed border-muted-foreground"></div> Epsilon (ε)</div>
                     </Panel>
                   </ReactFlow>
                </div>
             </Tabs>
           ) : (
              <div className="flex-1 flex items-center justify-center bg-muted/10">
                 <div className="text-center text-muted-foreground">
                    <Network className="w-12 h-12 mx-auto mb-4 opacity-20" />
                    <p>Enter a regex and click Generate to view the automata graphs.</p>
                 </div>
              </div>
           )}
        </Card>

        {/* Info Area */}
        <Card className="flex-1 lg:w-[35%] flex flex-col overflow-hidden border-border/50 shadow-md">
           <div className="h-12 border-b bg-muted/30 flex items-center px-4">
             <h3 className="font-semibold text-sm flex items-center gap-2"><Layers className="w-4 h-4 text-primary" /> Theory & Analysis</h3>
           </div>
           
           {result ? (
             <ScrollArea className="flex-1 p-0">
                
                <div className="p-4 border-b">
                   <h4 className="text-xs font-bold uppercase text-muted-foreground mb-3">Regular Grammar</h4>
                   <div className="bg-card border rounded-lg p-3 font-mono text-sm">
                      <div className="text-primary mb-2">Start: {result.grammar?.startSymbol}</div>
                      <div className="max-h-[200px] overflow-y-auto custom-scrollbar pr-2">
                        {result.grammar?.productions?.map((p, i) => (
                           <div key={i} className="flex gap-2 py-0.5">
                             <span className="text-foreground w-6 text-right">{p.lhs}</span>
                             <span className="text-muted-foreground">→</span>
                             <span className="text-green-500">{p.rhs}</span>
                           </div>
                        ))}
                      </div>
                   </div>
                </div>

                {activeTab === 'dfa' && result.dfa?.subsets && (
                   <div className="p-4 border-b">
                      <h4 className="text-xs font-bold uppercase text-muted-foreground mb-3">DFA Subsets (NFA States)</h4>
                      <div className="bg-card border rounded-lg p-3 font-mono text-xs flex flex-col gap-1 max-h-[150px] overflow-y-auto custom-scrollbar">
                         {(Object.entries(result.dfa.subsets) as [string, number[]][]).map(([dfaState, nfaStates]) => (
                            <div key={dfaState} className="flex gap-2">
                               <span className="text-primary w-4">{dfaState}</span>
                               <span className="text-muted-foreground">=</span>
                               <span className="text-foreground">{`{ ${nfaStates.join(', ')} }`}</span>
                            </div>
                         ))}
                      </div>
                   </div>
                )}

                <div className="p-4">
                   <h4 className="text-xs font-bold uppercase text-muted-foreground mb-3">
                     Derivations for {`"${testString}"`}
                   </h4>
                   
                   <div className="mb-4">
                      <div className="text-sm font-semibold mb-2">Leftmost Derivation</div>
                      {Array.isArray(result.leftmostDerivation) && result.leftmostDerivation.length > 0 ? (
                          <div className="bg-card border rounded-lg p-3 font-mono text-sm max-h-[150px] overflow-y-auto custom-scrollbar flex flex-col gap-1">
                            {result.leftmostDerivation.map((step, i) => (
                               <div key={i}>
                                  {i > 0 && <span className="text-muted-foreground mr-2 opacity-50 text-xs">⇒</span>}
                                  {step}
                               </div>
                            ))}
                         </div>
                      ) : (
                         <div className="text-sm text-red-500 bg-red-500/10 p-2 rounded border border-red-500/20">
                            String rejected by Automata
                         </div>
                      )}
                   </div>

                   <div>
                      <div className="text-sm font-semibold mb-2">Rightmost Derivation</div>
                      {Array.isArray(result.rightmostDerivation) && result.rightmostDerivation.length > 0 ? (
                          <div className="bg-card border rounded-lg p-3 font-mono text-sm max-h-[150px] overflow-y-auto custom-scrollbar flex flex-col gap-1">
                            {result.rightmostDerivation.map((step, i) => (
                               <div key={i}>
                                  {i > 0 && <span className="text-muted-foreground mr-2 opacity-50 text-xs">⇒</span>}
                                  {step}
                               </div>
                            ))}
                         </div>
                      ) : (
                         <div className="text-sm text-red-500 bg-red-500/10 p-2 rounded border border-red-500/20">
                            String rejected by Automata
                         </div>
                      )}
                   </div>
                </div>

             </ScrollArea>
           ) : (
              <div className="flex-1 flex items-center justify-center text-muted-foreground p-6 text-center text-sm">
                 Generate an automata to view the resulting grammar, DFA subsets, and string derivations.
              </div>
           )}
        </Card>
      </div>
    </div>
  );
}
