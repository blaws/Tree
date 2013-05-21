all: tree

tree: tree.c gfx.c gfx.h
	gcc tree.c gfx.c -o tree -lm -lX11

clean:
	rm -f tree

