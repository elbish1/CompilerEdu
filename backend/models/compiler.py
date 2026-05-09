"""Pydantic models for compiler-related API requests and responses."""
from pydantic import BaseModel, Field


class SourceCodeRequest(BaseModel):
    """Request body for compiler endpoints."""
    source_code: str = Field(
        ...,
        min_length=1,
        max_length=50000,
        description="Source code to compile (toy language)",
        examples=["int a = 2;\nint b = 3;\nprint(a + b);"],
    )


class TokenModel(BaseModel):
    type: str
    lexeme: str
    line: int
    col: int


class SymbolModel(BaseModel):
    name: str
    type: str
    declared: bool


class IRInstrModel(BaseModel):
    op: str
    res: str
    arg1: str
    arg2: str
    hasSideEffect: bool


class LexicalResponse(BaseModel):
    tokens: list[TokenModel]
    symbolTable: list[SymbolModel]
    literalTable: list[int]


class SyntaxResponse(BaseModel):
    tokens: list[TokenModel]
    symbolTable: list[SymbolModel]
    literalTable: list[int]
    parseTree: dict
    errors: list[str]


class SemanticResponse(BaseModel):
    tokens: list[TokenModel]
    symbolTable: list[SymbolModel]
    literalTable: list[int]
    parseTree: dict
    parseErrors: list[str]
    semanticErrors: list[str]


class IntermediateResponse(BaseModel):
    tokens: list[TokenModel]
    symbolTable: list[SymbolModel]
    literalTable: list[int]
    parseTree: dict
    parseErrors: list[str]
    semanticErrors: list[str]
    ir: list[IRInstrModel]


class OptimizeResponse(BaseModel):
    tokens: list[TokenModel]
    symbolTable: list[SymbolModel]
    literalTable: list[int]
    parseTree: dict
    parseErrors: list[str]
    semanticErrors: list[str]
    ir: list[IRInstrModel]
    optimizedIr: list[IRInstrModel]


class PipelineResponse(BaseModel):
    tokens: list[TokenModel]
    symbolTable: list[SymbolModel]
    literalTable: list[int]
    parseTree: dict
    parseErrors: list[str]
    semanticErrors: list[str]
    ir: list[IRInstrModel]
    optimizedIr: list[IRInstrModel]
    targetCode: list[str]
