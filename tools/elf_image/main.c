#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE	4096

FILE * fout;

static int write_bytes(char * b, size_t bytes)
{
	size_t written;

	while (bytes && bytes > (written = fwrite(b, 1, bytes,	fout))) {
		if (written == 0 && ferror(fout)) {
			printf("Writout failed\n");
			return -1;
		}
		b += written;
		bytes -= written;
	}

	return 0;
}

int main(int argc, char ** argv)
{
	int i;
	char buff[BUF_SIZE];
	unsigned int osz = 0;

	if (argc < 3) {
		printf("usage <fout> <files in ...>\n");
		return -1;
	}

	fout = fopen(argv[1], "w");
	if (!fout) {
		printf("Cannot open output\n");
		return -1;
	}

	printf("Building ELF boot image %s ... \n", argv[1]);
	for(i = 2; i < argc; i++)
	{
		FILE * fin;
		size_t bytes;

		printf("adding %s offset 0x%x\n", argv[i], osz);
		fin = fopen(argv[i], "r");
		if (!fin) {
			printf("Cannot open file\n");
			return -1;
		}

		while ((bytes = fread(buff, 1, BUF_SIZE, fin))) {
			if (write_bytes(buff, bytes))
				return -1;
			osz += bytes;
		}

		if (i + 1 < argc) {
			bytes = 512 - (osz & 511);
			memset(buff, 0, bytes);
			if (write_bytes(buff, bytes))
				return -1;
			osz += bytes;

		}
	}
	printf("image ready\n");

	return 0;
}
