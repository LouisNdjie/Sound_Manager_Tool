@echo off
setlocal enabledelayedexpansion

chcp 65001> nul

for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set ESC=%%b

:: Verifier si l'argument principal est "build"
if /i "%1" neq "build" (
    echo Usage: %0 build [compilateur] [debug^|release]
    exit /b 1
)

::set clang++ comme compilateur par défaut , ou choisir celui passé en second argument
set "COMPILATEUR=%~2"
if "%COMPILATEUR%"=="" set "COMPILATEUR=clang++"

::set release comme mode de compilation par défaut, ou choisir debug
set "MODE=%~3"
if /i "!MODE!"=="debug" (
    set "FLAGS=-g -O0 -std=c++20"
) else (
    set "FLAGS=-O2 -std=c++20"
    set "MODE=release"
)

::rappeler le compilateur utilisé et le mode actif
echo !ESC![1;34m Build with %COMPILATEUR% in !MODE! mode .... !ESC![0m

::chemins pour la SDL3 
set "INCLUDE=C:\msys64\ucrt64\include\SDL3"
set "LIB=C:\msys64\ucrt64\lib"

::dossiers du projet 
set "SOURCE_DIR=.\src"
set "OBJECT_DIR=.\build\object"
set "BINARY_DIR=.\build\binary"
set "UTILS_DIR=.\src\Utils"

::nom de l'éxécutable
set "OUTPUT_FILE=SoundManagerTool.exe"

::vérifie si le dossier de source existe
if not exist "%SOURCE_DIR%" (
    echo !ESC![1;4;31m ERROR: !ESC![0m Le dossier %SOURCE_DIR% est inexistant.
    exit /b 1
)

::créer les dossiers s'il n'existent pas
if not exist "%OBJECT_DIR%" mkdir "%OBJECT_DIR%"
if not exist "%BINARY_DIR%" mkdir "%BINARY_DIR%"

::vérifier s'il y a des fichiers .cpp
set "CPP_FILE_COUNT=0"
for /r "%SOURCE_DIR%" %%f in (*.cpp) do (
    set /a CPP_FILE_COUNT+=1
)
if !CPP_FILE_COUNT! equ 0 (
    echo !ESC![1;4;31m ERROR: !ESC![0m Aucun fichier .cpp trouvé dans %SOURCE_DIR% ou dans les sous-dossiers.
    exit /b 1
)

::compiler les.cpp qui ont été modifiés uniquement
set "OBJECT_FILES="
for /r "%SOURCE_DIR%" %%f in (*.cpp) do (
    set "CPP_FULL=%%~ff"
    set "base=%%~nf"
    set "o=%OBJECT_DIR%\!base!.o"
    set "compile=no"

    call :FileModTime "!CPP_FULL!" cpp_date
    call :FileModTime "!o!" o_date

    ::vérifie si le fichier objet existe et compiler sinon
    if "!o_date!"=="" (
        set "compile=yes"
    ) else (
        ::vérifie si le fichier.cpp est récent et compiler si oui
        if "!cpp_date!" GTR "!o_date!" (
            set "compile=yes"
        ) else (
            ::vérifier si les fichiers.h sont récents
            call :CheckHeadersSimple "!CPP_FULL!" "!o!" compile
        )
    )

    if "!compile!"=="yes" (
        echo ✓ !ESC![1;32m Compile : !ESC![0m !base!.cpp
        "%COMPILATEUR%" -c "!CPP_FULL!" -I "%SOURCE_DIR%" -I "%INCLUDE%" !FLAGS! -o "!o!"
        if errorlevel 1 (
            echo ✗ !ESC![1;4;31m Error compile : !ESC![0m !base!.cpp
            exit /b 1
        )
    ) else (
        echo !ESC![1;34m Ignore : !ESC![0m !base!.cpp
    )
    set "OBJECT_FILES=!OBJECT_FILES! "!o!""
)

::vérifier si les fichiers objets sont bien présents
if "!OBJECT_FILES!"=="" (
    echo ✗ !ESC![1;4;31m Error : !ESC![0m pas de fichier objets.
    exit /b 1
)

::linker les fichiers objets avec l'exécutable
echo !ESC![1;34m Linking ... !ESC![0m
"%COMPILATEUR%" !OBJECT_FILES! -L "%LIB%" -lSDL3 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lshell32 !FLAGS! -o "%BINARY_DIR%\%OUTPUT_FILE%"
if errorlevel 1 (
    echo  ✗ !ESC![1;4;31m Error : !ESC![0m échec de linkage
    exit /b 1
)

:: Copier les ressources
if exist "%UTILS_DIR%" (
    echo !ESC![1;34m Copying : !ESC![0m les utilitaires vers -> %BINARY_DIR%...
    xcopy /E /I /Y "%UTILS_DIR%\*" "%BINARY_DIR%\" >nul || (
        echo ✗ !ESC![1;4;31m Error : !ESC![0m échec de copie des utilitaires
        exit /b 1
    )
) else (
    echo ✗ !ESC![1;4;31m Warning: : !ESC![0m le dossier %UTILS_DIR% n'existe pas, aucune ressource copiée.
)

echo !ESC![1;4;34m Build completed: !ESC![0m %BINARY_DIR%\%OUT%

goto :EOF

:CheckHeadersSimple CppFile ObjFile ResultVar
:: Version simplifiee qui verifie tous les fichiers .h dans le dossier Sources

setlocal
set "needs_recompile=no"

call :FileModTime "%~2" obj_date

:: Verifier tous les fichiers .h dans Sources
for /r "%SOURCE_DIR%" %%h in (*.h) do (
    call :FileModTime "%%h" header_date
    if "!header_date!" GTR "!obj_date!" (
        set "needs_recompile=yes"
        goto :HeaderCheckDone
    )
)

:HeaderCheckDone
endlocal & set "%~3=%needs_recompile%"
exit /b

:FileModTime  File  DateVar
:: Utilise dir pour obtenir la date de modification dans un format comparable
setlocal
set "ts="
if not exist "%~1" (
    set "ts="
) else (
    for /f "tokens=1-2" %%A in ('dir "%~1" /t:w 2^>nul ^| findstr /r /c:"%~nx1$"') do (
        set "date=%%A"
        set "time=%%B"
    )
    if defined date (
        :: Reformater la date pour comparaison lexicographique (YYYYMMDDHHMMSS)
        for /f "tokens=1-3 delims=/" %%X in ("!date!") do (
            set "day=%%X"
            set "month=%%Y"
            set "year=%%Z"
        )
        for /f "tokens=1-2 delims=:." %%X in ("!time!") do (
            set "hour=%%X"
            set "minute=%%Y"
        )
        :: Completer avec des zeros pour un format fixe
        set "day=00!day!" & set "day=!day:~-2!"
        set "month=00!month!" & set "month=!month:~-2!"
        set "hour=00!hour!" & set "hour=!hour:~-2!"
        set "minute=00!minute!" & set "minute=!minute:~-2!"
        set "ts=!year!!month!!day!!hour!!minute!"
    )
)
endlocal & set "%~2=%ts%"
exit /b