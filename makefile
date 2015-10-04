all: const_time

const_time: const_time.c
	$(CC) -O3 -Wall const_time.c -o const_time

clean:
	-rm -f const_time
