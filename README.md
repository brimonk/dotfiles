# Brian's Dotfiles (and Random Repo)

**WARNING** everything in `src/` is supposed to be programs that get compiled and put into$HOME/bin

## IDEAS

### Single Header File Libs

#### PHEAP

- Persistent Heap
- Deps: C STDLIB

This is mostly a research project. It wouldn't really be used to make super fast programs or
anything, it would 100% be a convenience. And, all of the magic would be basically in the regular
\*alloc function overloading, and an additional two other functions:

```c
size_t palloc_loadf();
size_t palloc_loadm();
size_t palloc_savef();
size_t palloc_savem();
```

Where, an application programmer can load their program heap from a file, from some memory they got
from somewhere else, and save it to a file or to another buffer.

## TODO

- figure out how to get a makefile to make one exe per .c file (in a more automatic way)

