.PHONY: clean all

CFLAGS+=-ggdb -O0 -Wall

all: problem1.bin problem2.bin problem3.bin


sim_example.bin: sim_example.o simulator.o
	${CC} ${CFLAGS} -o $@ $^

problem1.bin: simulator.o
%.bin: %.o
	${CC} ${CFLAGS} -o $@ $^ -lpthread


clean:
	rm -rf *.bin *.o
