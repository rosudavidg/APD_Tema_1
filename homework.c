#include "homework.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int num_threads;
int resize_factor;
int GaussianKernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
int GaussianSum = 16;

// Primeste un file pointer si intoarce
// primul numar gasit pana la un spatiu alb,
// citindu-l si pe acela
int read_next_int(FILE** file) {
	char c;
	int number = 0;

	fread(&c, sizeof(char), 1, *file);

	while (c >= '0' && c <= '9') {
		number = number * 10 + c - 48;
		fread(&c, sizeof(char), 1, *file);
	}

	return number;
}

int apply_gaussian(int array[3][3]) {
	int sum = 0;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			sum += array[i][j] * GaussianKernel[i][j];
		}
	}
	return sum / GaussianSum;
}

// Afla numarul de culori in functie
// de formatul imaginii
void set_num_colors(image* img) {
	switch ((*img).file_type) {
		case 5:
			(*img).num_colors = 1;
			break;

		case 6:
			(*img).num_colors = 3;
			break;
	}
}

// Citeste o imagine de tip P5 sau P6
void readInput(const char* file_name, image* img) {

	// Deschidere fisier binar
	FILE* file = fopen(file_name, "rb");

	// Skip litera P
	fseek(file, 1, SEEK_SET);

	(*img).file_type =  read_next_int(&file);
	(*img).width     =  read_next_int(&file);
	(*img).height    =  read_next_int(&file);
	(*img).max_value =  read_next_int(&file);

	set_num_colors(img);

	int width      = (*img).width;
	int height     = (*img).height;
	int num_colors = (*img).num_colors;

	(*img).pixels = (pixel**) malloc(sizeof(pixel*) * height);
	int i, j;

	// Citire pixel cu pixel
	for (i = 0; i < height; ++i) {
		(*img).pixels[i] = (pixel*) malloc(sizeof(pixel) * width);
		
		if (num_colors == 3) {
			fread(&(*img).pixels[i][0], width * num_colors, 1, file);
		} else {
			for (j = 0; j < width; ++j) {
				fread(&(*img).pixels[i][j].value, num_colors, 1, file);
			}
		}
	}
	fclose(file);
}

// Primeste un numar si scrie numarul
// intr-un fisier binar, cifra cu cifra
void write_int(FILE** file, int number) {
	
	int final_zero = 0;

	// Reverse number
	int aux = 0;
	while (number != 0) {
		aux = aux * 10 + number % 10;
		number /= 10;

		if (aux == 0) {
			final_zero++;
		}
	}
	number = aux;

	// Scriere cifra cu cifra in fisier
	while (number != 0) {
		unsigned char digit = number % 10 + 48;
		number /= 10;

		fwrite(&digit, 1, sizeof(char), *file);
	}

	while (final_zero != 0) {
		fwrite("0", 1, sizeof(char), *file);
		final_zero--;
	}
}

// Scrie intr-un fisier binar o imagine
// de tip P5 sau P6
void writeData(const char* file_name, image* img) {
	FILE* file = fopen(file_name, "wb");

	fwrite("P", 1, sizeof(char), file);  // Litera P
	write_int(&file, (*img).file_type);  // Tip 5 sau 6
	fwrite("\n", 1, sizeof(char), file); // new line
	
	write_int(&file, (*img).width);      // width
	fwrite(" ", 1, sizeof(char), file);  // space
	write_int(&file, (*img).height);     // height
	fwrite("\n", 1, sizeof(char), file); // new line

	write_int(&file, (*img).max_value);  // max_value
	fwrite("\n", 1, sizeof(char), file); // new line

	int height     = (*img).height;
	int width      = (*img).width;
	int num_colors = (*img).num_colors;

	// Scriere pixel cu pixel
	for (int i = 0; i < height; ++i) {
		if (num_colors == 3) {
			fwrite(&(*img).pixels[i][0], 1, width * num_colors, file);
		} else {
			for (int j = 0; j < width; ++j) {
				fwrite(&(*img).pixels[i][j], 1, num_colors, file);
			}
		}
	}

	fclose(file);
}

