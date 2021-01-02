#build MazeGrammar.cpp
%D%/MazeGrammarVisitor.cpp %D%/MazeGrammarLexer.cpp %D%/MazeGrammarParser.cpp %D%/MazeGrammarBaseVisitor.cpp: $(top_srcdir)/src/grammar/MazeGrammar.g4 Makefile
	cd src/grammar && \
	cp ../../$(top_srcdir)/src/grammar/MazeGrammar.g4 . && \
	antlr4 -Dlanguage=Cpp -no-listener -visitor MazeGrammar.g4 && \
	rm MazeGrammar.g4 && \
	cd -

BUILT_SOURCES+=\
	%D%/MazeGrammarLexer.cpp \
	%D%/MazeGrammarParser.cpp \
	%D%/MazeGrammarBaseVisitor.cpp \
	%D%/MazeGrammarVisitor.cpp

__top_builddir__lib_libmaze_a_SOURCES+= \
	%D%/MazeGrammarLexer.cpp \
	%D%/MazeGrammarParser.cpp \
	%D%/MazeGrammarBaseVisitor.cpp \
	%D%/MazeGrammarVisitor.cpp

