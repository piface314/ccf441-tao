TARGET := taoc
SRC := hash symtable lex.yy y.tab
OBJS := $(addprefix obj/,$(addsuffix .o,$(SRC)))
CCFLAGS := -O2 -Wall -lm
LDFLAGS := -Wall


all: bin/$(TARGET)

bin/$(TARGET): $(OBJS)
	gcc -o bin/$(TARGET) $(OBJS) $(LDFLAGS)

obj/%.o: src/%.c src/%.h
	gcc -c $(CCFLAGS) src/$*.c -o obj/$*.o

obj/%.yy.o: src/%.yy.c
	gcc -c $(CCFLAGS) src/$*.yy.c -o obj/$*.yy.o

obj/%.tab.o: src/%.tab.c src/%.tab.h
	gcc -c $(CCFLAGS) src/$*.tab.c -o obj/$*.tab.o

src/y.tab.h: src/parse.y
	yacc src/parse.y --defines=src/y.tab.h -o src/y.tab.c

src/y.tab.c: src/parse.y
	yacc src/parse.y --defines=src/y.tab.h -o src/y.tab.c

src/lex.yy.c: src/lex.l src/y.tab.h
	flex -o src/lex.yy.c src/lex.l