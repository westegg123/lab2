SRC = $(wildcard src/*.c)
INPUT ?= $(wildcard inputs/*.x)

.PHONY: all

all: sim

sim: $(SRC)
	gcc -g -O2 $^ -o $@

clean:
	rm -rf *.o *~ sim