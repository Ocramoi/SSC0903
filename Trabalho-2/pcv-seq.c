/*
 * Universidade de São Paulo - ICMC - BCC
 * SSC0903 - Computação de Alto Desempenho (2022/2)
 * Segundo Trabalho Prático (TB2) - Código Paralelo.
 * Turma: B
 * Grupo 8
 * Integrantes:
 * - Francisco de Freitas Pedrosa, RA: 11215699
 * - Luca Gomes Urssi, RA: 10425396
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
 * Foi usado o seguinte método para associar cada caminho possível a um número:
 *
 * Todo caminho começa com 0, depois precisam ser feitas N-1 escolhas.
 * A escolha 1 é sobre a cidade na próxima posição. Ela tem n-1 opções, porque não pode escolher 0.
 * A escolha 2 é sobre a cidade na próxima posição. Ela tem n-2 opções, porque não pode escolher 0, nem a cidade na posição anteiror.
 * A escolha N-1 só tem 1 opção porque só uma cidade vai ter sobrado.
 *
 * Podemos juntar todas as escolhas da seguinte forma.
 * N_Caminho = escolha_1 + (N-1)*escolha_2 + (N-1)*(N-2)**escolha_3 + ... + (N-1)!*escolha_n-1
 * Dessa forma podemos obter a escolha 0 com
 * escolha_0 = N_Caminho % (N-1)
 * E o resto do caminho tem valor:
 * resto = N_Caminho / (N-1)
 *
 * A proxima escolha vai ser a escolha_0 desse novo caminho. Que vai ter tamanho N-1.
*/

//Custo máximo de uma aresta (Elas vão de 1 a Max)
#define MAX 10


//Enche as arestas de custos aleatorios
//O custo estre uma aresta e ela mesma é 0. 
int* custos_aleatorios(int N){
    srand(0);
    int tam = N*N;
    int* custos = malloc(tam*sizeof(int));
    for(int i = 0; i < tam; i++){
        if(i/N != i%N) custos[i] = rand() % MAX;
        else custos[i] = 0;
    }
    return custos;
}

//Acha o enésimo, valor nulo em um vetor. (ignora a posição 0)
int n_vazio(int * vet, int n){
    int i = 0;
    while(n > 0){
        i++;
        n -= (1 - vet[i]);
    }
    return i;
}

//Dado o número do caminho e a distância, imprime o valor e os nós visitados.
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

//Acha a distância de um caminho recursivamente.
//Para cada uma das tam opções iniciais, calcula a distância da lista de tamanho N-1.
int dist_cam(int primeiro, int tam, int* ja_foram, int* custos, int* caminho, int N){

    //Caso base se o tamanho for zero a distância é distância de primeiro para o 0.
    if(tam == 0){
        *caminho = 0;
        return custos[N*primeiro + 0];
    }

    //Valor máximo para iniciar.
    int menor_dist = N*MAX;
    int cam_filho;

    for (int i = 0; i < tam; i++){
        int atual = n_vazio(ja_foram,i+1);
        ja_foram[atual] = 1;
        int dist =  custos[N*primeiro + atual] + dist_cam(atual, tam-1,ja_foram,custos,&cam_filho,N);
        ja_foram[atual] = 0;
        if (dist < menor_dist) {
            menor_dist = dist;
            *caminho = cam_filho*tam + i;
        }
    }

    
    return menor_dist;
}

int main(int argc, char* argv[]){

    //Pega o valor do N vindo da entrada
    int N = atoi(argv[1]);

    //Aloca um vetor para controlar quais nós ja foram visitados.
    int* ja_foram = calloc(N,sizeof(int));

    //Gera os custos.
    int* custos = custos_aleatorios(N);

    //Acha o melhor caminho.
    int caminho;
    int menor_dist = dist_cam(0,N-1,ja_foram,custos,&caminho,N);

    //Imprime o melhor caminho.
    imprime_caminho(menor_dist,caminho,N,ja_foram);

    free(ja_foram);
    free(custos);
}
