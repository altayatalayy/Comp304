
SRCFILES = $(shell find src/ -type f -name '*.c')
OBJFILES = $(subst src/, bin, $(patsubst %.c, %.o, $(SRCFILES)))

TARGET = ./bin/main.out

test: $(TARGET)
	$(TARGET) -n 4 -p 0.75 -q 5 -t 3 -b 0.05

$(TARGET): main.c $(OBJFILES)
	echo $(OBJFILES)
	gcc main.c -o $(TARGET) $(OBJFILES) -I ./include

bin/%.o: src/%.c include/%.h
	gcc -c $(subst bin, src, $<) -o $@ -I ./include

clean:
	rm -rf bin

.PHONY:
	clean
