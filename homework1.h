#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
	unsigned char** pixels;          // matrice de pixeli
} image;

// Structura unei date pe care o primeste un thread
typedef struct {
	int id;     // id-ul thread-ului
	image* im;  // referinta catre imaginea
} thread_arg;

void initialize(image* im);
void render(image* im);
void writeData(const char* fileName, image* img);

#endif /* HOMEWORK_H1 */