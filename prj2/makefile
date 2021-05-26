
test: main.out
	./main.out -n 1 -q 2 -t 5 -p 0.5 -b 0.5

main.out: main.c main.h queue.o log.h
	gcc main.c -o main.out queue.o


queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o


