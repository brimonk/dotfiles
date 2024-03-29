CC=gcc
LINKER=-lm -ldl -lpthread
CFLAGS=-Wall -g3 -march=native
SRC=src/lump.c src/main.c src/sys_linux.c src/molttest.c
OBJ=$(SRC:.c=.o)
DEP=$(OBJ:.o=.d) # one dependency file for each source

.PHONY: all

all: bin/todo bin/project bin/slides

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

-include $(DEP)

bin/todo: src/todo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LINKER)

bin/project: src/project.o
	$(CC) $(CFLAGS) -o $@ $^ $(LINKER)

bin/slides: src/slides.o
	$(CC) $(CFLAGS) -o $@ $^ $(LINKER)

clean:
	rm -f $(OBJ) $(DEP) bin/todo bin/project bin/slides
