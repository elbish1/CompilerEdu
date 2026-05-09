"use client";

import { useParams } from "next/navigation";
import { theoryData, theoryOrder } from "../data";
import { motion } from "framer-motion";
import Link from "next/link";
import { ArrowLeft, ArrowRight, ChevronLeft } from "lucide-react";
import { Button } from "@/components/ui/button";

export default function TheoryArticlePage() {
  const params = useParams();
  const slug = params.slug as string;
  const article = theoryData[slug];

  if (!article) {
    return (
      <div className="container mx-auto p-12 text-center">
        <h1 className="text-2xl font-bold mb-4">Article Not Found</h1>
        <Link href="/theory">
          <Button variant="outline">Back to Theory</Button>
        </Link>
      </div>
    );
  }

  const currentIndex = theoryOrder.indexOf(slug);
  const prevSlug = currentIndex > 0 ? theoryOrder[currentIndex - 1] : null;
  const nextSlug = currentIndex < theoryOrder.length - 1 ? theoryOrder[currentIndex + 1] : null;

  return (
    <div className="container mx-auto px-4 py-12 max-w-4xl">
      <Link href="/theory" className="inline-flex items-center text-muted-foreground hover:text-foreground transition-colors mb-8">
        <ChevronLeft className="w-4 h-4 mr-1" /> Back to Theory Overview
      </Link>
      
      <motion.div
        initial={{ opacity: 0, y: 10 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ duration: 0.4 }}
      >
        <div className="flex items-center gap-4 mb-8">
          <div className="p-3 bg-primary/10 rounded-xl text-primary">
            <article.icon className="w-8 h-8" />
          </div>
          <h1 className="text-3xl md:text-5xl font-bold tracking-tight">{article.title}</h1>
        </div>
        
        <div className="prose prose-invert prose-p:text-muted-foreground prose-h3:text-foreground max-w-none mb-12">
          {article.content}
        </div>

        <div className="flex justify-between items-center pt-8 border-t border-border/50">
          <div>
            {prevSlug && (
              <Link href={`/theory/${prevSlug}`}>
                <Button variant="ghost" className="text-muted-foreground hover:text-foreground">
                  <ArrowLeft className="w-4 h-4 mr-2" /> {theoryData[prevSlug].title}
                </Button>
              </Link>
            )}
          </div>
          <div>
            {nextSlug && (
              <Link href={`/theory/${nextSlug}`}>
                <Button className="bg-primary text-white hover:bg-primary/90">
                  {theoryData[nextSlug].title} <ArrowRight className="w-4 h-4 ml-2" />
                </Button>
              </Link>
            )}
          </div>
        </div>
      </motion.div>
    </div>
  );
}
