OUT := opengl
OBJS := main.o shader-loader.o cleanup.o
DEPS := $(OBJS:%.o=%.d)

CC := x86_64-w64-mingw32-gcc-posix
CFLAGS := -std=c17

CXX := x86_64-w64-mingw32-g++-posix
CXXFLAGS := -std=c++17 -Wall -Wextra

CPPFLAGS := -Iinclude

LDFLAGS := -Llib
LDLIBS := -lglfw3 -lgl3w -lgdi32 -static-libgcc -static-libstdc++ -static -lwinpthread

DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

AR := x86_64-w64-mingw32-ar
RANLIB := x86_64-w64-mingw32-ranlib

$(OUT): $(OBJS)
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o ._$@ && mv ._$@ $@

$(OBJS) : | .setup

%.o: %.cc
	$(COMPILE.cc) $(DEPFLAGS) $(OUTPUT_OPTION) $<

.setup: | glfw gl3w
	cd gl3w && python3 gl3w_gen.py
	make gl3w/src/gl3w.o --no-print-directory
	$(AR) rc gl3w/src/libgl3w.a gl3w/src/gl3w.o
	$(RANLIB) gl3w/src/libgl3w.a
	cmake -S glfw -B glfw/build -D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake
	make -C glfw/build glfw --no-print-directory
	touch .setup

.PHONY: clean
clean:
	rm -f $(DEPS) $(OBJS) $(OUT)

.PHONY: reset-setup
reset-setup:
	git submodule foreach --recursive git clean -ffdx
	rm -f .setup

include $(wildcard $(DEPS))
