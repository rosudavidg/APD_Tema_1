#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	unsigned char value[3];
} pixel;

// Structura unei imagini PGM sau PNM
typedef struct {
	unsigned char file_type; // 5 pentru PGM sau 6 pentru PNM
	int width;     // latimea imaginii
	int height;    // inaltimea imaginii
	unsigned char max_value; // valoare maxima a unui pixel
	unsigned char num_colors; // numarul de culori dintr-un pixel

	pixel** pixels;  // matrice de pixeli, fiecare cu cate num_colors culori
} image;

// Structura unei date pe care o primeste un thread
typedef struct {
	int id;     // id-ul thread-ului
	image* in;  // referinta catre imaginea initiala
	image* out; // referinta catre imaginea finala
} thread_arg;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */