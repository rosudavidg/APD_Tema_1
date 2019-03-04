#include "homework1.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

int num_threads;
int resolution;

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

// Alocare memorie
void initialize(image *im) {
	(*im).pixels = (unsigned char**) malloc(sizeof(unsigned char*) * resolution);

	for (int i = 0; i < resolution; i++) {
		(*im).pixels[i] = (unsigned char*) malloc(sizeof(unsigned char) * resolution);
	}
}

// Calculare distanta
double distance(double x, double y) {
	return abs(2 * y - x) / (double)sqrt(5);
}

void* threadFunction(void *var)
{
	thread_arg t_arg = *(thread_arg*)var;
	image* im = t_arg.im;

	int start =      t_arg.id  * resolution / num_threads;
	int stop  = (1 + t_arg.id) * resolution / num_threads;

	if (t_arg.id == num_threads) {
		stop = resolution;
	}

	for (int i = start; i < stop; i++) {
		for (int j = 0; j < resolution; j++) {

			double x = ((double) 100 * (j + 0.5) / resolution);
			double y = ((double) 100 * (i + 0.5) / resolution);

			if (distance(x, y) <= 3) {
				(*im).pixels[resolution - i - 1][j] = 0;
			} else {
				(*im).pixels[resolution - i - 1][j] = 255;
			}
		}
	}
}

void render(image *im) {

	pthread_t tid[num_threads];
	thread_arg t_args[num_threads];
	
	for(int i = 0; i < num_threads; i++) {
		t_args[i].id = i;
		t_args[i].im = im;
	}

	for(int i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(t_args[i]));
	}

	for(int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}

void writeData(const char * file_name, image *img) {
	FILE* file = fopen(file_name, "wb");

	fwrite("P5\n", 3, sizeof(char), file);  // P5 + newline
	
	write_int(&file, resolution);           // width
	fwrite(" ", 1, sizeof(char), file);     // space
	write_int(&file, resolution);           // height
	fwrite("\n", 1, sizeof(char), file);    // new line

	fwrite("255\n", 4, sizeof(char), file); // max value + new line

	// Scriere pixel cu pixel
	for (int i = 0; i < resolution; i++)
		fwrite(*&(*img).pixels[i], 1, resolution, file);

	fclose(file);
}

