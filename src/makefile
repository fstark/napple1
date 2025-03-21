# napple1 makefile 

CC = g++
CFLAGS = -Wall -Werror
DEBUG_FLAGS = -g
LIBS = -lpthread

# List of source files
SRC = m6502.cpp memory.cpp pia6820.cpp screen.cpp msgbuf.cpp keyboard.cpp main.cpp disassemble.cpp commands.cpp rom512.cpp symbols.cpp

# Generate object files list from source files
OBJ = $(SRC:.cpp=.o)

# Generate dependency files list from source files
DEP = $(SRC:.cpp=.d)

all: ../napple1

debug: CFLAGS += $(DEBUG_FLAGS)
debug: ../napple1

../napple1: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Generic rule for compiling .cpp files to .o files
%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

# Include dependency files
-include $(DEP)

clean:
	-rm -f $(OBJ)
	-rm -f $(DEP)
	-rm -f ../napple1

