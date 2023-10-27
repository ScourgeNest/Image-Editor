#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 10
#define I_AM_HANDSOME 1
#define MAX_FILE_NAME 1024

#define LOAD 9999
#define SELECT 9998
#define HISTOGRAM 9997
#define EQUALIZE 9996
#define ROTATE 9995
#define CROP 9994
#define APPLY 9993
#define SAVE 9992
#define EXIT 9991

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} pixel;

typedef struct {
	int type;
	int width;
	int length;
	char *file_name;
	unsigned char **matrix;
	pixel **color_matrix;
	unsigned char max_color;
	int x1, x2, y1, y2;
	short loaded;
} image_prop;

int which_command(char *command)
{
	if (strcmp(command, "LOAD") == 0)
		return -LOAD;
	else if (strcmp(command, "SELECT") == 0)
		return -SELECT;
	else if (strcmp(command, "HISTOGRAM") == 0)
		return -HISTOGRAM;
	else if (strcmp(command, "EQUALIZE") == 0)
		return -EQUALIZE;
	else if (strcmp(command, "ROTATE") == 0)
		return -ROTATE;
	else if (strcmp(command, "CROP") == 0)
		return -CROP;
	else if (strcmp(command, "APPLY") == 0)
		return -APPLY;
	else if (strcmp(command, "SAVE") == 0)
		return -SAVE;
	else if (strcmp(command, "EXIT") == 0)
		return -EXIT;
	else
		return 69;
}

pixel **pixel_alloc_matrix(int width, int length)
{
	pixel **tmp_pixel_matrix = (pixel **)malloc(width * sizeof(pixel *));
	if (!tmp_pixel_matrix) {
		printf("Failed to allocate tmp");
		free(tmp_pixel_matrix);
	}
	for (int i = 0; i < width; i++) {
		tmp_pixel_matrix[i] = (pixel *)malloc(length * sizeof(pixel));
		if (!tmp_pixel_matrix[i]) {
			printf("Failed to allocate tmp[i]");
			free(tmp_pixel_matrix[i]);
			for (int j = i - 1; j >= 0; j--)
				free(tmp_pixel_matrix[j]);
			free(tmp_pixel_matrix);
			break;
		}
	}
	return tmp_pixel_matrix;
}

unsigned char **alloc_matrix(int width, int length)
{
	unsigned char **tmp_matrix;
	tmp_matrix = (unsigned char **)malloc(width * sizeof(unsigned char *));
	if (!tmp_matrix) {
		printf("Failed to allocate tmp_matrix\n");
		free(tmp_matrix);
		exit(1);
	}
	for (int i = 0; i < width; i++) {
		tmp_matrix[i] = (unsigned char *)malloc(length * sizeof(unsigned char));
		if (!tmp_matrix[i]) {
			printf("Failed to allocate tmp_matrix\n");
			for (int j = i; j > 0; j--)
				free(tmp_matrix[i]);
			free(tmp_matrix);
			exit(1);
		}
	}
	return tmp_matrix;
}

void get_image_name(image_prop *image)
{
	char *file_name;
	int dim = MAX_FILE_NAME;

	file_name = (char *)malloc(sizeof(char) * dim);
	if (!file_name) {
		printf("Nu s-a putut aloca memoria necesara.");
		free(file_name);
		exit(1);
	}
	scanf("%s", file_name);

	image->file_name = file_name;
}

void alloc_matrix_pixel(image_prop *image)
{
	pixel **tmp = (pixel **)malloc(image->width * sizeof(pixel *));
	if (!tmp) {
		printf("Failed to allocate tmp");
		free(tmp);
	}
	for (int i = 0; i < image->width; i++) {
		tmp[i] = (pixel *)malloc(image->length * sizeof(pixel));
		if (!tmp[i]) {
			printf("Failed to allocate tmp[i]");
			free(tmp[i]);
			for (int j = i - 1; j >= 0; j--)
				free(tmp[j]);
			free(tmp);
			break;
		}
	}
	image->color_matrix = tmp;
}

void alloc_matrix_unsigned_char(image_prop *image)
{
	unsigned char **tmp =
			(unsigned char **)malloc(image->width * sizeof(unsigned char *));
	if (!tmp) {
		printf("Failed to allocate tmp");
		free(tmp);
	}
	for (int i = 0; i < image->width; i++) {
		tmp[i] = (unsigned char *)malloc(image->length * sizeof(unsigned char));
		if (!tmp[i]) {
			printf("Failed to allocate tmp[i]");
			free(tmp[i]);
			for (int j = i - 1; j >= 0; j--)
				free(tmp[j]);
			free(tmp);
			break;
		}
	}
	image->matrix = tmp;
}

