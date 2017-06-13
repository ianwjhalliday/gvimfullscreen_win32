@echo off
rem optimized
cl /W4 /WX /LD /Zi /Oy- /O2 /Ob2 /D "NDEBUG" user32.lib gdi32.lib gvimfullscreen.cpp

rem debug
rem cl /W4 /WX /LD /Zi /Oy- /Od /Ob0 /D "_DEBUG" user32.lib gdi32.lib gvimfullscreen.cpp
