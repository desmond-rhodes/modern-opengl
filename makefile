opengl.exe: main.cc | .setup
	x86_64-w64-mingw32-g++-posix -std=c++17 -Iinclude $^ gl3w/src/gl3w.c -Llib -lglfw3 -lgdi32 -static-libstdc++ -static -lpthread -o $@

.setup: | glfw gl3w
	cd gl3w && python3 gl3w_gen.py
	cmake -S glfw -B glfw/build -D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake
	make -C glfw/build glfw --no-print-directory
	touch .setup

.PHONY: clean
clean:
	rm -f opengl.exe

.PHONY: reset-setup
reset-setup:
	git submodule foreach --recursive git clean -ffdx
	rm -f .setup
