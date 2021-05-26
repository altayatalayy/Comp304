
SRCFILES = $(shell find src/ -type f -name '*.c')
OBJFILES = $(subst src/, bin, $(patsubst %.c, %.o, $(SRCFILES)))

TARGET = ./bin/main.out

test: $(TARGET)
	$(TARGET) -n 1 -q 2 -t 5 -p 0.5 -b 0.5

$(TARGET): main.c $(OBJFILES)
	echo $(OBJFILES)
	gcc main.c -o $(TARGET) $(OBJFILES) -I ./include

bin/%.o: src/%.c include/%.h
	gcc -c $(subst bin, src, $<) -o $@ -I ./include

clean:
	rm -rf bin

.PHONY:
	clean
