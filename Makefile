all: main.c utils.c fifo.c sjf.c psjf.c rr.c task.c
	gcc -o main main.c utils.c fifo.c sjf.c psjf.c rr.c -lrt
	gcc -o task task.c utils.c -lrt

clean:
	rm -f main task
