#ifndef UTILS_H_
#define UTILS_H_

#define SPREAD 101

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <limits.h>

// Calcula uma lista de notas aleatória.
unsigned int* valoresAleatorios(unsigned int size);

// Acha a mediana baseado em um vetor de frequências e seu tamanho.
static inline unsigned int mediana(unsigned int freqs[SPREAD], unsigned long tam) {
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

// Acha o desvio padrão baseado no vetor de frequências, seu tamanho e a média previamente calculada.
static inline double dpParalelo(unsigned int freqs[SPREAD], unsigned long tam, double media) {
    double sum2 = 0;
    #pragma omp simd
    for (int i = 0; i < SPREAD; ++i) {
        if (!freqs[i]) continue;
        sum2 += freqs[i] * pow(i - media, 2.f)/(1.f*(tam ?: 1));
    }
    return sqrt(sum2);
}

#endif // UTILS_H_
