@echo off

gcc -std=c99 -Wall -Wextra -shared -o bin/engine.dll src/engine/*.c src/engine/*.def -Llib -lmingw32 -lSDL2main -lSDL2 -Iinclude -Wl,--out-implib,lib/libEngine.dll.a -Wl,--enable-auto-image-base

echo Done DLL

gcc -std=c99 -Wall -Wextra -o bin/app.exe src/sandbox/*.c -Llib -lmingw32 -lSDL2main -lSDL2 -lEngine -Iinclude

echo Done Compilation