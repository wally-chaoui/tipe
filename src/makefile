CFLAGS = -Wall -lm 
DEBUG = -fsanitize=address -g

all: main

main: main.o vector3.o sphere.o light.o triangle.o edge.o delaunay2.o colour.o half.o delaunay3.o
	gcc -o main main.o vector3.o sphere.o light.o triangle.o edge.o delaunay2.o colour.o half.o delaunay3.o $(CFLAGS) `sdl2-config --cflags --libs`

main.o: main.c
	gcc -c main.c $(CFLAGS)

vector3.o: vector3.c
	gcc -c vector3.c $(CFLAGS)

sphere.o: sphere.c
	gcc -c sphere.c $(CFLAGS)

light.o: light.c
	gcc -c light.c $(CFLAGS)

triangle.o: triangle.c
	gcc -c triangle.c $(CFLAGS)

edge.o: edge.c
	gcc -c edge.c $(CFLAGS)

delaunay2.o: delaunay2.c
	gcc -c delaunay2.c $(CFLAGS)

colour.o: colour.c
	gcc -c colour.c $(CFLAGS)

half.o: half.c
	gcc -c half.c $(CFLAGS)

delaunay3.o: delaunay3.c
	gcc -c delaunay3.c $(CFLAGS)

run:
	./main

clean:
	rm -f main *.o

rerun: clean main run