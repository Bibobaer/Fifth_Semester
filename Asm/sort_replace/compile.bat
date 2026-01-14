@echo off
echo Compiling ASM file...
ml /c func.asm

echo Linking...
link func.obj msvcrt.lib legacy_stdio_definitions.lib /out:main.exe
del func.obj

echo Running program...
main.exe str "a " " World" "Hello World"