#CXX=g++

CFLAGS= #-gdb #-DDMALLOC_FUNC_CHECK
LIBS=-lm
LIBS+=-lncurses

kmc: kmc.o
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf kmc *.o *~
