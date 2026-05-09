"""Automata API endpoints – regex to NFA/DFA, grammar, derivations."""
from fastapi import APIRouter, HTTPException
from backend.models.automata import (
    RegexRequest,
    DerivationRequest,
    NFAResponse,
    DFAResponse,
    GrammarResponse,
    DerivationResponse,
)
from backend.services.engine import run_engine, EngineError

router = APIRouter(prefix="/api/automata", tags=["automata"])


@router.post("/regex-to-nfa", response_model=NFAResponse)
async def regex_to_nfa(req: RegexRequest):
    """Convert a regular expression to an NFA (Thompson construction)."""
    try:
        result = await run_engine("regex2nfa", req.regex)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/nfa-to-dfa", response_model=DFAResponse)
async def nfa_to_dfa(req: RegexRequest):
    """Convert regex → NFA → DFA (subset construction)."""
    try:
        result = await run_engine("nfa2dfa", req.regex)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/grammar", response_model=GrammarResponse)
async def grammar(req: RegexRequest):
    """Convert regex → NFA → DFA + generate regular grammar."""
    try:
        result = await run_engine("grammar", req.regex)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))


@router.post("/derivations", response_model=DerivationResponse)
async def derivations(req: DerivationRequest):
    """Generate leftmost and rightmost derivations for a string."""
    try:
        input_text = f"{req.regex}\n{req.test_string}"
        result = await run_engine("derivation", input_text)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))
