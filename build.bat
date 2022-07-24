
@echo off
mkdir bin
pushd src
cl -Zi ^
   /std:c++17 ^
   /I ..\..\libs\glfw\include ^
   /I ..\..\libs\imgui ^
   /I ..\..\libs\glew\include ^
   /I ..\libs\soloud\include ^
   /I ..\libs\include ^
   main.c ^
   program\program.c ^
   program\sound\sound.c ^
   program\mainMenu\mainMenu.c ^
   program\level\level.c ^
   program\enemy\enemy.c ^
   program\core\fonts\fonts.c ^
   program\core\graphics\graphics.c ^
   program\core\input\keyboard.c ^
   program\core\input\mouse.c ^
   program\core\shader\shader.c ^
   program\core\sprite\sprite.c ^
   program\core\stackAllocator\stack_allocator.c ^
   ..\..\libs\imgui\imgui.cpp^
   ..\..\libs\imgui\imgui_demo.cpp^
   ..\..\libs\imgui\imgui_draw.cpp^
   ..\..\libs\imgui\backends\imgui_impl_glfw.cpp^
   ..\..\libs\imgui\backends\imgui_impl_opengl3.cpp^
   ..\..\libs\imgui\imgui_tables.cpp^
   ..\..\libs\imgui\imgui_widgets.cpp^
   ..\..\libs\glfw\lib-vc2022\glfw3dll.lib ^
   ..\..\libs\glfw\lib-vc2022\glfw3_mt.lib ^
   ..\..\libs\glew\lib\Release\x64\glew32.lib ^
   opengl32.lib ^
   -o ../bin/main.exe
popd

REM xcopy ..\libs\glfw\lib-vc2022\glfw3.dll bin /Y
REM xcopy assets bin\assets /E /H /C /I /Y

pushd bin
main.exe
popd
