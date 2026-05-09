"""Health check endpoint."""
from fastapi import APIRouter
from pathlib import Path

router = APIRouter(tags=["health"])

_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent


@router.get("/api/health")
async def health_check():
    """Return health status and engine availability."""
    import platform
    binary_name = "compiler_api.exe" if platform.system() == "Windows" else "compiler_api"
    binary_exists = (_PROJECT_ROOT / binary_name).exists()
    return {
        "status": "healthy",
        "engine_available": binary_exists,
        "platform": platform.system(),
    }
