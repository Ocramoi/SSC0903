#ifndef UTILS_H_
#define UTILS_H_

#define SPREAD 101

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <limits.h>
#include <time.h>

// Calcula uma lista de notas aleatória.
unsigned int* valoresAleatorios(unsigned int size);

// Acha a mediana baseado em um vetor de frequências e seu tamanho.
static inline float mediana(unsigned int freqs[SPREAD], unsigned long tam) {
    unsigned long med = (tam/2) - 1;
    unsigned int pos = 0;
    float r = 0;
    unsigned short rep = 0;
    for (int i = 0; i < SPREAD; ++i) {
        if ((pos + freqs[i]) > med) {
            if (tam % 2) {
                r = i;
                break;
            } else {
                rep++;
                if (!rep) i--;
                r += i/2.f;
                pos = med;
                if (rep == 2) break;
            }
        } else
            pos += freqs[i];
    }
    return r;
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
