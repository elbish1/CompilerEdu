import { create } from "zustand";

interface AppState {
  isSidebarOpen: boolean;
  toggleSidebar: () => void;
  compilerTheme: string;
  setCompilerTheme: (theme: string) => void;
}

export const useAppStore = create<AppState>((set) => ({
  isSidebarOpen: false,
  toggleSidebar: () => set((state) => ({ isSidebarOpen: !state.isSidebarOpen })),
  compilerTheme: "vs-dark",
  setCompilerTheme: (theme) => set({ compilerTheme: theme }),
}));
