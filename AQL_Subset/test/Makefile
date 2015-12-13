test: Tokenizer_test Lexer_test Parser_test

Tokenizer_test: ../Tokenizer.h ../Tokenizer.cpp Tokenizer_test.cpp
	g++ -std=c++11 -g Tokenizer_test.cpp ../Tokenizer.h ../Tokenizer.cpp -o Tokenizer_test
Lexer_test: ../Lexer.h ../Lexer.cpp ../Tokenizer.h ../Tokenizer.cpp Lexer_test.cpp
	g++ -std=c++11 -g Lexer_test.cpp ../Lexer.h ../Lexer.cpp ../Tokenizer.h ../Tokenizer.cpp -o Lexer_test
Parser_test: ../Lexer.h ../Lexer.cpp ../Tokenizer.h ../Tokenizer.cpp ../Parser.h ../Parser.cpp Parser_test.cpp
	g++ -std=c++11 -g Parser_test.cpp ../Lexer.h ../Lexer.cpp ../Tokenizer.h ../Tokenizer.cpp ../Parser.h ../Parser.cpp -o Parser_test
