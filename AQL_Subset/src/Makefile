test: Tokenizer_test Lexer_test Parser_test

Tokenizer_test: ./Tokenizer.h ./Tokenizer.cpp ./test/Tokenizer_test.cpp
	g++ -std=c++11 -g ./test/Tokenizer_test.cpp ./Tokenizer.h ./Tokenizer.cpp -o Tokenizer_test
Lexer_test: ./Lexer.h ./Lexer.cpp ./Tokenizer.h ./Tokenizer.cpp ./test/Lexer_test.cpp
	g++ -std=c++11 -g ./test/Lexer_test.cpp ./Lexer.h ./Lexer.cpp ./Tokenizer.h ./Tokenizer.cpp -o Lexer_test
Parser_test: ./Lexer.h ./Lexer.cpp ./Tokenizer.h ./Tokenizer.cpp ./Parser.h ./Parser.cpp ./test/Parser_test.cpp
	g++ -std=c++11 -g ./test/Parser_test.cpp ./Lexer.h ./Lexer.cpp ./Tokenizer.h ./Tokenizer.cpp ./Parser.h ./Parser.cpp -o Parser_test
