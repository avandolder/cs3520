CFLAGS = -O2
LIBS = -lglut -lGL -lGLU
CC=g++ $(CFLAGS)

draw_line: draw_line.o
	$(CC) -o draw_line draw_line.o $(LIBS)

$*.o: $*.cpp