void ascii_grayscale(image_prop *image, FILE *load)
{
	alloc_matrix_unsigned_char(image);
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			int tmp;
			fscanf(load, "%d", &tmp);
			image->matrix[i][j] = (unsigned char)tmp;
		}
	}
}

void ascii_color(image_prop *image, FILE *load)
{
	alloc_matrix_pixel(image);
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			int tmp_red;
			fscanf(load, "%d", &tmp_red);
			image->color_matrix[i][j].red = (char)tmp_red;
			int tmp_green;
			fscanf(load, "%d", &tmp_green);
			image->color_matrix[i][j].green = (char)tmp_green;
			int tmp_blue;
			fscanf(load, "%d", &tmp_blue);
			image->color_matrix[i][j].blue = (char)tmp_blue;
		}
	}
}

void binary_grayscale(image_prop *image, FILE *load)
{
	char end_of_line;
	fscanf(load, "%c", &end_of_line);
	alloc_matrix_unsigned_char(image);
	for (int i = 0; i < image->width; i++)
		for (int j = 0; j < image->length; j++)
			fread(&image->matrix[i][j], sizeof(unsigned char), 1, load);
}

void binary_color(image_prop *image, FILE *load)
{
	char end_of_line;
	fscanf(load, "%c", &end_of_line);
	scanf("%c", &end_of_line);
	alloc_matrix_pixel(image);
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			fread(&image->color_matrix[i][j].red, sizeof(unsigned char),
				  1, load);
			fread(&image->color_matrix[i][j].green, sizeof(unsigned char),
				  1, load);
			fread(&image->color_matrix[i][j].blue, sizeof(unsigned char),
				  1, load);
		}
	}
}

int border(image_prop image, int i, int j)
{
	if (i == 0 || j == 0 || i == image.width - 1 ||
		j == image.length - 1)
		return 1;
	return 0;
}

void load_command(image_prop *image)
{
	if (image->loaded == 1) {
		if (image->type == 2 || image->type == 5) {
			for (int i = 0; i < image->width; i++)
				free(image->matrix[i]);
			free(image->matrix);
			free(image->file_name);
		} else if (image->type == 3 || image->type == 6) {
			for (int i = 0; i < image->width; i++)
				free(image->color_matrix[i]);
			free(image->color_matrix);
			free(image->file_name);
		}
	}
	get_image_name(image);

	FILE *load;
	load = fopen(image->file_name, "r");
	if (!load) {
		printf("Failed to load %s\n", image->file_name);
		free(image->file_name);
		image->loaded = 0;
		return;
	}
	char P;
	fscanf(load, "%c%d", &P, &image->type);
	if (image->type != 2 && image->type != 3 && image->type != 5 &&
		image->type != 6) {
		printf("Failed to load %s\n", image->file_name);
		fclose(load);
		free(image->file_name);
		image->loaded = 0;
		return;
	}
	fscanf(load, "%d %d", &image->length, &image->width);
	if (image->width < 0 || image->length < 0) {
		printf("Failed to load %s\n", image->file_name);
		fclose(load);
		free(image->file_name);
		image->loaded = 0;
		return;
	}
	int tmp;
	fscanf(load, "%d", &tmp);
	image->max_color = (unsigned char)tmp;
	switch (image->type) {
	case 2:
		ascii_grayscale(image, load);
		break;

	case 3:
		ascii_color(image, load);
		break;

	case 5:
		binary_grayscale(image, load);
		break;

	case 6:
		binary_color(image, load);
		break;

	default:
		printf("Failed to load %s", image->file_name);
		fclose(load);
		return;
	}
	printf("Loaded %s\n", image->file_name);
	image->loaded = 1;
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = image->length;
	image->y2 = image->width;

	fclose(load);
}

int verify(image_prop *image, int x1, int y1, int x2, int y2)
{
	if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0)
		return 1;
	if (x1 > image->length || x2 > image->length)
		return 1;
	if (y1 > image->width || y2 > image->width)
		return 1;
	if (x1 == x2 || y1 == y2)
		return 1;
	return 0;
}

void image_loaded(int *x1, int *y1, int *x2, int *y2, image_prop *image)
{
	if (image->loaded == 1) {
		*x1 = image->x1;
		*x2 = image->x2;
		*y1 = image->y1;
		*y2 = image->y2;
	}
}

void swap_coord(image_prop *image)
{
	if (image->x1 > image->x2) {
		int aux_x = image->x1;
		image->x1 = image->x2;
		image->x2 = aux_x;
	}
	if (image->y1 > image->y2) {
		int aux_y = image->y1;
		image->y1 = image->y2;
		image->y2 = aux_y;
	}
}

