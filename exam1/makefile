SOURCES := problem1.c problem2.c p2worker.bin problem3.bin
TARGETS := $(SOURCES:%.c=%.bin)

all: $(TARGETS)

CC = gcc
CFLAGS += -ggdb -Werror
LDFLAGS += -lm

.PHONY: clean all

p2worker.bin: util/p2worker.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

problem1.bin: util/p1.o
%.bin: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf *.o *.bin *.log

