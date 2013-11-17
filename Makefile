CC = gcc

all: bitmapheader

bitmapheader: bitmapheader.c
	$(CC) -o $@ $<

clean:
	rm bitmapheader

