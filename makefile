OUT := opengl
OBJS := main.o shader-loader.o
OBJS += lib/gl3w.o

CC := x86_64-w64-mingw32-gcc-posix
CFLAGS := -std=c17 -Iinclude

CXX := x86_64-w64-mingw32-g++-posix
CXXFLAGS := -std=c++17 -Iinclude

LDFLAGS := -Llib
LDLIBS := -lglfw3 -lgdi32
LDLIBS += -static-libgcc -static-libstdc++ -static -lwinpthread

TMP := .$(OUT)

$(OUT): $(OBJS)
	touch $(TMP).cc
	make $(TMP)
	mv $(TMP) $@
	rm $(TMP)*

$(TMP): $(TMP).cc $(OBJS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(OUT)
	rm -f $(TMP)*

.PHONY: clean-all
clean-all: clean
	rm -f lib/*.o
