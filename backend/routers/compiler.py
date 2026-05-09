"""Compiler phase API endpoints."""
from fastapi import APIRouter, HTTPException
from backend.models.compiler import (
    SourceCodeRequest,
    LexicalResponse,
    SyntaxResponse,
    SemanticResponse,
    IntermediateResponse,
    OptimizeResponse,
    PipelineResponse,
)
from backend.services.engine import run_engine, EngineError

router = APIRouter(prefix="/api/compiler", tags=["compiler"])


@router.post("/lexical", response_model=LexicalResponse)
async def lexical_analysis(req: SourceCodeRequest):
    """Run lexical analysis on the source code."""
    try:
        result = await run_engine("lexical", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/syntax", response_model=SyntaxResponse)
async def syntax_analysis(req: SourceCodeRequest):
    """Run lexical + syntax analysis on the source code."""
    try:
        result = await run_engine("syntax", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/semantic", response_model=SemanticResponse)
async def semantic_analysis(req: SourceCodeRequest):
    """Run lexical + syntax + semantic analysis on the source code."""
    try:
        result = await run_engine("semantic", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/intermediate", response_model=IntermediateResponse)
async def intermediate_code(req: SourceCodeRequest):
    """Run through intermediate code generation."""
    try:
        result = await run_engine("intermediate", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/optimize", response_model=OptimizeResponse)
async def optimize_code(req: SourceCodeRequest):
    """Run through code optimization."""
    try:
        result = await run_engine("optimize", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/generate", response_model=PipelineResponse)
async def generate_code(req: SourceCodeRequest):
    """Run full pipeline including target code generation."""
    try:
        result = await run_engine("codegen", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/pipeline", response_model=PipelineResponse)
async def full_pipeline(req: SourceCodeRequest):
    """Run the complete compiler pipeline (all phases)."""
    try:
        result = await run_engine("pipeline", req.source_code)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))