// Functie resize pentru resize_factor par
void* resize_even_thread(void *var) {

	thread_arg t_arg = *(thread_arg*) var;
	int id     = t_arg.id;
	image* in  = t_arg.in;
	image* out = t_arg.out;

	int num_colors = (*out).num_colors;
	int width      = (*out).width;
	int height     = (*out).height;

	int start =       id * height / num_threads;
	int stop  = (id + 1) * height / num_threads;
	int divide_factor = resize_factor * resize_factor;
	
	if (id == num_threads) {
		stop = height;
	}

	for (int i = start; i < stop; i++) {
		(*out).pixels[i] = (pixel*) malloc(sizeof(pixel) * width);
	}

	// h - linia noului pixel
	// w - coloana noului pixel
	int new_pixel;
	for (int k = 0; k < num_colors; ++k) {
		for (int h = start; h < stop; ++h) {
			int stop1 = (h + 1) * resize_factor;
			
			for (int w = 0; w < width; ++w) {
				new_pixel = 0;

				int stop2 = (w + 1) * resize_factor;

				for (int i = h * resize_factor; i < stop1; ++i) {
					for (int j = w * resize_factor; j < stop2; ++j) {
						new_pixel += (*in).pixels[i][j].value[k];
					}
				}

				(*out).pixels[h][w].value[k] = new_pixel / divide_factor;
			}
		}
	}

	return NULL;
}

// Functie resize pentru resize_factor egal cu 3
void* resize_3_thread(void *var)
{
	thread_arg t_arg = *(thread_arg*) var;
	int id     = t_arg.id;
	image* in  = t_arg.in;
	image* out = t_arg.out;

	int num_colors = (*out).num_colors;
	int width      = (*out).width;
	int height     = (*out).height;

	int start =       id * height / num_threads;
	int stop  = (id + 1) * height / num_threads;

	int array[3][3];

	if (id == num_threads) {
		stop = height;
	}

	for (int i = start; i < stop; i++) {
		(*out).pixels[i] = (pixel*) malloc(sizeof(pixel) * width);
	}

	// h - linia noului pixel
	// w - coloana noului pixel
	for (int h = start; h < stop; h++) {
		for (int w = 0; w < width; w++) {

			for (int k = 0; k < num_colors; k++) {
				for (int i = h * 3; i < (h + 1) * 3; i++) {
					for (int j = w * 3; j < (w + 1) * 3; j++) {
						array[i - h * 3][j - w * 3] = (*in).pixels[i][j].value[k];
					}
				}

				(*out).pixels[h][w].value[k] = apply_gaussian(array);
			}
		}
	}

	return NULL;
}

void resize(image* in, image* out) {
	// Copiere header
	(*out).file_type  = (*in).file_type;
	(*out).width      = (*in).width  / resize_factor;
	(*out).height     = (*in).height / resize_factor;
	(*out).max_value  = (*in).max_value;
	(*out).num_colors = (*in).num_colors;

	// Alocare memorie pentru out
	(*out).pixels = (pixel**) malloc(sizeof(pixel*) * (*out).height);

	// Creare thread-uri dupa scheletul de laborator
	pthread_t tid[num_threads];
	thread_arg t_args[num_threads];

	for(int i = 0; i < num_threads; i++) {
		t_args[i].id  = i;
		t_args[i].in  = in;
		t_args[i].out = out;
	}

	// Trimit threadurile in executie in functie de resize_factor
	if (resize_factor % 2 == 0) {
		for(int i = 0; i < num_threads; i++) {
			pthread_create(&(tid[i]), NULL, resize_even_thread, &(t_args[i]));
		}
	} else if (resize_factor == 3) {
		for(int i = 0; i < num_threads; i++) {
			pthread_create(&(tid[i]), NULL, resize_3_thread, &(t_args[i]));
		}
	}

	// Asteptare thread-uri
	for(int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}