void select_command(image_prop *image)
{
	char *all, space;
	scanf("%c", &space);
	int c_x1, c_x2, c_y1, c_y2;
	image_loaded(&c_x1, &c_y1, &c_x2, &c_y2, image);
	all = (char *)malloc(MAX_FILE_NAME * sizeof(char));
	fgets(all, MAX_FILE_NAME, stdin);
	all[strlen(all)] = '\0';
	int count_coord = 0;
	char *p = strtok(all, " ");
	if (strncmp(p, "ALL", 3) == 0) {
		if (image->loaded == 0) {
			printf("No image loaded\n");
			free(all);
			return;
		}
		image->x1 = 0;
		image->y1 = 0;
		image->x2 = image->length;
		image->y2 = image->width;
		printf("Selected ALL\n");
		free(all);
		return;
	}
	while (p && count_coord < 4) {
		if (isdigit(*p) || isdigit(p[1])) {
			switch (count_coord) {
			case 0:
				image->x1 = atoi(p);
				break;
			case 1:
				image->y1 = atoi(p);
				break;
			case 2:
				image->x2 = atoi(p);
				break;
			case 3:
				image->y2 = atoi(p);
				break;
			}
			count_coord++;
		}
		p = strtok(NULL, " ");
	}
	if (count_coord != 4) {
		image->x1 = c_x1;
		image->x2 = c_x2;
		image->y1 = c_y1;
		image->y2 = c_y2;
		printf("Invalid command\n");
		free(all);
		return;
	}
	if (p) {
		image->x1 = c_x1;
		image->x2 = c_x2;
		image->y1 = c_y1;
		image->y2 = c_y2;
		printf("Invalid command\n");
		free(all);
		return;
	}
	swap_coord(image);
	if (image->loaded == 0) {
		printf("No image loaded\n");
	} else if (verify(image, image->x1, image->y1, image->x2, image->y2)) {
		printf("Invalid set of coordinates\n");
		image->x1 = c_x1;
		image->x2 = c_x2;
		image->y1 = c_y1;
		image->y2 = c_y2;
	} else {
		printf("Selected %d %d %d %d\n", image->x1, image->y1, image->x2,
			   image->y2);
	}
	free(all);
}

void histogram_command(image_prop *image)
{
	char end_of_line;
	scanf("%c", &end_of_line);
	if (end_of_line == '\n') {
		if (image->loaded == 0) {
			printf("No image loaded\n");
			return;
		}
			printf("Invalid command\n");
			return;
	}
	int *v_freq;
	int x = -69, y = -69;
	scanf("%d", &x);
	scanf("%c", &end_of_line);
	if (end_of_line == '\n') {
		printf("Invalid command\n");
		return;
	}
	scanf("%d", &y);
	scanf("%c", &end_of_line);
	if (end_of_line != '\n') {
		char *useless = malloc(20);
		printf("Invalid command\n");
		fgets(useless, 20, stdin);
		free(useless);
		return;
	}
	if (image->loaded == 0) {
		printf("No image loaded\n");
		return;
	} else if (image->type == 3 || image->type == 6) {
		printf("Black and white image needed\n");
		return;
	}
	int *tmp = (int *)calloc(y, sizeof(int));
	if (!tmp) {
		printf("Failed of allocate tmp\n");
		exit(1);
	}
	v_freq = tmp;
	int interval = 256 / y;
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			int nr = 0, copy_interval = interval;
			while ((int)image->matrix[i][j] >= copy_interval) {
				nr++;
				copy_interval += interval;
			}
			v_freq[nr]++;
		}
	}
	int max_freq = 0;
	for (int i = 0; i < y; i++)
		if (v_freq[i] > max_freq)
			max_freq = v_freq[i];
	for (int i = 0; i < y; i++)
		v_freq[i] = (v_freq[i] * x) / max_freq;
	for (int i = 0; i < y; i++) {
		printf("%d\t|\t", v_freq[i]);
		for (int j = 0; j < v_freq[i]; j++)
			printf("*");
		printf("\n");
	}
	free(v_freq);
}

void equalize_command(image_prop *image)
{
	if (image->loaded == 0) {
		printf("No image loaded\n");
		return;
	} else if (image->type == 3 || image->type == 6) {
		printf("Black and white image needed\n");
		return;
	}
	int v_freq[256] = {0};
	unsigned long long area;
	area = image->width * image->length;
	for (int i = 0; i < image->width; i++)
		for (int j = 0; j < image->length; j++)
			v_freq[(int)image->matrix[i][j]]++;
	for (int i = 0; i < image->width; i++)
		for (int j = 0; j < image->length; j++) {
			unsigned long long sum = 0;
			for (int k = 0; k <= (int)image->matrix[i][j]; k++)
				sum += v_freq[k];
			sum = sum * 255;
			sum /= area;
			image->matrix[i][j] = (unsigned char)sum;
		}
	printf("Equalize done\n");
}

