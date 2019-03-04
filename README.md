```
=========================================================================================
Nume:    Rosu
Prenume: Gabriel - David
Grupa:   331CC
Data:    28.10.2018
Materie: APD, anul 3, semestrul 1
=========================================================================================
                                         Tema 1
                             Antialiasing si micro renderer
=========================================================================================
    CUPRINS
-----------------------------------------------------------------------------------------
    [1] INTRODUCERE
    [2] PARTEA 1
    [2.1] Structura si functii
    [2.2] Citirea
    [2.3] Scrierea
    [2.4] Algoritmul si paralelizarea
    [3] PARTEA 2
    [3.1] Structura si functii
    [3.2] Algoritmul si paralelizarea
    [6] FEEDBACK
=========================================================================================
[1] INTRODUCERE
-----------------------------------------------------------------------------------------
    Prima parte a temei reprezinta implementarea unui program care redimensioneaza o
imagine, in formatul PGM sau PNM.
    A doua parte este un micro renderer care traseaza o linie intr-o poza, dupa o anumita
formula si specificatii.
=========================================================================================
[2] PARETEA 1
=========================================================================================
[2.1] Structura si functii
-----------------------------------------------------------------------------------------
    Structura pe care am ales-o pentru o imagine contine:
    - o valoare unsiged char pentru tipul fisierului: 5 pentru PGM si 6 pentru PNM
    - o valoare int pentru latimea imaginii
    - o valoare int pentru inaltimea imaginii
    - o valoare unsigned char pentru valoarea maxima a unei culori (pixel)
    - o valoare unsigned char pentru valoarea numarului de culori (1 sau 3)
    - o referinta catre un vector 2D de tipul pixel, de dimensiuni inaltime x latime

    Structura unui pixel este un vector 1D cu 3 elemente. Daca pixelul contine o
singura culoare, atunci celelalte 2 campuri sunt ignorate. Daca pixelul contine
3 culori (RGB), atunci toate cele 3 campuri sunt utile. (0 - R, 1 - G, 2 - B)

    Pe langa aceste doua structuri, am mai folosit thread_arg, utila la trimiterea de
argumente unui thread. Aceasta contine doua adrese de memorie catre doua imagini, una
pentru input si una pentru output, dar si o valoare int pentru a retine id-ul
thread-ului.

    [*] int read_next_int(FILE**)
        Primeste adresa unui flux de citire a datelor.
        Functia citeste primul numar, pana la inalnirea unui caracter care nu este
    cifra.
        Intoarce numarul citit in valoare int.

    [*] int apply_gaussian(int[3][3])
        Primeste o matrice de dimensiune 3x3.
        Foloseste GaussianKernel si calculeaza media pixelilor.
        Intoarce media in format int.

    [*] set_num_colors(image*)
        Primeste o adresa de memorie unde este salvata o imagine.
        Functia verifica valoarea tipului fisierului (5 sau 6) si in functie de
    acesta, seteaza numarul de culori pe 1 sau 3.

    [*] void readInput(const char*, image*)
        Primeste numele unui fisier si o zona de memorie pentru o imagine.
        Functia citeste din fisier imaginea si o stocheaza in acea zona de memorie.

    [*] void writeData(const char*, image*)
        Primeste numele unui fisier si o zona de memorie pentru o imagine;
        Functia scrie in fisier imaginea.

    [*] void write_int(FILE**, int)
        Primeste adresa unui flux de scriere a datelor si un numar de tip int.
        Functia scrie acel numar, caracter cu caracter in fisierul de iesire.

    [*] void* resize_even_thread(void*)
        Reprezinta metoda accesata de un fir de executie pentru cazul cand
    resize_factor este par.

    [*] void* resize_3_thread(void*)
        Reprezinta metoda accesata de un fir de executie pentru cazul cand
    resize_factor este egal cu 3.

    [*] void resize(image*, image*)
        Primeste doua imagini (input si output).
        Aplica algoritmul de resize folosind thread-uri.
=========================================================================================
[2.2] Citirea
-----------------------------------------------------------------------------------------
    Pentru a citi poza folosesc un stream de tip read binary.
    Primul pas este sa sar peste litera P.
    Citesc in ordine tipul fisierului, latimea, inaltimea si valoarea maxima, folosind
functia read_next_int.
    Setez numarul de culori folosind set_num_colors.

    Aloc memorie pentru imagine si o citesc.
    Daca numarul de culori este 3, atunci pot citi linie cu linie poza, altfel, citesc
fiecare pixel.
=========================================================================================
[2.3] Scrierea
-----------------------------------------------------------------------------------------
    Scriu header-ul specific formatului PGM sau PNM, apoi scriu intreaga poza. Daca un
pixel are 3 culori, atunci pot afisa linie cu linie, altfel pixel cu pixel.
=========================================================================================
[2.4] Algoritmul si paralelizarea
-----------------------------------------------------------------------------------------
    In functia resize, aloc memorie pentru noua imaginie, copiez headerul si il modific
acolo unde este cazul (lattime, lungime). In functie de resize_factor, aleg functia pe
care cele n thread-uri o vor folosi (resize_even_thread pentru par si resize_3_thread
pentru impar). 
    
    Paralelizarea (pentru ambele functii) se face dupa linie.
    Astfel, fiecare thread calculeaza valoarea mai multor linii de pixeli din noua poza.
    Primul thread calculeaza primele nr_linii / nr_threaduri linii de pixeli, al doilea
urmatoarele etc, iar ultimul thread va calcula toate liniile care raman (in caz ca
numarul liniilor nu se imparte exact la numarul de threaduri).
    Astfel, aceasta implementare a temei scaleaza.
=========================================================================================
[3] PARTEA 2
=========================================================================================
[3.1] Structura si functii
-----------------------------------------------------------------------------------------
    Am utilizat structura image prezentata in homework1.h, care contine o matrice 2D
de dimensiune latime * inaltime cu elemente de tip unsigned char.
    Pe langa aceasta structura, am folosit thread_arg care reprezinta datele pe care
le primeste un fir de executie la lansare. Aceasta contine un id de tip int si o
adresa de memorie care pointeaza catre imaginea finala.

    [*] void write_int(FILE**, int)
        Aceasta functie scrie in fisier numarul primit ca parametru caracter cu
    caracter.

    [*] void initializa(image*)
        Aceasta functie aloca memorie pentru imagine.

    [*] double distance(double, double)
        Functia primeste ca argument un punct in coordonate xoy si returneaza distanta
    de la acesta pana la dreapta de ecuatie d: -1 * x + 2 * y + 0 = 0.

    [*] void render(image*)
        Functia creeaza threadurile si le apeleaza.

    [*] writeData(cons char*, image*)
        Functia afiseaza la iesire imaginea.
=========================================================================================
[3.2] Algoritmul si paralelizarea
-----------------------------------------------------------------------------------------
    Similar cu PARTEA 1, fiecare thread se va ocupa de un numar de linii:
inaltime / nr_threads. Daca aceasta impartire nu este exacta, ultimul thread
va calcula valorile pentru liniile ramase pana la final.
    Astfel, un numar mai mare de threaduri utilizate, creste numarul de linii calculate
simultan si scade timpul de executie al programului, de unde reiese scalabilitatea.

    Concret, un thread se ocupa de fiecare linie de la start pana la stop.
    Pentru fiecare pixel din acestea, se muta la jumatatea pixelului, face transformarea
in cm, iar apoi verifica daca distanta pana la dreapta este sau nu mai mica decat
grosimea de 3. Daca da, atunci pixelul va primi culoarea negru (0), altfel alb (255).
=========================================================================================
[6] FEEDBACK
-----------------------------------------------------------------------------------------
[+]     Tema isi indeplineste cu succes scopul, acela de a pune in practica cunostintele
acumulate de-alungul cursurilor, laboratoarelor, dar si studiului individual.
=========================================================================================
                                        SFARSIT
=========================================================================================
```