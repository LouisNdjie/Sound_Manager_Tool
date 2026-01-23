@echo off
setlocal enabledelayedexpansion

chcp 65001> nul

set LOCAL_DIR=%~dp0
set COMMAND_DIR=%LOCAL_DIR%Tools\command
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set ESC=%%b

if "%~1"=="" (
    echo !ESC![1;34m╔════════════════════════════════════╗!ESC![0m
    echo !ESC![1;34m║     ⚡COMPILER BUILD SYSTEM        ║!ESC![0m
    echo !ESC![1;34m  ══════════════════════════════════  !ESC![0m
    echo !ESC![1;34m  ║ 📖 Usage : compiler [command]  ║ !ESC![0m
    echo !ESC![1;34m  ║ 🛠️ Commandes disponibles:       ║ !ESC![0m
    echo !ESC![1;34m  ║ ➜ build 🔨         -Compiler   ║!ESC![0m
    echo !ESC![1;34m  ║ ➜ run 🚀           -exécuter   ║!ESC![0m
    echo !ESC![1;34m  ║ ➜ clean 🧹         -nettoyer   ║!ESC![0m
    echo !ESC![1;34m  ║ ➜ rebuild🔄        -recompiler ║!ESC![0m
    echo !ESC![1;34m  ══════════════════════════════════  !ESC![0m
    goto :eof
)

set ARGUMENT=%~1
shift

if "%ARGUMENT%"=="build" (
    call "%COMMAND_DIR%\build.bat" %*
) else if "%ARGUMENT%"=="run" (
    call "%COMMAND_DIR%\run.bat"
) else if "%ARGUMENT%"=="clean" (
    call "%COMMAND_DIR%\clean.bat"
) else if "%ARGUMENT%"=="rebuild" (
    call "%COMMAND_DIR%\rebuild.bat"
) else (
    echo Commande inconnue: %ARGUMENT%
    echo Utilisez les commandes disponibles.
)

endlocal