void rotate_90_right(image_prop *image)
{
	if (image->type == 2 || image->type == 5) {
		unsigned char **tmp_matrix;
		tmp_matrix = alloc_matrix(image->x2 - image->x1,
								  image->y2 - image->y1);
		for (int i = image->x1; i < image->x2; i++)
			for (int j = image->y1; j < image->y2; j++)
				tmp_matrix[i - image->x1][j - image->y1] =
						image->matrix[image->y2 + image->y1 - 1 - j][i];
		if (image->x1 == 0 && image->y1 == 0 && image->x2 == image->length &&
			image->y2 == image->width) {
			for (int i = 0; i < image->width; i++)
				free(image->matrix[i]);
			free(image->matrix);
			image->x1 = 0;
			image->y1 = 0;
			image->x2 = image->width;
			image->y2 = image->length;
			int aux = image->width;
			image->width = image->length;
			image->length = aux;
			image->matrix = tmp_matrix;
		} else {
			for (int i = image->y1; i < image->y2; i++)
				for (int j = image->x1; j < image->x2; j++)
					image->matrix[i][j] =
									tmp_matrix[i - image->y1][j - image->x1];
			for (int i = image->y1; i < image->y2; i++)
				free(tmp_matrix[i - image->y1]);
			free(tmp_matrix);
		}
	} else if (image->type == 3 || image->type == 6) {
		pixel **tmp_pixel_matrix = pixel_alloc_matrix(image->x2 - image->x1,
													image->y2 - image->y1);

		for (int i = image->x1; i < image->x2; i++) {
			for (int j = image->y1; j < image->y2; j++) {
				tmp_pixel_matrix[i - image->x1][j - image->y1].red =
				image->color_matrix[image->y2 + image->y1 - 1 - j][i].red;

				tmp_pixel_matrix[i - image->x1][j - image->y1].green =
				image->color_matrix[image->y2 + image->y1 - 1 - j][i].green;

				tmp_pixel_matrix[i - image->x1][j - image->y1].blue =
				image->color_matrix[image->y2 + image->y1 - 1 - j][i].blue;
			}
		}
		if (image->x1 == 0 && image->y1 == 0 && image->x2 == image->length &&
			image->y2 == image->width) {
			for (int i = 0; i < image->width; i++)
				free(image->color_matrix[i]);
			free(image->color_matrix);
			image->x1 = 0;
			image->y1 = 0;
			image->x2 = image->width;
			image->y2 = image->length;
			int aux = image->width;
			image->width = image->length;
			image->length = aux;
			image->color_matrix = tmp_pixel_matrix;
		} else {
			for (int i = image->y1; i < image->y2; i++) {
				for (int j = image->x1; j < image->x2; j++) {
					image->color_matrix[i][j].red =
						tmp_pixel_matrix[i - image->y1][j - image->x1].red;
					image->color_matrix[i][j].green =
						tmp_pixel_matrix[i - image->y1][j - image->x1].green;
					image->color_matrix[i][j].blue =
						tmp_pixel_matrix[i - image->y1][j - image->x1].blue;
				}
			}
			for (int i = image->y1; i < image->y2; i++)
				free(tmp_pixel_matrix[i - image->y1]);
			free(tmp_pixel_matrix);
		}
	}
}

