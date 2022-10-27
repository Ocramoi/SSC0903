#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib/utils.h"

//Uma Struct para a frequência de cada nota em nível municipal, regional e nacional.
typedef struct{
    int brasil[101];
    int regiao[101];
    int cidade[101];
} Frequencias;

//Uma Struct para os dados resumidos de uma região ou cidade.
typedef struct{
    int maximo;
    int minimo;
    int soma;
    float desvio,
        med;
} Dados;

//Uma Struct para acompanhar a cidade e região com maior média.
typedef struct{
    //Cidade
    int c_cidade;
    int c_regiao;
    float c_media;
    //Região
    int r_regiao;
    float r_media;
} Melhores;

//Lê cada nota de uma cidade, computa os dados resumidos.
Dados comp_cidade(unsigned int* notas,int inicio, int tamanho, Frequencias* freq, Melhores* melhores, int r, int c){
    //Cria dados vazios.
    Dados ret = {-1,101,0,0,0};
    //Zero o vetor de frequencia
    memset(freq->cidade, 0, sizeof(freq->cidade));

    //Para cada nota...
    for(int i = 0; i < tamanho; i++){
        int n = notas[inicio + i];

        //Aumenta os vetores de frequencia em todos os níveis.
        freq->brasil[n] ++;
        freq->regiao[n] ++;
        freq->cidade[n] ++;

        //Verifica se supera a nota maxima ou minima.
        if(n > ret.maximo){
            ret.maximo = n;
        }
        if( n < ret.minimo){
            ret.minimo = n;
        }

        //Atuliza a soma
        ret.soma += n;
    }

    //Calcula a media baseado na soma.
    float media = (float) ret.soma / tamanho;

    //Verifica se é a melhor cidade até o momento.
    if(media > melhores->c_media){
        melhores->c_media = media;
        melhores->c_cidade = c;
        melhores->c_regiao = r;
    }

    //Usa a média para calcular o desvio padrão.
    float desvio_total = 0.0;
    for(int i = 0; i < tamanho; i++){
        int n = notas[inicio + i];
        desvio_total += (n-media)*(n-media);
    }
    float desvio = sqrt(desvio_total/(tamanho-1));

    //Usa o vetor de frequências  para calcular a mediana.
    float med = mediana(freq->cidade,tamanho);

    #ifdef DEBUG
    printf("-> Valores gerados para a cidade: \n");
    for (unsigned int i = 0; i < sizeof(freq->cidade)/sizeof(freq->cidade[0]); i++) {
        if (!freq->cidade[i]) continue;
        printf("%d: %d, ", i + 1, freq->cidade[i]);
    } printf("\n");
    #endif

    //Imprime o resultado.
    printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n",r,c,ret.minimo,ret.maximo,med,media,desvio);

    return ret;
} 

//Baseados nos resumos das cidades computa o resumo da região.
Dados comp_regiao(unsigned int* notas, int tam_regiao, int tam_cidade, Frequencias* freq, Melhores* melhores, int r){

    //Cria dados vazios.
    Dados ret = {-1,101,0,0,0};
    //Zero o vetor de frequencia da região.
    memset(freq->regiao, 0, sizeof(freq->regiao));

    //Para cada cidade..
    for( int c = 0; c < tam_regiao; c++){
        Dados n = comp_cidade(notas, r*tam_regiao*tam_cidade + c*tam_cidade, tam_cidade, freq, melhores, r, c);

        //Verifica se supera a nota maxima ou minima.
        if(n.maximo > ret.maximo){
            ret.maximo = n.maximo;
        }
        if(n.minimo < ret.minimo){
            ret.minimo = n.minimo;
        }
        //Atuliza a soma.
        ret.soma += n.soma;
    }

    //Calcula a media baseado na soma.
    float media = (float)ret.soma / (tam_cidade*tam_regiao);

    //Verifica se é a melhor região até o momento.
    if(media > melhores->r_media){
        melhores->r_media = media;
        melhores->r_regiao = r;
    }

    //Usa a média para calcular o desvio padrão.
    float desvio_total = 0.0;
    for(int i = 0; i < tam_cidade*tam_regiao; i++){
        int n = notas[r*tam_regiao*tam_cidade + i];
        desvio_total += (n-media)*(n-media);
    }
    ret.desvio = sqrt(desvio_total/(tam_regiao*tam_cidade-1));

    //Usa o vetor de frequências  para calcular a mediana.
    ret.med = mediana(freq->regiao,tam_regiao*tam_cidade);
    return ret;
}


//Computa os dados gerais baseados nos dados de cada região.
Dados comp_geral(unsigned int* notas,int tam_pais, int tam_regiao, int tam_cidade){

    //Inicializa estruturas.
    Dados ret = {-1,101,0,0,0};
    Melhores melhores = {0,0,0,0,0};
    Frequencias freq;
    memset(freq.brasil, 0, sizeof(freq.brasil));

    //Cria um espaço para guardar os resumos das regiões, que serão impressos no final.
    Dados* regioes = malloc(tam_pais*sizeof(Dados));

    //Para cad região...
    for( int r = 0; r < tam_pais; r++){

        //Pega o resumo da região e salva no vetor.
        Dados n = comp_regiao(notas, tam_regiao, tam_cidade, &freq, &melhores, r);
        regioes[r] = n;

        //Verifica se supera a nota maxima ou minima.
        if(n.maximo > ret.maximo){
            ret.maximo = n.maximo;
        }
        if(n.minimo < ret.minimo){
            ret.minimo = n.minimo;
        }

        //Atuliza a soma.
        ret.soma += n.soma;

        //Imprime quebra de linha para separar as regiões.
        printf("\n");
    }

    //Imprime o resumo de cada região.
    for( int r = 0; r < tam_pais; r++){
        Dados ret = regioes[r];
        float media = (float) ret.soma/(tam_regiao*tam_cidade);
        printf("Reg %d: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n",r,ret.minimo,ret.maximo,ret.med,media,ret.desvio);
    }
    free(regioes);
    printf("\n");

    //Calcula a media nacional baseado na soma.
    float media = (float)ret.soma / (tam_cidade*tam_regiao*tam_pais);

    //Usa a média para calcular o desvio padrão.
    float desvio_total = 0.0;
    for(int i = 0; i < tam_cidade*tam_regiao*tam_pais; i++){
        int n = notas[i];
        desvio_total += (n-media)*(n-media);
    }
    float desvio = sqrt(desvio_total/(tam_pais*tam_regiao*tam_cidade-1));

    //Usa o vetor de frequências  para calcular a mediana.
    float med = mediana(freq.brasil,tam_pais*tam_regiao*tam_cidade);

    //Imprime os dados nacionais, a cidade e região premeadas.
    printf("Brasil: menor: %d, maior: %d, mediana: %.2f, média: %.2f e DP: %.2f\n",ret.minimo,ret.maximo,med,media,desvio);
    printf("\n");
    printf(": %f\n", melhores.r_media);
    printf("Melhor região: Região %d\n",melhores.r_regiao);
    printf(": %f\n", melhores.c_media);
    printf("Melhor cidade: Região %d, cidade %d\n",melhores.c_regiao,melhores.c_cidade);
    return ret;
}

int main(){
    unsigned int r, c, a, seed;
    if (!scanf("%u %u %u %u", &r, &c, &a, &seed)) exit(1);
    srand(seed);
    unsigned int* notas = valoresAleatorios(r*c*a);
    comp_geral(notas, r, c, a);
    free(notas);
}    
