#ifndef UTILS_H_
#define UTILS_H_

#define SPREAD 101

// Calcula uma lista de notas aleatória.
unsigned int* valoresAleatorios(unsigned int size);

// Acha a mediana baseado em um vetor de frequências e seu tamanho.
unsigned int mediana(unsigned int freqs[SPREAD], unsigned long tam);

// Acha o desvio padrão baseado no vetor de frequências, seu tamanho e a média previamente calculada.
double dpParalelo(unsigned int freqs[SPREAD], unsigned long tam, double media);

#endif // UTILS_H_
