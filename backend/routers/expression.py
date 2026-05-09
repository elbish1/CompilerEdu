"""Expression evaluation API endpoint."""
from fastapi import APIRouter, HTTPException
from backend.models.automata import ExpressionRequest, ExpressionResponse
from backend.services.engine import run_engine, EngineError

router = APIRouter(prefix="/api/expression", tags=["expression"])


@router.post("/evaluate", response_model=ExpressionResponse)
async def evaluate_expression(req: ExpressionRequest):
    """Evaluate a mathematical expression with precedence and parentheses."""
    try:
        result = await run_engine("evaluate", req.expression)
        return result
    except EngineError as e:
        raise HTTPException(status_code=422, detail=str(e))
