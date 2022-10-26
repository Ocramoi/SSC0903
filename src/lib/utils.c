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

// Busca mediana dos valores com base no vetor de frequências
unsigned int mediana(unsigned int freqs[SPREAD], unsigned long tam) {
    unsigned long med = tam/2;
    unsigned int pos = 0;
    for (int i = 0; i < SPREAD; ++i) {
        if ((pos + freqs[i]) > med)
            return i;
        else
            pos += freqs[i];
    }
    return UINT_MAX;
}

// Busca mediana dos valores com base no vetor de frequências de forma paralelizada
double dpParalelo(unsigned int freqs[SPREAD], unsigned long tam, double media) {
    double sum2 = 0;
    #pragma omp simd
    for (int i = 0; i < SPREAD; ++i) {
        if (!freqs[i]) continue;
        sum2 += (freqs[i] * i * i)/(1.f*(tam - 1 ?: 1));
    }
    return sqrt(sum2 - (tam*media*media)/(1.f*(tam - 1 ?: 1)));
}
