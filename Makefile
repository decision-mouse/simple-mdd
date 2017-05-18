OBJECTS = mdd.o list.o mddqueue.o main.o
GCC = /usr/bin/gcc
CFLAGS = -g -std=c99

all: mddapp

mddapp: $(OBJECTS)
	$(GCC) $(CFLAGS) $(OBJECTS) -o mddapp

%.o : %.c
	$(GCC) $(CFLAGS) -c $<

clean:
	rm -rf *.~ $(OBJECTS) mddapp