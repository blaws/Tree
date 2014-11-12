GCC_OPTIONS=-Wall -pedantic -Iinclude -g -Wno-deprecated
GL_OPTIONS=-framework OpenGL -framework GLUT 
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: tree

InitShader.o: InitShader.cpp
	g++ -c InitShader.cpp $(GCC_OPTIONS)

tree: InitShader.o tree.o
	g++ $^ $(OPTIONS) -o $@  

tree.o: tree.cpp
	g++ -c tree.cpp $(GCC_OPTIONS)

clean:
	rm -rf *.o *~ *.dSYM tree