void rotate_90_left(image_prop *image)
{
	if (image->type == 2 || image->type == 5) {
		unsigned char **tmp_matrix;
		tmp_matrix = alloc_matrix(image->x2 - image->x1,
								  image->y2 - image->y1);
		for (int i = image->x1; i < image->x2; i++)
			for (int j = image->y1; j < image->y2; j++)
				tmp_matrix[i - image->x1][j - image->y1] =
						image->matrix[j][image->x1 + image->x2 - 1 - i];
		if (image->x1 == 0 && image->y1 == 0 && image->x2 == image->length &&
			image->y2 == image->width) {
			for (int i = 0; i < image->width; i++)
				free(image->matrix[i]);
			free(image->matrix);
			image->x1 = 0;
			image->y1 = 0;
			image->x2 = image->width;
			image->y2 = image->length;
			int aux = image->width;
			image->width = image->length;
			image->length = aux;
			image->matrix = tmp_matrix;
		} else {
			for (int i = image->y1; i < image->y2; i++)
				for (int j = image->x1; j < image->x2; j++)
					image->matrix[i][j] =
									tmp_matrix[i - image->y1][j - image->x1];
			for (int i = image->y1; i < image->y2; i++)
				free(tmp_matrix[i - image->y1]);
			free(tmp_matrix);
		}
	} else if (image->type == 3 || image->type == 6) {
		pixel **tmp_pixel_matrix = pixel_alloc_matrix(image->x2 - image->x1,
													image->y2 - image->y1);
		for (int i = image->x1; i < image->x2; i++) {
			for (int j = image->y1; j < image->y2; j++) {
				tmp_pixel_matrix[i - image->x1][j - image->y1].red =
				image->color_matrix[j][image->x1 + image->x2 - 1 - i].red;

				tmp_pixel_matrix[i - image->x1][j - image->y1].green =
				image->color_matrix[j][image->x1 + image->x2 - 1 - i].green;

				tmp_pixel_matrix[i - image->x1][j - image->y1].blue =
				image->color_matrix[j][image->x1 + image->x2 - 1 - i].blue;
			}
		}
		if (image->x1 == 0 && image->y1 == 0 && image->x2 == image->length &&
			image->y2 == image->width) {
			for (int i = 0; i < image->width; i++)
				free(image->color_matrix[i]);
			free(image->color_matrix);
			image->x1 = 0;
			image->y1 = 0;
			image->x2 = image->width;
			image->y2 = image->length;
			int aux = image->width;
			image->width = image->length;
			image->length = aux;
			image->color_matrix = tmp_pixel_matrix;
		} else {
			for (int i = image->y1; i < image->y2; i++) {
				for (int j = image->x1; j < image->x2; j++) {
					image->color_matrix[i][j].red =
						tmp_pixel_matrix[i - image->y1][j - image->x1].red;
					image->color_matrix[i][j].green =
						tmp_pixel_matrix[i - image->y1][j - image->x1].green;
					image->color_matrix[i][j].blue =
						tmp_pixel_matrix[i - image->y1][j - image->x1].blue;
				}
			}
			for (int i = image->y1; i < image->y2; i++)
				free(tmp_pixel_matrix[i - image->y1]);
			free(tmp_pixel_matrix);
		}
	}
}

void rotate_command(image_prop *image)
{
	char space;
	int angle;
	scanf("%c", &space);
	if (space == '\n')
		return;
	scanf("%d", &angle);
	if (image->loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (image->x2 - image->x1 != image->y2 - image->y1 &&
		(image->x1 != 0 && image->x2 != image->length && image->y1 != 0 &&
		image->y2 != image->width)) {
		printf("The selection must be square\n");
		return;
	}
	if (angle > 0) {
		if (angle == 90) {
			rotate_90_right(image);
			printf("Rotated %d\n", angle);
		} else if (angle == 180) {
			rotate_90_right(image);
			rotate_90_right(image);
			printf("Rotated %d\n", angle);
		} else if (angle == 270) {
			rotate_90_right(image);
			rotate_90_right(image);
			rotate_90_right(image);
			printf("Rotated %d\n", angle);
		} else if (angle == 360) {
			printf("Rotated %d\n", angle);
		} else {
			printf("Unsupported rotation angle\n");
		}
	} else if (angle < 0) {
		if (angle == -90) {
			rotate_90_left(image);
			printf("Rotated %d\n", angle);
		} else if (angle == -180) {
			rotate_90_left(image);
			rotate_90_left(image);
			printf("Rotated %d\n", angle);
		} else if (angle == -270) {
			rotate_90_left(image);
			rotate_90_left(image);
			rotate_90_left(image);
			printf("Rotated %d\n", angle);
		} else if (angle == -360) {
			printf("Rotated %d\n", angle);
		} else {
			printf("Unsupported rotation angle\n");
		}
	} else if (angle == 0) {
		printf("Rotated %d\n", angle);
	}
}

void crop_command(image_prop *image)
{
	if (image->loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	int length = image->x2 - image->x1;
	int width = image->y2 - image->y1;

	if (image->type == 2 || image->type == 5) {
		unsigned char **tmp_matrix;
		tmp_matrix = alloc_matrix(width, length);
		for (int i = image->y1; i < image->y2; i++)
			for (int j = image->x1; j < image->x2; j++)
				tmp_matrix[i - image->y1][j - image->x1] = image->matrix[i][j];
		for (int i = 0; i < image->width; i++)
			free(image->matrix[i]);
		free(image->matrix);
		image->width = width;
		image->length = length;
		image->x1 = 0;
		image->y1 = 0;
		image->x2 = length;
		image->y2 = width;
		alloc_matrix_unsigned_char(image);
		for (int i = 0; i < width; i++)
			for (int j = 0; j < length; j++)
				image->matrix[i][j] = tmp_matrix[i][j];
		for (int i = 0; i < width; i++)
			free(tmp_matrix[i]);
		free(tmp_matrix);
	} else if (image->type == 3 || image->type == 6) {
		pixel **tmp_pixel_matrix = (pixel **)malloc(width * sizeof(pixel *));
		if (!tmp_pixel_matrix) {
			printf("Failed to allocate tmp");
			free(tmp_pixel_matrix);
		}
		for (int i = 0; i < width; i++) {
			tmp_pixel_matrix[i] = (pixel *)malloc(length * sizeof(pixel));
			if (!tmp_pixel_matrix[i]) {
				printf("Failed to allocate tmp[i]");
				free(tmp_pixel_matrix[i]);
				for (int j = i - 1; j >= 0; j--)
					free(tmp_pixel_matrix[j]);
				free(tmp_pixel_matrix);
				break;
			}
		}
		for (int i = image->y1, j = 0; i < image->y2 && j < width; i++, j++) {
			for (int k = image->x1, l = 0; k < image->x2 &&
				 l < length; k++, l++) {
				tmp_pixel_matrix[j][l].red = image->color_matrix[i][k].red;
				tmp_pixel_matrix[j][l].green = image->color_matrix[i][k].green;
				tmp_pixel_matrix[j][l].blue = image->color_matrix[i][k].blue;
			}
		}
		for (int i = 0; i < image->width; i++)
			free(image->color_matrix[i]);
		free(image->color_matrix);
		image->width = width;
		image->length = length;
		image->x1 = 0;
		image->y1 = 0;
		image->x2 = length;
		image->y2 = width;
		alloc_matrix_pixel(image);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < length; j++) {
				image->color_matrix[i][j].red = tmp_pixel_matrix[i][j].red;
				image->color_matrix[i][j].green = tmp_pixel_matrix[i][j].green;
				image->color_matrix[i][j].blue = tmp_pixel_matrix[i][j].blue;
			}
		}
		for (int i = 0; i < width; i++)
			free(tmp_pixel_matrix[i]);
		free(tmp_pixel_matrix);
	}
	printf("Image cropped\n");
}

