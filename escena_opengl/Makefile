CFLAGS=-g -Wall
LDFLAGS!=pkgconf --libs --cflags opengl glfw3 glew

.PHONY: all clean run build

scene_opengl: scene_opengl.cpp
	g++ scene_opengl.cpp $(LDFLAGS) $(CFLAGS) -o scene_opengl

build: scene_opengl

clean:
	rm scene_opengl

run: build
	./scene_opengl

all: build
