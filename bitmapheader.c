// bitmapheader.c - Add a windows bitmap (.bmp) header to arbitrary files
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// http://de.wikipedia.org/wiki/Windows_Bitmap

void fwrite_c(char c, FILE* fp) {
	fwrite(&c, 1, 1, fp);
}

// wrapping stuff for little endian output of uint16/32s

void fwrite_ui16(uint16_t i, FILE* fp) {
	// TODO: check endianess of system?
	char c;
	c = i & 0xff;
	fwrite(&c, 1, 1, fp);
	c = (i >> 8) & 0xff;
	fwrite(&c, 1, 1, fp);
}

void fwrite_ui32(uint32_t i, FILE* fp) {
	// TODO: check endianess of system?
	char c;
	c = i & 0xff;
	fwrite(&c, 1, 1, fp);
	c = (i >> 8) & 0xff;
	fwrite(&c, 1, 1, fp);
	c = (i >> 16) & 0xff;
	fwrite(&c, 1, 1, fp);
	c = (i >> 24) & 0xff;
	fwrite(&c, 1, 1, fp);
}

int main(int argc, char **argv) {

	int data_bytes;
	int h;
	int w = 1024;
	int depth = 24;

	FILE *fin;
	FILE *fout;

	if (argc != 3) {
		printf("Usage: %s inputfile outputfile.bmp\n", argv[0]);
		return 1;
	}

	fin = fopen(argv[1], "r");
	if (fin == NULL) {
		printf("Unable to open %s\n for reading.", argv[1]);
		return 1;
	}

	fout = fopen(argv[2], "w");
	if (fout == NULL) {
		printf("Unable to open %s for writing.\n", argv[2]);
		return 1;
	}

	// determine filesize
	fseek(fin, 0L, SEEK_END);
	data_bytes = ftell(fin);
	fseek(fin, 0L, SEEK_SET);

	h = data_bytes / w;

	// BITMAPFILEHEADER
	// uint16_t bfType;
	fwrite_c('B', fout);
	fwrite_c('M', fout);
	// uint32_t bfSize;
	fwrite_ui32(14 + 40 + (w * h) * (depth / 8), fout); // TODO
	// uint32_t bfReserved;
	fwrite_ui32(0, fout);
	// uint32_t bfOffBits;
	fwrite_ui32(14 + 40, fout);

	// BITMAPINFOHEADER
	// uint32_t biSize;
	fwrite_ui32(40, fout);
	// int32_t biWidth;
	fwrite_ui32(w, fout);
	// int32_t biHeight;
	fwrite_ui32(-h, fout); // negative: rows from top to bottom
	// uint16_t biPlanes;
	fwrite_ui16(1, fout);
	// uint16_t biBitCount;
	fwrite_ui16(depth, fout);
	// uint32_t biCompression;
	fwrite_ui32(0, fout);
	// uint32_t biSizeImage;
	fwrite_ui32(0, fout); // (TODO)
	// int32_t biXPelsPerMeter;
	fwrite_ui32(0, fout);
	// int32_t biYPelsPerMeter;
	fwrite_ui32(0, fout);
	// uint32_t biClrUsed;
	fwrite_ui32(0, fout);
	// uint32_t biClrImportant;
	fwrite_ui32(0, fout);

	// fwrite(fh, sizeof(bfh), 1, fout);
	int i;
	char c;
	for (i = 0; i < data_bytes; i++) {
		fread(&c, 1, 1, fin);
		// grey scale
		fwrite_c(c & 0b11111111, fout); // B
		fwrite_c(c & 0b11111111, fout); // G
		fwrite_c(c & 0b11111111, fout); // R
	}
	fclose(fout);
	fclose(fin);
	return 0;
}

