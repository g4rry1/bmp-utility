#include "../bmp/bmp.h"

int main(int argc, char **argv) {
	if (checking_argc_and_argv(argc, argv)) {
		return 1;
	}
	FILE *input_image = fopen(argv[1], "r");
	if (!input_image) {
		fprintf(stderr, "unable to open file\n");
		return 1;
	}
	BITMAPFILEHEADER *file_header = read_file_header(input_image);
	BITMAPINFO *info_header = read_info_header(input_image);
	if (check_format(info_header->biBitCount)) {
		free_headers(file_header, info_header);
		fclose(input_image);
		return 1;
	}
	uint count_array_of_pixels = abs(info_header->biHeight) * info_header->biWidth;
	uint size_of_pixel = info_header->biBitCount;
	uint size_image = count_array_of_pixels * size_of_pixel / 8;
	uint count_extra_bytes = counting_extra_bytes(info_header->biWidth, abs(info_header->biHeight), size_of_pixel);
	size_image += count_extra_bytes;
	uint palette_size = (file_header->bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFO)) / 4;
	if (!check_correct_file_size(file_header->bfSize, palette_size * 4, size_image)) {
		free_headers(file_header, info_header);
		fclose(input_image);
		return 1;
	}
	FILE *output_image = fopen(argv[2], "w");
	if (!output_image) {
		fprintf(stderr, "unable to open file\n");
		free_headers(file_header, info_header);
		fclose(input_image);
		return 1;
	}
	if (size_of_pixel == 8) {
		RGBQUAD *palette = read_palette(input_image, palette_size);
		uint8_t *array_of_pixels_8 = read_image(input_image, size_image);
		fwrite(file_header, sizeof(BITMAPFILEHEADER), 1, output_image);
		fwrite(info_header, sizeof(BITMAPINFO), 1, output_image);
		invert_palette(palette, palette_size);
		fwrite(palette, sizeof(RGBQUAD) * palette_size, 1, output_image);
		fwrite(array_of_pixels_8, sizeof(uint8_t) * size_image, 1, output_image);
		free(palette);
		free(array_of_pixels_8);
	}
	if (size_of_pixel == 24) {
		if (palette_size > 0) {
			RGBQUAD *palette = read_palette(input_image, palette_size);
			fwrite(file_header, sizeof(BITMAPFILEHEADER), 1, output_image);
			fwrite(info_header, sizeof(BITMAPINFO), 1, output_image);
			fwrite(palette, sizeof(RGBQUAD) * palette_size, 1, output_image);
		} else {
			fwrite(file_header, sizeof(BITMAPFILEHEADER), 1, output_image);
			fwrite(info_header, sizeof(BITMAPINFO), 1, output_image);
		}
		uint8_t *array_of_pixels_24 = read_image(input_image, size_image);
		invert_pixels(array_of_pixels_24, size_image, info_header->biWidth, abs(info_header->biHeight),
			      count_extra_bytes);
		fwrite(array_of_pixels_24, sizeof(uint8_t) * size_image, 1, output_image);
		free(array_of_pixels_24);
	}
	uint count_bytes_trash =
	    file_header->bfSize - (size_image + 4 * palette_size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO));
	if (count_bytes_trash > 0) {
		uint8_t *bytes_trash = malloc(sizeof(uint8_t) * count_bytes_trash);
		fread(bytes_trash, sizeof(uint8_t) * count_bytes_trash, 1, input_image);
		fwrite(bytes_trash, sizeof(uint8_t) * count_bytes_trash, 1, output_image);
		free(bytes_trash);
	}
	free_headers(file_header, info_header);
	fclose(input_image);
	fclose(output_image);

	return 0;
}
