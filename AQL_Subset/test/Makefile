test: Tokenizer_test Lexer_test

Tokenizer_test: ../Tokenizer.h ../Tokenizer.cpp Tokenizer_test.cpp
	g++ -g Tokenizer_test.cpp ../Tokenizer.h ../Tokenizer.cpp -o Tokenizer_test
Lexer_test: ../Lexer.h ../Lexer.cpp ../Tokenizer.h ../Tokenizer.cpp Lexer_test.cpp
	g++ -g Lexer_test.cpp ../Lexer.h ../Lexer.cpp ../Tokenizer.h ../Tokenizer.cpp -o Lexer_test