void set_kernel(char *param, double (*kernel)[3], int *type, int *ok)
{
	if (strcmp(param, "EDGE") == 0) {
		kernel[0][0] = -1;
		kernel[0][1] = -1;
		kernel[0][2] = -1;
		kernel[1][0] = -1;
		kernel[1][1] = 8;
		kernel[1][2] = -1;
		kernel[2][0] = -1;
		kernel[2][1] = -1;
		kernel[2][2] = -1;
	} else if (strcmp(param, "SHARPEN") == 0) {
		kernel[0][0] = 0;
		kernel[0][1] = -1;
		kernel[0][2] = 0;
		kernel[1][0] = -1;
		kernel[1][1] = 5;
		kernel[1][2] = -1;
		kernel[2][0] = 0;
		kernel[2][1] = -1;
		kernel[2][2] = 0;
	} else if (strcmp(param, "BLUR") == 0) {
		kernel[0][0] = 1;
		kernel[0][1] = 1;
		kernel[0][2] = 1;
		kernel[1][0] = 1;
		kernel[1][1] = 1;
		kernel[1][2] = 1;
		kernel[2][0] = 1;
		kernel[2][1] = 1;
		kernel[2][2] = 1;
		*type = 3;
	} else if (strcmp(param, "GAUSSIAN_BLUR") == 0) {
		kernel[0][0] = 1;
		kernel[0][1] = 2;
		kernel[0][2] = 1;
		kernel[1][0] = 2;
		kernel[1][1] = 4;
		kernel[1][2] = 2;
		kernel[2][0] = 1;
		kernel[2][1] = 2;
		kernel[2][2] = 1;
		*type = 4;
	} else {
		printf("APPLY parameter invalid\n");
		free(param);
		*ok = 0;
		return;
	}
}

