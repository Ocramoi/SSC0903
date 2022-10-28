#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include <string.h> // TODO remove if not working with memset

#include "lib/utils.h"

/* Estruturas de dados calculados */
typedef struct {
    unsigned int *grades,
        maior,
        menor;
    double media,
        mediana,
        dp;
} City;

typedef struct {
    City* cities;
    unsigned int maior,
        menor;
    double mediana,
        media,
        dp;
} Region;

typedef struct {
    Region* regions;
    unsigned long nRegions,
        nCities,
        nStudents;
    unsigned int maior,
        menor;
    double media,
        mediana,
        dp;

    struct bestRegion_t {
        double value;
        unsigned long region;
    } bestRegion;

    struct bestCity_t {
        double value;
        unsigned long region,
            city;
    } bestCity;
} Country;

/* Libera memória interna de todos os nós do país (suas regiões, cidades, etc.) */
unsigned short freeInnerCountry(Country* country) {
    if (!country) return 1;

    for (typeof(country->nRegions) i = 0; i < country->nRegions; ++i) {
        for (typeof(country->nCities) j = 0; j < country->nCities; ++j) {
            if (!country->regions[i].cities[j].grades) return 1;
            free(country->regions[i].cities[j].grades);
        }
        if (!country->regions[i].cities) return 1;
        free(country->regions[i].cities);
    }
    if (!country->regions) return 1;
    free(country->regions);

    return 0;
}

/* Gera estrutura inicial com valores de notas aleatórios */
Country* setup(unsigned int nRegions, unsigned int nCities, unsigned int nStudents, unsigned int seed) {
    // Usa seed dada
    srand(seed);

    // Salva dados de entrada e aloca espaço inicial do país
    Country *c = malloc(sizeof(Country));
    c->regions = malloc(nRegions * sizeof(Region));
    c->nRegions = nRegions;
    c->nCities = nCities;
    c->nStudents = nStudents;

    for (typeof(nRegions) i = 0; i < nRegions; ++i) {
        // Para cada região aloca seu espaço para cidades
        c->regions[i].cities = malloc(nCities * sizeof(City));
        for (typeof(nCities) j = 0; j < nCities; ++j)
            // Para cada cidade gera suas notas aleatórias de alunos
            c->regions[i].cities[j].grades = valoresAleatorios(nStudents);
    }

    return c;
}

/* Computa os dados pedidos sobre o país dado e inicializado com os valores passados */
void comp(Country* country) {
    if (!country) exit(1);

    // Permite loops paralelizados aninhadas
    omp_set_nested(1);

    unsigned int freqs[SPREAD] = { 0 }, // Vetor de frequências de notas para cálculo
        // Valores a serem calculados no contexto do país
        countryMax = 0,
        countryMin = UINT_MAX;
    // Variáveis auxiliares para valor de melhor cidade e região
    double bestRegion = 0,
        bestCity = 0,
        countryMed = 0;

    // Loop paralelo entre as regiões do país reduzindo os valores procurados
    #pragma omp parallel for reduction(+: freqs[:SPREAD], countryMed) \
        reduction(max: countryMax, bestRegion, bestCity) \
        reduction(min: countryMin) \
        firstprivate(country)
    for (typeof(country->nRegions) i = 0; i < country->nRegions; ++i) {
        // Ponteiro para a atual região no loop (auxiliar)
        Region *curRegion = &country->regions[i];
        // Valores a serem calculados no contexto da região
        unsigned int regionMax = 0,
            regionMin = UINT_MAX;
        double regionMed = 0;
        typeof(freqs) regionFreqs = { 0 };

        /* Loop entre as cidades de cada região reduzindo os valores procurados */
        #pragma omp parallel for reduction(+: regionFreqs[:SPREAD], regionMed) \
            reduction(max: regionMax, bestCity) \
            reduction(min: regionMin)           \
            firstprivate(curRegion)
        for (typeof(country->nCities) j = 0; j < country->nCities; ++j) {
            // Ponteiro para a atual cidade no loop (auxiliar)
            City *curCity = &curRegion->cities[j];
            // Valores a serem calculados no contexto da cidade
            unsigned int cityMax = 0,
                cityMin = UINT_MAX;
            double cityMed = 0;
            typeof(freqs) cityFreqs = { 0 };

            // Redução dos valores das notas da cidade
            #pragma omp simd reduction(+: cityFreqs[:SPREAD], cityMed) \
                reduction(max: cityMax) \
                reduction(min: cityMin)
            for (typeof(country->nStudents) k = 0; k < country->nStudents; ++k) {
                unsigned int grade = curCity->grades[k];
                cityFreqs[grade]++;
                if (grade > cityMax) cityMax = grade;
                if (grade < cityMin) cityMin = grade;
                cityMed += grade/(1.f * country->nStudents);
            }

            // Mede dados do nível superior
            regionMed += cityMed/(1.f * country->nCities);
            if (cityMax > regionMax) regionMax = cityMax;
            if (cityMin < regionMin) regionMin = cityMin;
            if (cityMed > bestCity) bestCity = cityMed;

            // Registra valores da cidade com base nos dados reduzidos das notas
            curCity->maior = cityMax;
            curCity->menor = cityMin;
            curCity->media = cityMed;
            curCity->mediana = mediana(cityFreqs, country->nStudents);
            curCity->dp = dpParalelo(cityFreqs, country->nStudents, cityMed);

            // Atualiza vetor de frequência do nível superior
            #pragma omp simd
            for (int s = 0; s < SPREAD; ++s)
                regionFreqs[s] += cityFreqs[s];
        }

        // Registra dados do nível superior
        countryMed += regionMed/(1.f * country->nRegions);
        if (regionMax > countryMax) countryMax = regionMax;
        if (regionMin < countryMin) countryMin = regionMin;
        if (regionMed > bestRegion) bestRegion = regionMed;

        // Registra valores da região com base nos dados reduzidos das cidades
        curRegion->maior = regionMax;
        curRegion->menor = regionMin;
        curRegion->media = regionMed;
        curRegion->mediana = mediana(regionFreqs, country->nCities * country->nStudents);
        curRegion->dp = dpParalelo(regionFreqs, country->nCities * country->nStudents, regionMed);

        // Atualiza vetor de frequência do nível superior
        #pragma omp simd
        for (int s = 0; s < SPREAD; ++s)
            freqs[s] += regionFreqs[s];
    }

    // Registra valores do país com base nos dados reduzidos das regiões
    country->maior = countryMax;
    country->menor = countryMin;
    country->media = countryMed;
    country->mediana = mediana(freqs, country->nRegions * country->nCities * country->nStudents);
    country->dp = dpParalelo(freqs, country->nRegions * country->nCities * country->nStudents, countryMed);
    country->bestCity.value = bestCity; country->bestCity.city = ULONG_MAX; country->bestCity.region = ULONG_MAX;
    country->bestRegion.value = bestRegion; country->bestRegion.region = ULONG_MAX;
}

