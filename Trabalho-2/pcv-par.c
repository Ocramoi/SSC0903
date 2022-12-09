#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 10

int* pesos_aleatorios(int N, int* pesos){
    srand(0);
    int tam = N*N;
    for(int i = 0; i < tam; i++){
        if(i/N != i%N) pesos[i] = rand() % MAX;
        else pesos[i] = 0;
    }
    return pesos;
}

int min(int a, int b){
    return a < b ? a : b;    
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

void imprime_caminho(int menor_dist,int menor_caminho, int N,int *ja_foram){
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

    MPI_Init(&argc,&argv);

    int npes;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = atoi(argv[1]);
    int* ja_foram = malloc(N * sizeof(int));

    int* pesos = malloc(N*N*sizeof(int));
    if(rank == 0){
        pesos_aleatorios(N,pesos);
    }
    MPI_Bcast(pesos, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    int menor_dist = N*MAX;
    int menor_caminho = 0;

    int caminhos = fatorial(N-1);
    int tam_parte = caminhos/npes;
    int comeco = rank*tam_parte;
    int fim = min((rank+1)*tam_parte,caminhos);

    for(int i = comeco; i < fim; i++){
        int dist = dist_cam(i,N,ja_foram,pesos);
        if(dist < menor_dist){
            menor_dist = dist;
            menor_caminho = i;
        }
    }

    if(rank == 0){
        int buff[2];

        for(int i = 1; i < npes; i++){
            MPI_Status status;
            MPI_Recv(buff,2,MPI_INT,i,0,MPI_COMM_WORLD,&status);
            if(buff[0] < menor_dist){
                menor_dist = buff[0];
                menor_caminho = buff[1];
            }
        }

        imprime_caminho(menor_dist,menor_caminho,N,ja_foram);
    }
    else{
        int buff[2] = {menor_dist,menor_caminho};
        MPI_Send(buff,2,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    fflush(0);
    MPI_Finalize();

    free(ja_foram);
    free(pesos);
}