void apply_kernel(image_prop *image, int min_x, int max_x, int min_y,
				  int max_y, int type, pixel **tmp_pixel_matrix,
				  double kernel[][3])
{
	for (int i = min_y; i < max_y; i++) {
		for (int j = min_x; j < max_x; j++) {
			double sum = 0;
			if (border(*image, i, j))
				continue;
			sum += kernel[0][0] * (int)image->color_matrix[i - 1][j - 1].red;
			sum += kernel[0][1] * (int)image->color_matrix[i - 1][j].red;
			sum += kernel[0][2] * (int)image->color_matrix[i - 1][j + 1].red;
			sum += kernel[1][0] * (int)image->color_matrix[i][j - 1].red;
			sum += kernel[1][1] * (int)image->color_matrix[i][j].red;
			sum += kernel[1][2] * (int)image->color_matrix[i][j + 1].red;
			sum += kernel[2][0] * (int)image->color_matrix[i + 1][j - 1].red;
			sum += kernel[2][1] * (int)image->color_matrix[i + 1][j].red;
			sum += kernel[2][2] * (int)image->color_matrix[i + 1][j + 1].red;
			if (type == 3)
				sum /= 9;
			else if (type == 4)
				sum /= 16;
			if (sum + 0.5 > (int)(sum + 1))
				sum = (int)(sum + 1);
			else
				sum = (int)sum;
			if (sum < 0)
				sum = 0;
			else if (sum > 255)
				sum = 255;
			tmp_pixel_matrix[i][j].red = sum;
			sum = 0;
			sum += kernel[0][0] * (int)image->color_matrix[i - 1][j - 1].green;
			sum += kernel[0][1] * (int)image->color_matrix[i - 1][j].green;
			sum += kernel[0][2] * (int)image->color_matrix[i - 1][j + 1].green;
			sum += kernel[1][0] * (int)image->color_matrix[i][j - 1].green;
			sum += kernel[1][1] * (int)image->color_matrix[i][j].green;
			sum += kernel[1][2] * (int)image->color_matrix[i][j + 1].green;
			sum += kernel[2][0] * (int)image->color_matrix[i + 1][j - 1].green;
			sum += kernel[2][1] * (int)image->color_matrix[i + 1][j].green;
			sum += kernel[2][2] * (int)image->color_matrix[i + 1][j + 1].green;
			if (type == 3)
				sum /= 9;
			else if (type == 4)
				sum /= 16;
			if (sum + 0.5 > (int)(sum + 1))
				sum = (int)(sum + 1);
			else
				sum = (int)sum;
			if (sum < 0)
				sum = 0;
			else if (sum > 255)
				sum = 255;
			tmp_pixel_matrix[i][j].green = sum;
			sum = 0;
			sum += kernel[0][0] * (int)image->color_matrix[i - 1][j - 1].blue;
			sum += kernel[0][1] * (int)image->color_matrix[i - 1][j].blue;
			sum += kernel[0][2] * (int)image->color_matrix[i - 1][j + 1].blue;
			sum += kernel[1][0] * (int)image->color_matrix[i][j - 1].blue;
			sum += kernel[1][1] * (int)image->color_matrix[i][j].blue;
			sum += kernel[1][2] * (int)image->color_matrix[i][j + 1].blue;
			sum += kernel[2][0] * (int)image->color_matrix[i + 1][j - 1].blue;
			sum += kernel[2][1] * (int)image->color_matrix[i + 1][j].blue;
			sum += kernel[2][2] * (int)image->color_matrix[i + 1][j + 1].blue;
			if (type == 3)
				sum /= 9;
			else if (type == 4)
				sum /= 16;
			if (sum + 0.5 > (int)(sum + 1))
				sum = (int)(sum + 1);
			else
				sum = (int)sum;
			if (sum < 0)
				sum = 0;
			else if (sum > 255)
				sum = 255;
			tmp_pixel_matrix[i][j].blue = sum;
		}
	}
}

void apply_command(image_prop *image)
{
	char *param, space;
	int ok = 1;
	scanf("%c", &space);
	if (space == '\n') {
		if (image->loaded == 0) {
			printf("No image loaded\n");
			return;
		}
		printf("Invalid command\n");
		return;
	}
	param = (char *)malloc(15 * sizeof(char));
	scanf("%s", param);
	if (image->loaded == 0) {
		printf("No image loaded\n");
		free(param);
		return;
	}

	if (image->type == 2 || image->type == 5) {
		printf("Easy, Charlie Chaplin\n");
		free(param);
		return;
	}
	double kernel[3][3];
	int type = 0;
	set_kernel(param, kernel, &type, &ok);
	if (ok == 0)
		return;
	pixel **tmp_pixel_matrix = pixel_alloc_matrix(image->width, image->length);
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			tmp_pixel_matrix[i][j].red = image->color_matrix[i][j].red;
			tmp_pixel_matrix[i][j].green = image->color_matrix[i][j].green;
			tmp_pixel_matrix[i][j].blue = image->color_matrix[i][j].blue;
		}
	}
	int min_x = image->x1 > image->x2 ? image->x2 : image->x1;
	int max_x = image->x1 > image->x2 ? image->x1 : image->x2;
	int min_y = image->y1 > image->y2 ? image->y2 : image->y1;
	int max_y = image->y1 > image->y2 ? image->y1 : image->y2;
	if (min_x == 0)
		min_x += 1;
	if (max_x == image->length)
		max_x -= 1;
	if (min_y == 0)
		min_y += 1;
	if (max_y == image->width)
		max_y -= 1;

	//apply_kernel(kernel,image,)
	apply_kernel(image, min_x, max_x, min_y, max_y, type, tmp_pixel_matrix,
				 kernel);
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			image->color_matrix[i][j].red = tmp_pixel_matrix[i][j].red;
			image->color_matrix[i][j].green = tmp_pixel_matrix[i][j].green;
			image->color_matrix[i][j].blue = tmp_pixel_matrix[i][j].blue;
		}
	}
	for (int i = 0; i < image->width; i++) {
		for (int j = 0; j < image->length; j++) {
			image->color_matrix[i][j].red = tmp_pixel_matrix[i][j].red;
			image->color_matrix[i][j].green = tmp_pixel_matrix[i][j].green;
			image->color_matrix[i][j].blue = tmp_pixel_matrix[i][j].blue;
		}
	}
	printf("APPLY %s done\n", param);
	free(param);
	for (int i = 0; i < image->width; i++)
		free(tmp_pixel_matrix[i]);
	free(tmp_pixel_matrix);
}

