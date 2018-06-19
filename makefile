CC=emcc
SOURCES:=main.cpp $(wildcard src/*.cpp) $(wildcard src/Loader/*.cpp)
LDFLAGS=-O2 --llvm-opts 2
OUTPUT=out/mega_racer.html

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES) 
	$(CC) -Isrc -I/usr/local/include/ -Isrc/Loader $(SOURCES) --bind -s FULL_ES3=1 -std=c++11 $(LDFLAGS) -o $(OUTPUT) -s WASM=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -lGLU -lGL

clean:
	rm $(OUTPUT)
	
