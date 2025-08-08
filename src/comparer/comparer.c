#include "../bmp/bmp.h"

int main(int argc, char **argv) {
	if (checking_argc_and_argv(argc, argv)) {
		return 1;
	}

	FILE *image_1 = fopen(argv[1], "r");
	if (!image_1) {
		fprintf(stderr, "unable to open file\n");
		return 1;
	}
	BITMAPFILEHEADER *file_header_1 = read_file_header(image_1);
	BITMAPINFO *info_header_1 = read_info_header(image_1);
	if (check_format(info_header_1->biBitCount)) {
		free_headers(file_header_1, info_header_1);
		fclose(image_1);
		return 1;
	}
	uint need_mirror_image_1 = 0;
	if (info_header_1->biHeight < 0) {
		info_header_1->biHeight = abs(info_header_1->biHeight);
		need_mirror_image_1 = 1;
	}
	uint count_array_of_pixels_1 = info_header_1->biHeight * info_header_1->biWidth;
	uint size_of_pixel_1 = info_header_1->biBitCount;
	uint size_image_1 = count_array_of_pixels_1 * size_of_pixel_1 / 8;
	uint count_extra_bytes_1 =
	    counting_extra_bytes(info_header_1->biWidth, info_header_1->biHeight, size_of_pixel_1);
	size_image_1 += count_extra_bytes_1;
	uint palette_size_1 = (file_header_1->bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFO)) / 4;
	if (!check_correct_file_size(file_header_1->bfSize, palette_size_1 * 4, size_image_1)) {
		free_headers(file_header_1, info_header_1);
		fclose(image_1);
		return 1;
	}
	uint8_t *array_of_pixels_1 = NULL;
	uint8_t *array_of_pixels_2 = NULL;
	RGBQUAD *palette_1 = NULL;
	RGBQUAD *palette_2 = NULL;
	if (info_header_1->biBitCount == 8) {
		palette_1 = read_palette(image_1, palette_size_1);
		array_of_pixels_1 = read_image(image_1, size_image_1);
		if (need_mirror_image_1 == 1) {
			uint count_extra_bytes_1_per_line = count_extra_bytes_1 / info_header_1->biHeight;
			mirror_image(info_header_1->biHeight, info_header_1->biWidth, count_extra_bytes_1_per_line,
				     array_of_pixels_1, size_of_pixel_1);
		}
	}
	if (info_header_1->biBitCount == 24) {
		fseek(image_1, file_header_1->bfOffBits, 0);
		array_of_pixels_1 = read_image(image_1, size_image_1);
		if (need_mirror_image_1 == 1) {
			uint count_extra_bytes_1_per_line = count_extra_bytes_1 / info_header_1->biHeight;
			mirror_image(info_header_1->biHeight, info_header_1->biWidth, count_extra_bytes_1_per_line,
				     array_of_pixels_1, size_of_pixel_1);
		}
	}

	FILE *image_2 = fopen(argv[2], "r");
	if (!image_2) {
		fprintf(stderr, "unable to open file\n");
		free_headers(file_header_1, info_header_1);
		free_palette_and_image(palette_1, array_of_pixels_1);
		return 1;
	}
	BITMAPFILEHEADER *file_header_2 = read_file_header(image_2);
	BITMAPINFO *info_header_2 = read_info_header(image_2);
	if (check_format(info_header_2->biBitCount)) {
		free_headers(file_header_1, info_header_1);
		free_palette_and_image(palette_1, array_of_pixels_1);
		free_headers(file_header_2, info_header_2);
		fclose(image_1);
		fclose(image_2);
		return 1;
	}
	uint need_mirror_image_2 = 0;
	if (info_header_2->biHeight < 0) {
		info_header_2->biHeight = abs(info_header_2->biHeight);
		need_mirror_image_2 = 1;
	}
	uint count_array_of_pixels_2 = info_header_2->biHeight * info_header_2->biWidth;
	uint size_of_pixel_2 = info_header_2->biBitCount;
	uint size_image_2 = count_array_of_pixels_2 * size_of_pixel_2 / 8;
	uint count_extra_bytes_2 =
	    counting_extra_bytes(info_header_2->biWidth, info_header_2->biHeight, size_of_pixel_2);
	size_image_2 += count_extra_bytes_2;
	uint palette_size_2 = (file_header_2->bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFO)) / 4;
	if (!check_correct_file_size(file_header_2->bfSize, palette_size_2 * 4, size_image_2)) {
		free_headers(file_header_1, info_header_1);
		free_palette_and_image(palette_1, array_of_pixels_1);
		free_headers(file_header_2, info_header_2);
		fclose(image_1);
		fclose(image_2);
		return 1;
	}
	if (info_header_1->biBitCount != info_header_2->biBitCount) {
		fprintf(stderr, "the images are not comparable\n");
		free_headers(file_header_1, info_header_1);
		free_palette_and_image(palette_1, array_of_pixels_1);
		free_headers(file_header_2, info_header_2);
		fclose(image_1);
		fclose(image_2);
		return 1;
	}
	if (info_header_1->biHeight != info_header_2->biHeight) {
		fprintf(stderr, "the images are not comparable\n");
		free_headers(file_header_1, info_header_1);
		free_palette_and_image(palette_1, array_of_pixels_1);
		free_headers(file_header_2, info_header_2);
		fclose(image_1);
		fclose(image_2);
		return 1;
	}
	if (info_header_1->biWidth != info_header_2->biWidth) {
		fprintf(stderr, "the images are not comparable\n");
		free_headers(file_header_1, info_header_1);
		free_palette_and_image(palette_1, array_of_pixels_1);
		free_headers(file_header_2, info_header_2);
		fclose(image_1);
		fclose(image_2);
		return 1;
	}
	if (info_header_2->biBitCount == 8) {
		uint palette_size_2 = (file_header_2->bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFO)) / 4;
		palette_2 = read_palette(image_2, palette_size_2);
		array_of_pixels_2 = read_image(image_2, size_image_2);
		if (need_mirror_image_2 == 1) {
			uint count_extra_bytes_2_per_line = count_extra_bytes_2 / info_header_2->biHeight;
			mirror_image(info_header_2->biHeight, info_header_2->biWidth, count_extra_bytes_2_per_line,
				     array_of_pixels_2, size_of_pixel_2);
		}
	}
	if (info_header_2->biBitCount == 24) {
		fseek(image_2, file_header_2->bfOffBits, 0);
		array_of_pixels_2 = read_image(image_2, size_image_2);
		if (need_mirror_image_2 == 1) {
			uint count_extra_bytes_2_per_line = count_extra_bytes_2 / info_header_2->biHeight;
			mirror_image(info_header_2->biHeight, info_header_2->biWidth, count_extra_bytes_2_per_line,
				     array_of_pixels_2, size_of_pixel_2);
		}
	}

	uint count_extra_bytes_per_line = count_extra_bytes_2 / info_header_1->biWidth;
	uint count_extra_bytes = 0;
	COORDINATES *different_pixels = calloc(100, sizeof(COORDINATES));
	uint count_different_pixels = 0;
	if ((info_header_2->biBitCount == 8) && (info_header_1->biBitCount == 8)) {
		for (uint i = 0; i < size_image_1; i++) {
			if (checking_for_extra_byte(i, info_header_1->biWidth, info_header_1->biHeight,
						    count_extra_bytes_per_line, 1)) {
				count_extra_bytes++;
				continue;
			}
			if (!pixel_comparison_8(array_of_pixels_1[i], array_of_pixels_2[i], palette_1, palette_2)) {
				if (count_different_pixels < 100) {
					different_pixels[count_different_pixels].x =
					    (i - count_extra_bytes) % info_header_1->biWidth;
					different_pixels[count_different_pixels].y =
					    (i - count_extra_bytes) / info_header_1->biWidth;
					count_different_pixels++;
				}
			}
		}
	}
	if ((info_header_2->biBitCount == 24) && (info_header_1->biBitCount == 24)) {
		for (uint i = 0; i < size_image_1 - 2; i += 3) {
			if ((checking_for_extra_byte(i, info_header_1->biWidth, info_header_1->biHeight,
						     count_extra_bytes_per_line, 3)) &&
			    (i >= 2)) {
				count_extra_bytes++;
				i -= 2;
				continue;
			}
			if ((array_of_pixels_1[i] != array_of_pixels_2[i]) ||
			    (array_of_pixels_1[i + 1] != array_of_pixels_2[i + 1]) ||
			    (array_of_pixels_1[i + 2] != array_of_pixels_2[i + 2])) {
				if (count_different_pixels < 100) {
					different_pixels[count_different_pixels].x =
					    ((i - count_extra_bytes) / 3) % info_header_1->biWidth;
					different_pixels[count_different_pixels].y =
					    ((i - count_extra_bytes) / 3) / info_header_1->biWidth;
					count_different_pixels++;
				}
			}
		}
	}
	free_headers(file_header_1, info_header_1);
	free_palette_and_image(palette_1, array_of_pixels_1);
	free_headers(file_header_2, info_header_2);
	free_palette_and_image(palette_2, array_of_pixels_2);
	fclose(image_1);
	fclose(image_2);
	if (count_different_pixels != 0) {
		output_the_first_100_different_pixels(different_pixels, count_different_pixels);
	}
	free(different_pixels);
	if (count_different_pixels != 0) {
		return 2;
	}
	printf("Images are same\n");
	return 0;
}
