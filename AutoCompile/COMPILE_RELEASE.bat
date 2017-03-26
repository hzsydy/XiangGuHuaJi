@echo off
dir /s/b content\*.cpp > dir.txt

for /f "delims=," %%i in (dir.txt) do (
  echo ------------------------------------------------------------------------------------------------------------------------------------
  echo %%i
  cl @RESP_RELEASE %%i content\user_toolbox.cpp
)

copy *.dll compiled_release\

del *.ilk
del *.pdb
del *.lib
del *.exp
del *.obj
del *.dll