void save_command(image_prop *image)
{
	char *file_name, space;
	scanf("%c", &space);
	file_name = (char *)malloc(MAX_FILE_NAME * sizeof(char));
	fgets(file_name, MAX_FILE_NAME, stdin);
	if (image->loaded == 0) {
		printf("No image loaded\n");
		free(file_name);
		return;
	}
	file_name[strlen(file_name)] = '\0';
	char *p = strtok(file_name, " ");
	p = strtok(NULL, " ");
	if (p) {
		if (image->type > 3)
			image->type -= 3;
		else if (image->type < 4)
			image->type += 3;
	} else {
		file_name[strlen(file_name) - 1] = '\0';
	}
	FILE *save = fopen(file_name, "w");
	fprintf(save, "P%d\n", image->type);
	fprintf(save, "%d %d\n", image->length, image->width);
	if (image->type < 4) {
		if (image->type != 1)
			fprintf(save, "%d\n", image->max_color);
		for (int i = 0; i < image->width; i++) {
			int ok = 0;
			for (int j = 0; j < image->length; j++) {
				if (image->type != 3) {
					if (!ok) {
						fprintf(save, "%d", (int)(image->matrix[i][j]));
						ok = 1;
					} else {
						fprintf(save, " %d", (int)(image->matrix[i][j]));
					}
				} else {
					if (!ok) {
						fprintf(save, "%d", (int)
							   (image->color_matrix[i][j].red));
						fprintf(save, " %d", (int)
							   (image->color_matrix[i][j].green));
						fprintf(save, " %d", (int)
							   (image->color_matrix[i][j].blue));
						ok = 1;
					} else {
						fprintf(save, " %d", (int)
								(image->color_matrix[i][j].red));
						fprintf(save, " %d", (int)
								(image->color_matrix[i][j].green));
						fprintf(save, " %d", (int)
								(image->color_matrix[i][j].blue));
					}
				}
			}
			fprintf(save, "\n");
		}
	} else {
		if (image->type != 4)
			fprintf(save, "%d\n", image->max_color);
		for (int i = 0; i < image->width; i++) {
			for (int j = 0; j < image->length; j++) {
				if (image->type == 6) {
					fwrite(&image->color_matrix[i][j].red,
						   sizeof(unsigned char), 1, save);
					fwrite(&image->color_matrix[i][j].green,
						   sizeof(unsigned char), 1, save);
					fwrite(&image->color_matrix[i][j].blue,
						   sizeof(unsigned char), 1, save);
				} else if (image->type != 6) {
					fwrite(&image->matrix[i][j], sizeof(unsigned char), 1,
						   save);
				}
			}
		}
	}
	fclose(save);
	printf("Saved %s\n", file_name);
	free(file_name);
}

void exit_command(image_prop *image)
{
	if (image->loaded == 1) {
		if (image->type == 3 || image->type == 6) {
			for (int i = 0; i < image->width; i++)
				free(image->color_matrix[i]);
			free(image->color_matrix);
		} else {
			for (int i = 0; i < image->width; i++)
				free(image->matrix[i]);
			free(image->matrix);
		}
		free(image->file_name);
	} else if (image->loaded == 0) {
		printf("No image loaded\n");
	}
	exit(0);
}

int main(void)
{
	char *command;
	command = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	char end_of_line;
	image_prop image;
	image.loaded = 0;
	while (I_AM_HANDSOME) {
		scanf("%s", command);
		switch (which_command(command)) {
		case -LOAD:

			load_command(&image);
			break;

		case -SELECT:
			select_command(&image);
			break;

		case -HISTOGRAM:
			histogram_command(&image);
			break;

		case -EQUALIZE:
			equalize_command(&image);
			break;

		case -ROTATE:
			rotate_command(&image);
			break;

		case -CROP:
			crop_command(&image);
			break;

		case -APPLY:
			apply_command(&image);
			break;

		case -SAVE:
			save_command(&image);
			break;

		case -EXIT:
			free(command);
			exit_command(&image);
			break;

		default:
			scanf("%c", &end_of_line);
			while (end_of_line != '\n') {
				scanf("%s", command);
				scanf("%c", &end_of_line);
			}
			printf("Invalid command\n");
		}
	}
	return 0;
}
