
test: main.out
	./main.out -n 4 -q 5 -t 3 -p 0.75 -b 0.05

main.out: main.c main.h queue.o log.h
	gcc main.c -o main.out queue.o


queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o


