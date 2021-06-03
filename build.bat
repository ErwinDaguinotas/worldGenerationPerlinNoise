@echo off

:menu 
echo.
cd
echo [1] Compile debug
echo [2] Run debug
echo [3] Compile and run debug
echo [4] Compile release
echo [5] Run release
echo [6] Compile and run release
echo [7] Exit
echo | set /p=">>> "

set /p user=

if %user% == 1 goto compileDebug
if %user% == 2 goto runDebug
if %user% == 3 goto compileRunDebug
if %user% == 4 goto compileRelease
if %user% == 5 exit runRelease
if %user% == 6 goto compileRunRelease
if %user% == 7 exit

:compileDebug
echo Compiling for debug...
g++ -c src/*.cpp -std=c++14 -Werror -Wfatal-errors -I C:\Developer\SFML-2.5.1\include -I .\include
g++ *.o -o bin/debug/Game.exe -L C:\Developer\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
echo compiled successfully
goto menu

:compileRunDebug
echo Compiling and running for debug...
g++ -c src/*.cpp -std=c++14 -Werror -Wfatal-errors -I C:\Developer\SFML-2.5.1\include -I .\include
g++ *.o -o bin/debug/Game.exe -L C:\Developer\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
echo compiled successfully
"bin/debug/Game.exe"
goto menu

:compileRelease
echo Compiling for release...
g++ -c src/*.cpp -std=c++14 -Werror -Wfatal-errors -I C:\Developer\SFML-2.5.1\includ -I .\includee -O3 -s -fexpensive-optimizations
g++ *.o -o bin/release/Game.exe -L C:\Developer\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
echo compiled successfully
goto menu

:compileRunRelease
echo Compiling and runnig for release...
g++ -c src/*.cpp -std=c++14 -Werror -Wfatal-errors -I C:\Developer\SFML-2.5.1\include -I .\include -O3 -s -fexpensive-optimizations
g++ *.o -o bin/release/Game.exe -L C:\Developer\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
echo compiled successfully
"bin/release/Game.exe"
goto menu

:runDebug
echo running debug
"bin/debug/Game.exe"
goto menu

:runRelease
echo running release
"bin/release/Game.exe"
goto menu