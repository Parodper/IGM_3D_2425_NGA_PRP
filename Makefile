.PHONY: all clean

all:
	$(MAKE) -C escena_opengl
	$(MAKE) -C escena_osg

clean:
	$(MAKE) -C escena_opengl clean
	$(MAKE) -C escena_osg clean
