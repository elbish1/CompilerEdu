"""
FastAPI application – Educational Compiler Visualization Platform.

Run with:
    uvicorn backend.main:app --reload --port 8000
"""
import logging
from contextlib import asynccontextmanager

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from backend.routers import compiler, automata, expression, health
from backend.services.engine import _ensure_binary, EngineError

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
logger = logging.getLogger(__name__)


@asynccontextmanager
async def lifespan(app: FastAPI):
    """Compile the C++ engine on startup if needed."""
    logger.info("Starting up – checking C++ engine binary...")
    try:
        binary = await _ensure_binary()
        logger.info(f"C++ engine ready at {binary}")
    except EngineError as e:
        logger.warning(f"Could not prepare C++ engine: {e}")
    yield
    logger.info("Shutting down.")


app = FastAPI(
    title="Compiler Visualization Platform API",
    description=(
        "Backend API for the Educational Compiler Design & Automata Theory platform. "
        "Provides endpoints for lexical analysis, syntax analysis, semantic analysis, "
        "intermediate code generation, optimization, code generation, automata "
        "construction, and expression evaluation."
    ),
    version="1.0.0",
    lifespan=lifespan,
)

# CORS – allow the Next.js dev server and production origins
app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "http://localhost:3000",
        "http://127.0.0.1:3000",
        "http://localhost:3001",
        "http://localhost:8001",
        "*" # Allow all for local dev ease, this is a local tool
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Register routers
app.include_router(compiler.router)
app.include_router(automata.router)
app.include_router(expression.router)
app.include_router(health.router)


@app.get("/")
async def root():
    return {
        "name": "Compiler Visualization Platform API",
        "version": "1.0.0",
        "docs": "/docs",
    }
