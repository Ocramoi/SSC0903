#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 10

int* pesos_aleatorios(int N){
    srand(0);
    int tam = N*N;
    int* pesos = malloc(tam*sizeof(int));
    for(int i = 0; i < tam; i++){
        if(i/N != i%N) pesos[i] = rand() % MAX;
        else pesos[i] = 0;
    }
    return pesos;
}

int fatorial(int n){
    int prod = 1;
    for(int i = 2; i <= n; i++){
       prod *= i;
    }
    return prod;
}

int n_vazio(int * vet, int n){
    int i = 0;
    while(n > 0){
        i++;
        n -= (1 - vet[i]);
    }
    return i;
}


int dist_cam(int num, int N, int* ja_foram, int* pesos){
    int soma = 0;
    int ultimo = 0;
    memset(ja_foram, 0, N * sizeof(int));
    ja_foram[0] = 1;
    for (int i = N-1; i > 0; i--){
        int atual = n_vazio(ja_foram,num % i + 1);
        ja_foram[atual] = 1;
        soma += pesos[N*ultimo + atual];
        ultimo = atual;
        num /= i;
    }
    return soma + pesos[N*ultimo + 0];
}




int main(int argc, char* argv[]){
    int N = atoi(argv[1]);
    int* pesos = pesos_aleatorios(N);
    int* ja_foram = malloc(N * sizeof(int));

    int menor_dist = N*MAX;
    int menor_caminho = 0;

    int caminhos = fatorial(N-1);
    for(int i = 0; i < caminhos; i++){
        int dist = dist_cam(i,N,ja_foram,pesos);
        if(dist < menor_dist){
            menor_dist = dist;
            menor_caminho = i;
        }
    }

    memset(ja_foram, 0, N * sizeof(int));
    ja_foram[0] = 1;
    printf("0");
    int num = menor_caminho;
    for(int i = N-1; i > 0;i--){
        int atual = n_vazio(ja_foram,num % i + 1);
        ja_foram[atual] = 1;
        printf("-%d",atual);  
        num /= i;
    }

    printf("-0\n%d\n",menor_dist);

    free(ja_foram);
    free(pesos);
}
