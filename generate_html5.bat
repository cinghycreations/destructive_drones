cmake ^
	-S . ^
	-B temp/build/html5 ^
	-G "Ninja" ^
	-D CMAKE_MAKE_PROGRAM="C:\Projects\destructive_drones\ninja.exe" ^
	-D CMAKE_TOOLCHAIN_FILE="C:\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" ^
	-D CMAKE_BUILD_TYPE=Release ^
	-D PLATFORM=Web
pause
