@echo off
SETLOCAL

set COMPILER_FLAGS=

set INCLUDE=-Ivendor/raylib-5.0/include
set LIB=-Lvendor/raylib-5.0/lib

set LINK=^
    -lraylib -lgdi32 -lwinmm

set SRC_DIR=src
set TARGET_DIR=target

set OUTPUT=curvemaker.exe
set COMPILE=^
    %SRC_DIR%/main.c

mkdir %TARGET_DIR%

@echo on

gcc %COMPILER_FLAGS% %COMPILE% %INCLUDE% %LIB% %LINK% -o ./%TARGET_DIR%/%OUTPUT%

@echo off

set OUTPUT_LOCAL=curvemaker-local.exe
copy %TARGET_DIR%\%OUTPUT% %OUTPUT_LOCAL%

:: gcc main.c -Ivendor/raylib/include -Lvendor/raylib/lib -lraylib -lgdi32 -lwinmm -o target/game.exe