// Exibe valores computados com a formatação dada
void display(Country* country) {
    for (typeof(country->nRegions) i = 0; i < country->nRegions; ++i) {
        for (typeof(country->nCities) j = 0; j < country->nCities; ++j) {
            printf(
                "Reg %lu - Cid %lu: menor: %u, maior: %u, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",
                i,
                j,
                country->regions[i].cities[j].menor,
                country->regions[i].cities[j].maior,
                country->regions[i].cities[j].mediana,
                country->regions[i].cities[j].media,
                country->regions[i].cities[j].dp
            );
            // Registra id da região e cidade da melhor média de cidade registrado
            if (country->regions[i].cities[j].media >= country->bestCity.value) country->bestCity.region = i, country->bestCity.city = j;
        }
        printf("\n");
    }

    for (typeof(country->nRegions) i = 0; i < country->nRegions; ++i) {
        printf(
            "Reg %lu: menor: %u, maior: %u, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",
            i,
            country->regions[i].menor,
            country->regions[i].maior,
            country->regions[i].mediana,
            country->regions[i].media,
            country->regions[i].dp
        );
        // Registra id da região da melhor média
        if (country->regions[i].media >= country->bestRegion.value) country->bestRegion.region = i;
    }

    printf(
        "\nBrasil: menor: %u, maior: %u, mediana: %.2lf, média: %.2lf e DP: %.2lf\n",
        country->menor,
        country->maior,
        country->mediana,
        country->media,
        country->dp
    );

    printf("\nMelhor região: Região %lu", country->bestRegion.region);
    printf("\nMelhor cidade: Região %lu, Cidade %lu\n", country->bestCity.region, country->bestCity.city);
}

int main() {
    // Leitura dos parâmetros do cálculo
    unsigned int nRegions, nCities, nStudents, seed;
    if (!scanf("%u %u %u %u", &nRegions, &nCities, &nStudents, &seed)) exit(1);

    // Gera país
    Country* country = setup(nRegions, nCities, nStudents, seed);

    // Variáveis para cálculo de tempo elapsado
    double init, end, aux;
    aux = omp_get_wtime();
    init = omp_get_wtime();

    // Computa valores
    comp(country);

    // Calcula e exibe tempo elapsado
    end = omp_get_wtime();

    // Exibição formatada
    display(country);
    printf("\nTempo de resposta sem considerar E/S, em segundos: %.3lfs\n", (end - init - (init - aux)));

    // Libera memória
    freeInnerCountry(country);
    if (country) free(country);

    return 0;
}
