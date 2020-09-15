# Compiler options.
WARNING    = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
COMPILECPP = g++ -std=c++17 -g -O0 ${WARNING}

# Files.
EXECBIN = nSnake.exe
MAIN_PRG = main.cpp
GAME_SRCS = kbd.cpp game.cpp
SOURCES = ${MAIN_PRG} ${GAME_SRCS}
OBJECTS = ${SOURCES:.cpp=.o}

all: ${EXECBIN}

${EXECBIN}: ${OBJECTS}
	${COMPILECPP} ${OBJECTS} -o ${EXECBIN}

%.o: %.cpp
	${COMPILECPP} -c $< -o $@

clean:
	-rm ${OBJECTS}

spotless: clean
	-rm ${EXECBIN}
