TARGET := taoc
SRC := hash symtable lex.yy y.tab
OBJS := $(addprefix obj/,$(addsuffix .o,$(SRC)))
CCFLAGS := -O2 -Wall -lm
LDFLAGS := -Wall
YACCFLAGS := -Wother -Wconflicts-sr

all: bin/$(TARGET)

bin/$(TARGET): $(OBJS)
	gcc -o bin/$(TARGET) $(OBJS) $(LDFLAGS)

obj/%.o: src/%.c src/%.h src/defs.h
	gcc -c $(CCFLAGS) src/$*.c -o obj/$*.o

obj/%.yy.o: src/%.yy.c src/defs.h
	gcc -c $(CCFLAGS) src/$*.yy.c -o obj/$*.yy.o

obj/%.tab.o: src/%.tab.c src/%.tab.h src/defs.h
	gcc -c $(CCFLAGS) src/$*.tab.c -o obj/$*.tab.o

src/y.tab.h: src/parse.y
	yacc $(YACCFLAGS) src/parse.y --defines=src/y.tab.h -o src/y.tab.c

src/y.tab.c: src/parse.y
	yacc $(YACCFLAGS) src/parse.y --defines=src/y.tab.h -o src/y.tab.c

src/%.yy.c: src/%.l src/y.tab.h
	flex -o src/$*.yy.c src/$*.l

clean:
	rm obj/* bin/* src/y.tab.* src/*.yy.c