#include "bmp.h"

BITMAPFILEHEADER *read_file_header(FILE *bmp_file) {
	BITMAPFILEHEADER *file_header = malloc(sizeof(BITMAPFILEHEADER));
	fread(file_header, sizeof(BITMAPFILEHEADER), 1, bmp_file);
	return file_header;
}

BITMAPINFO *read_info_header(FILE *bmp_file) {
	BITMAPINFO *info_header = malloc(sizeof(BITMAPINFO));
	fread(info_header, sizeof(BITMAPINFO), 1, bmp_file);
	return info_header;
}

RGBQUAD *read_palette(FILE *bmp_file, uint palette_size) {
	RGBQUAD *palette = malloc(sizeof(RGBQUAD) * palette_size);
	fread(palette, sizeof(RGBQUAD) * palette_size, 1, bmp_file);
	return palette;
}

uint counting_extra_bytes(uint biWidth, uint biHeight, uint size_of_pixel) {
	uint number_extra_bytes = 0;
	uint number_of_extra_bytes_per_line = 0;
	if (size_of_pixel == 8) {
		number_of_extra_bytes_per_line = (3 * biWidth) % 4;
	} else {
		number_of_extra_bytes_per_line = (biWidth % 4);
	}
	number_extra_bytes = number_of_extra_bytes_per_line * biHeight;
	return number_extra_bytes;
}

uint8_t *read_image(FILE *bmp_file, uint size) {
	uint8_t *array_of_pixels = malloc(sizeof(uint8_t) * size);
	fread(array_of_pixels, sizeof(uint8_t) * size, 1, bmp_file);
	return array_of_pixels;
}

void invert_palette(RGBQUAD *palette, uint palette_size) {
	for (uint i = 0; i < palette_size; i++) {
		palette[i].rgbRed = 255 - palette[i].rgbRed;
		palette[i].rgbBlue = 255 - palette[i].rgbBlue;
		palette[i].rgbGreen = 255 - palette[i].rgbGreen;
	}
}

uint checking_for_extra_byte(uint i, uint biWidth, uint biHeight, uint count_extra_pixels_per_line, uint size_pixel) {
	if (count_extra_pixels_per_line == 0) {
		return 0;
	}
	uint offset = 0;
	for (uint j = 0; j < biHeight; j++) {
		for (uint k = offset; k < count_extra_pixels_per_line + offset; k++) {
			if (i == biWidth * (j + 1) * size_pixel + k) {
				return 1;
			}
		}
		offset += count_extra_pixels_per_line;
	}
	return 0;
}

void invert_pixels(uint8_t *array_of_pixels, uint size, uint biWidth, uint biHeight, uint count_extra_pixels) {
	uint count_extra_pixels_per_line = count_extra_pixels / biHeight;
	for (uint i = 0; i < size; i++) {
		if (checking_for_extra_byte(i, biWidth, biHeight, count_extra_pixels_per_line, 3)) {
			continue;
		}
		array_of_pixels[i] = 255 - array_of_pixels[i];
	}
}

uint pixel_comparison_8(uint8_t pixel_1, uint8_t pixel_2, RGBQUAD *palette_1, RGBQUAD *palette_2) {
	if ((palette_1[pixel_1].rgbBlue == palette_2[pixel_2].rgbBlue) &&
	    (palette_1[pixel_1].rgbGreen == palette_2[pixel_2].rgbGreen) &&
	    (palette_1[pixel_1].rgbRed == palette_2[pixel_2].rgbRed)) {
		return 1;
	}
	return 0;
}

void output_the_first_100_different_pixels(COORDINATES *different_pixels, uint count_different_pixels) {
	fprintf(stderr, "Next pixels are different:\n");
	for (uint i = 0; i < count_different_pixels; i++) {
		fprintf(stderr, "x%-6d y%-6d\n", different_pixels[i].x, different_pixels[i].y);
	}
}

uint checking_argc_and_argv(uint argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "invalid command line arguments\n");
		return 1;
	}
	if (strncmp(&(argv[1][strlen(argv[1]) - 4]), ".bmp", 4) != 0) {
		fprintf(stderr, "invalid command line arguments\n");
		return 1;
	}
	if (strncmp(&(argv[2][strlen(argv[2]) - 4]), ".bmp", 4) != 0) {
		fprintf(stderr, "invalid command line arguments\n");
		return 1;
	}
	return 0;
}

uint check_format(uint16_t biBitCount) {
	if (!((biBitCount == 8) || (biBitCount == 24))) {
		fprintf(stderr, "unsupported format\n");
		return 1;
	}
	return 0;
}

uint check_correct_file_size(uint32_t bfSize, uint palette_size, uint size_array_of_pixels) {
	if ((uint)bfSize < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + palette_size + size_array_of_pixels) {
		fprintf(stderr, "invalid file size\n");
		return 0;
	}
	return 1;
}

void free_headers(BITMAPFILEHEADER *file_header, BITMAPINFO *info_header) {
	free(file_header);
	free(info_header);
}

void free_palette_and_image(RGBQUAD *palette, uint8_t *array_of_pixels) {
	free(palette);
	free(array_of_pixels);
}

void mirror_image(uint biHeight, uint biWidth, uint count_extra_bytes_per_line, uint8_t *array, uint size_of_pixel) {
	size_of_pixel /= 8;
	biWidth *= size_of_pixel;
	uint8_t tmp = 0;
	uint one_line = biWidth + count_extra_bytes_per_line;
	for (uint i = 0; i < biHeight / 2; i++) {
		for (uint j = 0; j < one_line; j++) {
			tmp = array[i * one_line + j];
			array[i * one_line + j] = array[(biHeight - i - 1) * one_line + j];
			array[(biHeight - i - 1) * one_line + j] = tmp;
		}
	}
}

void reverse_array(uint biHeight, uint biWidth, uint count_extra_bytes_per_line, uint8_t *array, uint size_of_pixel) {
	size_of_pixel /= 8;
	biWidth *= size_of_pixel;
	uint count_extra_bytes = 0;
	uint8_t tmp = 0;
	uint flag = 1;
	for (uint i = 0; i < (biHeight + 1) / 2; i++) {
		if (i == biHeight / 2) {
			flag = 2;
		}
		for (uint j = 0; j < biWidth / flag; j++) {
			tmp = array[i * biWidth + count_extra_bytes + j];
			array[i * biWidth + count_extra_bytes + j] =
			    array[(biHeight - i) * biWidth + count_extra_bytes_per_line * (biHeight - 1 - i) - j - 1];
			array[(biHeight - i) * biWidth + count_extra_bytes_per_line * (biHeight - 1 - i) - j - 1] = tmp;
		}
		count_extra_bytes += count_extra_bytes_per_line;
	}
	if (size_of_pixel == 3) {
		count_extra_bytes = 0;
		for (uint i = 0; i < biHeight; i++) {
			for (uint j = 0; j < biWidth; j += 3) {
				tmp = array[i * biWidth + count_extra_bytes + j];
				array[i * biWidth + count_extra_bytes + j] =
				    array[i * biWidth + count_extra_bytes + j + 2];
				array[i * biWidth + count_extra_bytes + j + 2] = tmp;
			}
			count_extra_bytes += count_extra_bytes_per_line;
		}
	}
}
