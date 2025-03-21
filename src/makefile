# napple1 makefile 

CC = g++
CFLAGS = -Wall # -Werror
DEBUG_FLAGS = -g
LIBS = -lpthread

# List of source files
SRC = m6502.c memory.c pia6820.c screen.c msgbuf.c keyboard.c main.c disassemble.c commands.c rom512.c symbols.c

# Generate object files list from source files
OBJ = $(SRC:.c=.o)

# Generate dependency files list from source files
DEP = $(SRC:.c=.d)

all: ../napple1

debug: CFLAGS += $(DEBUG_FLAGS)
debug: ../napple1

../napple1: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Generic rule for compiling .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

# Include dependency files
-include $(DEP)

clean:
	-rm -f $(OBJ)
	-rm -f $(DEP)
	-rm -f ../napple1

