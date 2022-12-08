#include<stdio.h>
#include<stdlib.h>

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

int n_vazio(int * vet, int n){
    int i = 0;
    while(n > 0){
        i++;
        n -= (1 - vet[i]);
    }
    return i;
}

int dist_cam(int primeiro, int tam, int* ja_foram, int* pesos, int* caminho, int N){
    if(tam == 0){
        return pesos[N*primeiro + 0];
    }
    int menor_dist = N*MAX;

    int* cam_filho = malloc(tam * sizeof(int));

    for (int i = 0; i < tam; i++){
        int atual = n_vazio(ja_foram,i+1);
        ja_foram[atual] = 1;
        int dist =  pesos[N*primeiro + atual] + dist_cam(atual, tam-1,ja_foram,pesos,cam_filho,N);
        ja_foram[atual] = 0;
        if (dist < menor_dist) {
            menor_dist = dist;
            for(int i =0; i < tam-1;i++) caminho[i] = cam_filho[i];
            caminho[tam-1] = atual;
        }
    }
    free(cam_filho);

    /*
    for (int i = 0; i < N - tam; i++) printf("..");
    printf("%2d : %d",menor_dist,primeiro);
    for (int i = tam-1; i >= 0; i--) printf("-%d", caminho[i]);
    printf("\n");
    */

    return menor_dist;
}

int main(){
    int N = 12;
    int* ja_foram = calloc(N,sizeof(int));
    int* caminho = malloc(N * sizeof(int));
    int* pesos = pesos_aleatorios(N);

    /*
    for(int i = 0; i < N*N; i++){
        printf("%d ",pesos[i]);
        if(i % N == N-1) printf("\n");       
    }
    */

    ja_foram[0] == 1;
    caminho[N-1] = 0;
    int menor_dist = dist_cam(0,N-1,ja_foram,pesos,caminho,N);
    printf("%d\n",menor_dist);
    for(int i =N-1; i >= 0;i--) printf("%d ",caminho[i]);

    free(ja_foram);
    free(pesos);
}
