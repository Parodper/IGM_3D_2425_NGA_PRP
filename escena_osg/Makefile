CFLAGS=-g -Wall
LDFLAGS_OSG!=pkgconf --libs --cflags openscenegraph-osg openscenegraph-osgViewer openscenegraph-osgAnimation

.PHONY: all clean run build

scene_osg: scene_osg.cpp
	g++ scene_osg.cpp $(LDFLAGS) $(CFLAGS) -o scene_osg

build: scene_osg

clean:
	rm scene_osg

run: build
	./scene_osg

all: build
