@echo off
setlocal

echo Building...
g++ -std=c++17 -O2 -Wall -Wextra -Isrc -o compiler_app ^
  src\main.cpp ^
  src\compiler\Lexer.cpp src\compiler\Parser.cpp src\compiler\Semantic.cpp ^
  src\compiler\IR.cpp src\compiler\Optimizer.cpp src\compiler\CodeGen.cpp ^
  src\automata\Regex.cpp src\automata\NFA.cpp src\automata\DFA.cpp ^
  src\automata\Grammar.cpp ^
  src\tools\StringTools.cpp src\tools\ExpressionEval.cpp

if errorlevel 1 (
  echo Build failed.
  exit /b 1
)

g++ -std=c++17 -O2 -Wall -Wextra -Isrc -o compiler_api ^
  src\api_main.cpp ^
  src\compiler\Lexer.cpp src\compiler\Parser.cpp src\compiler\Semantic.cpp ^
  src\compiler\IR.cpp src\compiler\Optimizer.cpp src\compiler\CodeGen.cpp ^
  src\automata\Regex.cpp src\automata\NFA.cpp src\automata\DFA.cpp ^
  src\automata\Grammar.cpp ^
  src\tools\StringTools.cpp src\tools\ExpressionEval.cpp

if errorlevel 1 (
  echo Build failed.
  exit /b 1
)

echo Build OK. Run:
echo   compiler_app.exe   (interactive)
echo   compiler_api.exe   (JSON API mode)
endlocal

