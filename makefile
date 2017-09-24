SOURCES = driver-sample.cpp BList.h PRNG.cpp
FLAGS = -std=c++14 -Wall
LDIRS =
IDIRS =
LIBS =

clean:
	rm *-app *.o *.obj out

1:
	g++ -o out $(SOURCES) $(FLAGS)
	./out 1

8:
	g++ -o out $(SOURCES) $(FLAGS)
	./out 8 | more

22:
	g++ -o out $(SOURCES) $(FLAGS)
	./out 22 > output.txt

all:
	g++ -o out $(SOURCES) $(FLAGS)
	./out

debug:
	g++ -o out -g $(SOURCES) $(FLAGS)
	valgrind -q --leak-check=full --tool=memcheck ./out > output.txt 2>&1 
