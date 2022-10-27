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
static inline float mediana(unsigned int freqs[SPREAD], unsigned long tam) {
    unsigned long med = tam/2;
    unsigned int pos = 0;
    float r = 0;
    for (int i = 0; i < SPREAD; ++i) {
        if ((pos + freqs[i]) > med) {
            if (tam % 2 || (med - pos) < freqs[i]) {
                r = i;
            } else {
                r += i/2.f;
                for (int j = i + 1; j < SPREAD; ++j) {
                    if (!freqs[j]) continue;
                    r += j/2.f;
                    break;
                }
            }
            break;
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
