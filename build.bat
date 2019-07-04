@ECHO off

SET CFLAGS=-std=c99 -g -O2 -I ..\src\common -I ..\src\dicts

MKDIR build
PUSHD build

clang %CFLAGS%  -c  ..\src\dicts\common6.c

clang %CFLAGS% -c  ..\src\common\trie.c
clang %CFLAGS% -c  ..\src\common\words.c
clang %CFLAGS% -c  ..\src\common\game.c

clang %CFLAGS% -c  ..\src\win\window_action.c
clang %CFLAGS% -c  ..\src\win\window_input.c
clang %CFLAGS% -c  ..\src\win\mm.c

clang -g -l user32.lib -l gdi32.lib -o mm.exe trie.o words.o game.o mm.o window_action.o window_input.o common6.o
::clang -g -l user32.lib -l gdi32.lib -o termword.exe trie.o words.o game.o util.o main.o
POPD

