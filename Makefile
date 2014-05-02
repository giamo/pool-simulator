OBJS = biliardo.o
FLAGS = -lGL -lGLU -lglut

biliardo: $(OBJS)
	g++ $(FLAGS) $(OBJS) -o biliardo

biliardo.o: biliardo.cpp palla.h vector.h texture.h
	g++ -c biliardo.cpp

clean:
	rm -f ~.* *.o biliardo
