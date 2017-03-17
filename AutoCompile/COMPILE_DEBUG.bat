@echo off
dir /s/b content\*.cpp > dir.txt

for /f "delims=," %%i in (dir.txt) do (
  echo %%i
  cl @RESP_DEBUG %%i content\user_toolbox.cpp
)

copy *.dll compiled_debug\

del *.ilk
del *.pdb
del *.lib
del *.exp
del *.obj
del *.dll
