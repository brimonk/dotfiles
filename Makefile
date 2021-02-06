CC=cc
LINKER=-ldl -lm -lreadline
FLAGS=-Wall -g3 -march=native
TARGET=bin/jankometer
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(OBJ:.o=.d) # one dependency file for each source

all: $(TARGET)

%.d: %.c
	@$(CC) $(FLAGS) $< -MM -MT $(@:.d=.o) >$@

%.o: %.c
	$(CC) -c $(FLAGS) $(PREPROCESSPARMS) -o $@ $<

-include $(DEP)

$(TARGET): $(OBJ)
	$(CC) $(FLAGS) -o $(TARGET) $(OBJ) $(LINKER)

clean: clean-obj clean-bin

clean-obj:
	rm -f $(OBJ) $(DEP)
	
clean-bin:
	rm -f $(shell find . -maxdepth 1 -executable -type f)

