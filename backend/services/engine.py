"""
C++ Engine Bridge – executes the compiler_api binary and returns parsed JSON.
"""
import asyncio
import json
import os
import tempfile
import platform
from pathlib import Path
from typing import Any

# Resolve path to compiler_api binary relative to project root
_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent
_IS_WINDOWS = platform.system() == "Windows"
_BINARY_NAME = "compiler_api.exe" if _IS_WINDOWS else "compiler_api"
_BINARY_PATH = _PROJECT_ROOT / _BINARY_NAME
_TIMEOUT = 10  # seconds


class EngineError(Exception):
    """Raised when the C++ engine returns an error or crashes."""
    pass


async def _ensure_binary() -> Path:
    """Make sure the compiler_api binary exists. Compile if needed."""
    if _BINARY_PATH.exists():
        return _BINARY_PATH

    # Try to compile
    src_dir = _PROJECT_ROOT / "src"
    if not src_dir.exists():
        raise EngineError("C++ source directory not found at " + str(src_dir))

    sources = [
        "src/api_main.cpp",
        "src/compiler/Lexer.cpp", "src/compiler/Parser.cpp",
        "src/compiler/Semantic.cpp", "src/compiler/IR.cpp",
        "src/compiler/Optimizer.cpp", "src/compiler/CodeGen.cpp",
        "src/automata/Regex.cpp", "src/automata/NFA.cpp",
        "src/automata/DFA.cpp", "src/automata/Grammar.cpp",
        "src/tools/StringTools.cpp", "src/tools/ExpressionEval.cpp",
    ]

    cmd = [
        "g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-Isrc",
        "-o", str(_BINARY_PATH),
    ] + sources

    proc = await asyncio.create_subprocess_exec(
        *cmd,
        cwd=str(_PROJECT_ROOT),
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )
    stdout, stderr = await asyncio.wait_for(proc.communicate(), timeout=60)
    if proc.returncode != 0:
        raise EngineError(
            f"Failed to compile C++ engine:\n{stderr.decode(errors='replace')}"
        )
    return _BINARY_PATH


async def run_engine(command: str, input_text: str) -> dict[str, Any]:
    """
    Run the C++ compiler_api with the given command and input text.
    Returns parsed JSON output from the binary.
    """
    binary = await _ensure_binary()

    # Write input to a temp file
    tmp = tempfile.NamedTemporaryFile(
        mode="w", suffix=".txt", delete=False, encoding="utf-8"
    )
    try:
        tmp.write(input_text)
        tmp.close()

        proc = await asyncio.create_subprocess_exec(
            str(binary), command, tmp.name,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
            cwd=str(_PROJECT_ROOT),
        )

        try:
            stdout, stderr = await asyncio.wait_for(
                proc.communicate(), timeout=_TIMEOUT
            )
        except asyncio.TimeoutError:
            proc.kill()
            raise EngineError("C++ engine timed out after {_TIMEOUT}s")

        output = stdout.decode("utf-8", errors="replace").strip()

        if proc.returncode != 0:
            err_output = stderr.decode("utf-8", errors="replace").strip()
            # Try to parse error from stdout (engine outputs JSON errors)
            if output:
                try:
                    result = json.loads(output)
                    if "error" in result:
                        raise EngineError(result["error"])
                except json.JSONDecodeError:
                    pass
            raise EngineError(
                f"Engine exited with code {proc.returncode}: {err_output or output}"
            )

        if not output:
            raise EngineError("Engine produced no output")

        try:
            result = json.loads(output)
        except json.JSONDecodeError as e:
            raise EngineError(f"Failed to parse engine output as JSON: {e}")

        if "error" in result:
            raise EngineError(result["error"])

        return result

    finally:
        try:
            os.unlink(tmp.name)
        except OSError:
            pass
