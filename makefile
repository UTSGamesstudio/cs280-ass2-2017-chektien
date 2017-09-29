SOURCES = ObjectAllocator.cpp driver-sample.cpp PRNG.cpp
FLAGS = -std=c++14 -Wall

compile:
	g++ -o out $(SOURCES) $(FLAGS)

clean:
	rm *-app *.o *.obj out

1:
	g++ -o out $(SOURCES) $(FLAGS)
	./out 21 

8:
	g++ -o out $(SOURCES) $(FLAGS)
	./out 8 | more

22:
	g++ -o out $(SOURCES) $(FLAGS)
	./out 22 > output.txt

run:
	g++ -o out $(SOURCES) $(FLAGS)
	./out > output.txt

debug:
	g++ -o out -g $(SOURCES) $(FLAGS)
	valgrind -q --leak-check=full --tool=memcheck ./out > output.txt 2>&1 
