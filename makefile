TARGET="tao-lexer"

all: src/lex.l
	flex -o src/lex.yy.c src/lex.l 
	gcc src/lex.yy.c -o $(TARGET)

run:
	./$(TARGET) < $(INPUT)

	