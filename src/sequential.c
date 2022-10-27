#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib/utils.h"

unsigned int r, c, a, seed;

//Uma Struct para a frequência de cada nota em nível municipal, regional e nacional.
typedef struct{
    unsigned int brasil[101];
    unsigned int regiao[101];
    unsigned int cidade[101];
} Frequencias;

//Uma Struct para os dados resumidos de uma região ou cidade.
typedef struct{
    int maximo;
    int minimo;
    int soma;
    double desvio,
        med;
} Dados;

//Uma Struct para acompanhar a cidade e região com maior média.
typedef struct{
    //Cidade
    int c_cidade;
    int c_regiao;
    double c_media;
    //Região
    int r_regiao;
    double r_media;
} Melhores;

//Lê cada nota de uma cidade, computa os dados resumidos.
Dados comp_cidade(unsigned int* notas,int inicio, int tamanho, Frequencias* freq, Melhores* melhores, int reg, int cid, Dados *vals){
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
    double media = (double) ret.soma / tamanho;

    //Verifica se é a melhor cidade até o momento.
    if(media > melhores->c_media){
        melhores->c_media = media;
        melhores->c_cidade = cid;
        melhores->c_regiao = reg;
    }

    //Usa a média para calcular o desvio padrão.
    double desvio_total = 0.0;
    for(int i = 0; i < tamanho; i++){
        int n = notas[inicio + i];
        desvio_total += (n-media)*(n-media);
    }
    double desvio = sqrt(desvio_total/(tamanho));

    //Usa o vetor de frequências  para calcular a mediana.
    double med = mediana(freq->cidade,tamanho);

    //Imprime o resultado.
    /* printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",r,c,ret.minimo,ret.maximo,med,media,desvio); */
    vals[(reg*c) + cid].minimo = ret.minimo;
    vals[(reg*c) + cid].maximo = ret.maximo;
    vals[(reg*c) + cid].soma = ret.soma;
    vals[(reg*c) + cid].med = med;
    vals[(reg*c) + cid].desvio = desvio;

    return ret;
} 

