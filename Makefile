CFLAGS := -O2 -Wall -Wextra

.PHONY: all
all: bin2c

.PHONY: clean
clean:
	rm -f bin2c

bin2c: bin2c.c
	$(CC) $(CFLAGS) -o $@ $<
