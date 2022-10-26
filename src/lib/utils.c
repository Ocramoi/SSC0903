#include "utils.h"

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <limits.h>

// Gera lista de valores aleatórios com tamanho dado
unsigned int* valoresAleatorios(unsigned int size) {
    unsigned int* notas = malloc(size * sizeof(typeof(size)));
    for (unsigned int i = 0; i < size; i++)
        notas[i] = (int) round(rand() % SPREAD);
    return notas;
}
