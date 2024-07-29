OS = $(shell uname)
CC = gcc
CFLAGS = -lstdc++ -lgtest -std=c++17 -Wall -Werror -Wextra
LINUX = -lsubunit -lrt -lpthread -lm -g

TESTS = unit_tests
BUILDTESTS = tests/unit_tests.cc

all: uninstall clean install

install: clean
ifeq ($(OS), Darwin)
	@mkdir build
	@cd Maze && qmake6 && make && make clean && rm Makefile && mv Maze.app ../build/ && cd ../
else
	@mkdir build
	@cd Maze && qmake6 && make && make clean && rm Makefile && mv Maze ../build/ && cd ../
endif

uninstall:
	@rm -rf build*

clean:
	@/bin/rm -rf **/*.o *.a build *.gcno *gcda report *.info main *.dSYM *.out $(TESTS) Maze/.qmake.stash Maze/Maze.pro.user Maze/.qtc_clangd build-Maze-Desktop_x86_darwin_generic_mach_o_64bit-Debug ../Archive_Maze.tar.gz Archive_Maze RESULT_VALGRIND.txt

dvi:
	@open ./dvi.html
	
dist: 
ifeq ($(OS), Darwin)
	@rm -rf Archive_Maze/
	@mkdir Archive_Maze/
	@mkdir Archive_Maze/src
	@mv  ./build/Maze.app  Archive_Maze/src/
	@tar cvzf Archive_Maze.tgz Archive_Maze/
	@rm -rf Archive_Maze/
else
	@rm -rf Archive_Maze/
	@mkdir Archive_Maze/
	@mkdir Archive_Maze/src
	@mv  ./build/Maze Archive_Maze/src/
	@tar cvzf Archive_Maze.tgz Archive_Maze/
	@rm -rf Archive_Maze/
endif

tests: build_tests
	@./$(TESTS)

build_tests:
ifeq ($(OS), Darwin)
	@$(CC) $< tests/unit_tests.cc $(CFLAGS) -o $(TESTS)
else
	@$(CC) $< tests/unit_tests.cc $(CFLAGS) $(LINUX) -o $(TESTS)
endif

leaks: tests 
ifeq ($(OS), Darwin)
	@CK_FORK=no leaks --atExit -- ./$(TESTS)
else
	@CK_FORK=no valgrind --vgdb=no --leak-check=full --show-leak-kinds=all --tool=memcheck --track-origins=yes --verbose --log-file=RESULT_VALGRIND.txt ./$(TESTS)
endif

style:
	@cp ../materials/linters/.clang-format .
	@clang-format -style=Google -n **/*.cc **/*.h
	@rm -rf .clang-format

