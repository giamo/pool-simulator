OBJS = pool.o
FLAGS = -lGL -lGLU -lglut

pool: $(OBJS)
	g++ $(OBJS) -o pool $(FLAGS)

pool.o: pool.cpp ball.h vector.h texture.h
	g++ -c pool.cpp

clean:
	rm -f ~.* *.o pool
