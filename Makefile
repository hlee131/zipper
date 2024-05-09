all:
	gcc helper.c zip.c collections.c crc.c deflate.c datetime.c huffman.c lz77.c writer.c -o zipper

debug:
	gcc -g helper.c zip.c collections.c crc.c deflate.c datetime.c huffman.c lz77.c writer.c -o debug_zipper

zips:
	./zipper crc.c & zip test.zip crc.c

clean:
	rm zipper debug_zipper *.zip