//Baseados nos resumos das cidades computa o resumo da região.
Dados comp_regiao(unsigned int* notas, int tam_regiao, int tam_cidade, Frequencias* freq, Melhores* melhores, int reg, Dados *vals){

    //Cria dados vazios.
    Dados ret = {-1,101,0,0,0};
    //Zero o vetor de frequencia da região.
    memset(freq->regiao, 0, sizeof(freq->regiao));

    //Para cada cidade..
    for( int c = 0; c < tam_regiao; c++){
        Dados n = comp_cidade(notas, reg*tam_regiao*tam_cidade + c*tam_cidade, tam_cidade, freq, melhores, reg, c, vals);

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
    double media = (double)ret.soma / (tam_cidade*tam_regiao);

    //Verifica se é a melhor região até o momento.
    if(media > melhores->r_media){
        melhores->r_media = media;
        melhores->r_regiao = reg;
    }

    //Usa a média para calcular o desvio padrão.
    double desvio_total = 0.0;
    for(int i = 0; i < tam_cidade*tam_regiao; i++){
        int n = notas[reg*tam_regiao*tam_cidade + i];
        desvio_total += (n-media)*(n-media);
    }
    ret.desvio = sqrt(desvio_total/(tam_regiao*tam_cidade));

    //Usa o vetor de frequências  para calcular a mediana.
    ret.med = mediana(freq->regiao,tam_regiao*tam_cidade);
    return ret;
}

//Computa os dados gerais baseados nos dados de cada região.
Dados comp_geral(unsigned int* notas,int tam_pais, int tam_regiao, int tam_cidade, Dados *vals, Melhores *br){
    //Inicializa estruturas.
    Dados ret = {-1,101,0,0,0};
    Melhores melhores = {0,0,0,0,0};
    Frequencias freq;
    memset(freq.brasil, 0, sizeof(freq.brasil));

    //Cria um espaço para guardar os resumos das regiões, que serão impressos no final.
    Dados* regioes = malloc(tam_pais*sizeof(Dados));

    //Para cad região...
    for( int i = 0; i < tam_pais; i++){

        //Pega o resumo da região e salva no vetor.
        Dados n = comp_regiao(notas, tam_regiao, tam_cidade, &freq, &melhores, i, vals);
        regioes[i] = n;

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

    //Imprime o resumo de cada região.
    for( int i = 0; i < tam_pais; i++){
        Dados ret = regioes[i];
        /* double media = (double) ret.soma/(tam_regiao*tam_cidade); */
        /* printf("Reg %d: menor: %d, maior: %d, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",r,ret.minimo,ret.maximo,ret.med,media,ret.desvio); */
        vals[(tam_pais*tam_regiao) + i].minimo = ret.minimo;
        vals[(tam_pais*tam_regiao) + i].maximo = ret.maximo;
        vals[(tam_pais*tam_regiao) + i].med = ret.med;
        vals[(tam_pais*tam_regiao) + i].soma = ret.soma;
        vals[(tam_pais*tam_regiao) + i].desvio = ret.desvio;
    }
    free(regioes);

    //Calcula a media nacional baseado na soma.
    double media = (double)ret.soma / (tam_cidade*tam_regiao*tam_pais);

    //Usa a média para calcular o desvio padrão.
    double desvio_total = 0.0;
    for(int i = 0; i < tam_cidade*tam_regiao*tam_pais; i++){
        int n = notas[i];
        desvio_total += (n-media)*(n-media);
    }
    double desvio = sqrt(desvio_total/(tam_pais*tam_regiao*tam_cidade));

    //Usa o vetor de frequências  para calcular a mediana.
    double med = mediana(freq.brasil,tam_pais*tam_regiao*tam_cidade);

    //Imprime os dados nacionais, a cidade e região premeadas.
    /* printf("Brasil: menor: %d, maior: %d, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",ret.minimo,ret.maximo,med,media,desvio); */
    vals[(tam_pais*tam_regiao) + tam_regiao].minimo = ret.minimo;
    vals[(tam_pais*tam_regiao) + tam_regiao].maximo = ret.maximo;
    vals[(tam_pais*tam_regiao) + tam_regiao].soma = ret.soma;
    vals[(tam_pais*tam_regiao) + tam_regiao].med = med;
    vals[(tam_pais*tam_regiao) + tam_regiao].desvio = desvio;
    /* printf("\nMelhor região: Região %d\n",melhores.r_regiao); */
    /* printf("Melhor cidade: Região %d, Cidade %d\n",melhores.c_regiao,melhores.c_cidade); */
    br->r_regiao = melhores.r_regiao;
    br->c_regiao = melhores.c_regiao;
    br->c_cidade = melhores.c_cidade;
    return ret;
}

int main(){
    if (!scanf("%u %u %u %u", &r, &c, &a, &seed)) exit(1);
    srand(seed);
    unsigned int* notas = valoresAleatorios(r*c*a);

    Dados *vals = malloc(((r*c) + r + 1)*sizeof(Dados));
    Melhores *melhores = malloc(sizeof(Melhores));

    clock_t init, end, aux;
    aux = clock();
    init = clock();
    comp_geral(notas, r, c, a, vals, melhores);
    end = clock();
    printf("Time elapsed: %lf\n", (end - init - (init - aux))/(1.f*CLOCKS_PER_SEC));

    for (unsigned long i = 0; i < r*c; ++i) {
        if (i != 0 && !(i % c)) printf("\n");
        printf(
            "Reg %lu - Cid %lu: menor: %d, maior: %d, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",
            i / c,
            i % c,
            vals[i].minimo,
            vals[i].maximo,
            vals[i].med,
            vals[i].soma/((double) c),
            vals[i].desvio
        );
    }
    printf("\n");

    for (unsigned long i = r*c; i < r*c + r; ++i) {
        printf(
            "Reg %lu: menor: %d, maior: %d, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",
            i  - r*c,
            vals[i].minimo,
            vals[i].maximo,
            vals[i].med,
            vals[i].soma/((double) c*a),
            vals[i].desvio
        );
    }
    printf("\n");

    printf(
        "Brasil: menor: %d, maior: %d, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",
        vals[r*c + r].minimo,
        vals[r*c + r].maximo,
        vals[r*c + r].med,
        vals[r*c + r].soma/((double)r*c*a),
        vals[r*c + r].desvio
    );


    printf("\nMelhor região: Região %d\n", melhores->r_regiao);
    printf("Melhor cidade: Região %d, Cidade %d\n", melhores->c_regiao, melhores->c_cidade);

    free(notas);
    free(vals);
    free(melhores);

    return 0;
}    
