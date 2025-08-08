#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)

typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
	uint32_t biSize;
	uint32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BITMAPINFO;

typedef struct {
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbReserved;
} RGBQUAD;

#pragma pack(pop)

typedef struct {
	uint x;
	uint y;
} COORDINATES;

BITMAPFILEHEADER *read_file_header(FILE *bmp_file);

BITMAPINFO *read_info_header(FILE *bmp_file);

RGBQUAD *read_palette(FILE *bmp_file, uint palette_size);

uint8_t *read_image(FILE *bmp_file, uint size);

uint counting_extra_bytes(uint biWidth, uint biHeight, uint size_of_pixel);

void invert_palette(RGBQUAD *palette, uint palette_size);

void invert_pixels(uint8_t *array_of_pixels, uint size, uint biWidth, uint biHeight, uint count_extra_pixels);

uint pixel_comparison_8(uint8_t pixel_1, uint8_t pixel_2, RGBQUAD *palette_1, RGBQUAD *palette_2);

uint checking_for_extra_byte(uint i, uint biWidth, uint biHeight, uint count_extra_pixels_per_line, uint size_pixel);

void output_the_first_100_different_pixels(COORDINATES *different_pixels, uint count_different_pixels);

uint checking_argc_and_argv(uint argc, char **argv);

uint check_format(uint16_t biBitCount);

uint check_correct_file_size(uint32_t bfSize, uint palette_size, uint size_array_of_pixels);

void free_headers(BITMAPFILEHEADER *file_header, BITMAPINFO *info_header);

void free_palette_and_image(RGBQUAD *palette, uint8_t *array_of_pixels);

void mirror_image(uint biHeight, uint biWidth, uint count_extra_bytes_per_line, uint8_t *array, uint size_of_pixel);

void reverse_array(uint biHeight, uint biWidth, uint count_extra_bytes_per_line, uint8_t *array, uint size_of_pixel);
