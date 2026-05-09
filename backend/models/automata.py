"""Pydantic models for automata-related API requests and responses."""
from pydantic import BaseModel, Field


class RegexRequest(BaseModel):
    """Request body for automata endpoints."""
    regex: str = Field(
        ...,
        min_length=1,
        max_length=1000,
        description="Regular expression (e.g., (a|b)*abb)",
        examples=["(a|b)*abb"],
    )


class DerivationRequest(BaseModel):
    """Request body for derivation endpoint."""
    regex: str = Field(
        ...,
        min_length=1,
        max_length=1000,
        description="Regular expression",
        examples=["(a|b)*abb"],
    )
    test_string: str = Field(
        ...,
        max_length=1000,
        description="String to derive",
        examples=["aabb"],
    )


class ExpressionRequest(BaseModel):
    """Request body for expression evaluation."""
    expression: str = Field(
        ...,
        min_length=1,
        max_length=5000,
        description="Mathematical expression to evaluate",
        examples=["2 + 3 * (10 - 4)"],
    )


class TransitionModel(BaseModel):
    """A single NFA/DFA transition."""
    from_state: int = Field(alias="from")
    to: int
    symbol: str

    class Config:
        populate_by_name = True


class NFAModel(BaseModel):
    start: int
    stateCount: int
    accept: list[int]
    transitions: list[TransitionModel]


class DFAModel(BaseModel):
    start: int
    stateCount: int
    accept: list[int]
    alphabet: list[str]
    transitions: list[TransitionModel]
    subsets: dict[str, list[int]]


class ProductionModel(BaseModel):
    lhs: str
    rhs: str


class GrammarModel(BaseModel):
    startSymbol: str
    productions: list[ProductionModel]


class NFAResponse(BaseModel):
    nfa: NFAModel


class DFAResponse(BaseModel):
    nfa: NFAModel
    dfa: DFAModel


class GrammarResponse(BaseModel):
    nfa: NFAModel
    dfa: DFAModel
    grammar: GrammarModel


class DerivationResponse(BaseModel):
    nfa: NFAModel
    dfa: DFAModel
    grammar: GrammarModel
    leftmostDerivation: list[str]
    rightmostDerivation: list[str]


class ExpressionResponse(BaseModel):
    ok: bool
    value: int | None = None
    error: str | None = None
