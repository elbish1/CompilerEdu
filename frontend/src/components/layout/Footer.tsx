export function Footer() {
  return (
    <footer className="bg-muted/30 border-t py-12 mt-auto">
      <div className="container mx-auto px-4 flex flex-col md:flex-row items-center justify-between gap-6">
        <div className="flex flex-col items-center md:items-start gap-1">
          <div className="flex items-center gap-2">
            <div className="bg-primary/10 p-1.5 rounded-md">
              <svg
                xmlns="http://www.w3.org/2000/svg"
                viewBox="0 0 24 24"
                fill="none"
                stroke="currentColor"
                strokeWidth="2"
                strokeLinecap="round"
                strokeLinejoin="round"
                className="w-5 h-5 text-primary"
              >
                <polyline points="16 18 22 12 16 6" />
                <polyline points="8 6 2 12 8 18" />
              </svg>
            </div>
            <span className="font-semibold tracking-tight text-lg">CompilerEdu</span>
          </div>
          <p className="text-xs text-muted-foreground mt-2">
            Built for educational purposes. Faculty of Computers & Artificial Intelligence, RST.
          </p>
          <p className="text-xs text-muted-foreground mt-1">
            Developed by <span className="font-semibold text-foreground">Yousef Amr Abdelazeem Elbish</span>
          </p>
        </div>

        <div className="flex flex-wrap justify-center gap-4 text-sm font-medium">
          <a href="https://github.com/elbish1" target="_blank" rel="noopener noreferrer" className="text-muted-foreground hover:text-primary transition-colors flex items-center gap-1">
             GitHub
          </a>
          <a href="https://www.linkedin.com/in/elbish1" target="_blank" rel="noopener noreferrer" className="text-muted-foreground hover:text-primary transition-colors flex items-center gap-1">
             LinkedIn
          </a>
          <a href="https://yousef-elbish.vercel.app" target="_blank" rel="noopener noreferrer" className="text-muted-foreground hover:text-primary transition-colors flex items-center gap-1">
             Portfolio
          </a>
          <a href="mailto:yousefelbish@gmail.com" className="text-muted-foreground hover:text-primary transition-colors flex items-center gap-1">
             Email
          </a>
        </div>
      </div>
    </footer>
  );
}
