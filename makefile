TARGET := taoc
SRC := lexer parser ast symtable main
OBJS := $(addprefix obj/,$(addsuffix .o,$(SRC)))
YACCFLAGS := -Wother -Wconflicts-sr -Wconflicts-rr -v
LLVMCONFIG := llvm-config-12
CCFLAGS := -O2 -Wall -g -std=c++11 `$(LLVMCONFIG) --cppflags`
LDFLAGS := -Wall `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS := `$(LLVMCONFIG) --libs`

all: bin/$(TARGET)

bin/$(TARGET): $(OBJS)
	g++ -o bin/$(TARGET) $(OBJS) $(LIBS) $(LDFLAGS)

obj/%.o: src/%.cpp
	g++ -c $(CCFLAGS) src/$*.cpp -o obj/$*.o

src/parser.hpp: src/parser.cpp

src/parser.cpp: src/parse.y
	bison $(YACCFLAGS) src/parse.y --defines=src/parser.hpp -o src/parser.cpp

src/lexer.cpp: src/lex.l src/parser.hpp
	flex -o src/lexer.cpp src/lex.l

clean:
	rm -vf obj/* bin/* src/parser.cpp src/parser.hpp src/lexer.cpp
