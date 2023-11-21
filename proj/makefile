all :  proj

proj: main.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Linux/MicroGlut.c VectorUtils4.h LittleOBJLoaderX.h
	g++ -Wall -o proj -DGL_GLEXT_PROTOTYPES main.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Linux/MicroGlut.c -I../common -I../common/Linux -lXt -lX11 -lm -lGL -g

clean :
	rm proj
