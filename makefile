LDFLAGS=-Iinclude/\
		-Ilib/gmath/include/
SRC=$(wildcard src/*.cpp)\
	$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

bin: $(OBJ)
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $< -o $@ -O2 -fpermissive $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ)
