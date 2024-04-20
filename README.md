# win32-text-editor

A basic text editor written in C using only the Win32 API

# Compile and run

```shell
clang main.c -o main.exe -mwindows -municode -luser32 -lgdi32 -lcomdlg32 -lshell32
.\main.exe
```
