/* 
Trabalho Final de Análise de Algoritmos
Aluno: Guilherme Neves
Matricula: UC23100623 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define MATRICULA 623
 
/* retorna o tempo atual em segundos com alta precisao usando o contador do windows.
   necessario pois clock() tem resolucao minima de ~15ms, insuficiente para medir
   buscas em memoria que completam em microsegundos. */
double tempo_agora() {
    LARGE_INTEGER freq, contador;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&contador);
    return (double)contador.QuadPart / (double)freq.QuadPart;
}

/* percorre o vetor elemento por elemento ate encontrar a chave.
   o timeout do enunciado foi comentado pois com vetores de 100k-300k elementos
   cada busca levaria horas para concluir (1 seg por comparacao). */
int PesquisaSequencial(int *V, int N, int Chave) {
    for (int i = 0; i < N; i++) {
        // system("timeout /t 1 > nul");
        if (V[i] == Chave) return i;
    }
    return -1;
}

/* divide o espaco de busca ao meio a cada iteracao.
   exige que o vetor esteja ordenado em ordem crescente.
   timeout tambem removido pelo mesmo motivo da pesquisa sequencial. */
int PesquisaBinaria(int *V, int N, int Chave) {
    int Esquerda = 0;
    int Direita = N - 1;
    while (Esquerda <= Direita) {
        int Meio = (Esquerda + Direita) / 2;
        // system("timeout /t 1 > nul");
        if (V[Meio] == Chave) return Meio;
        else if (V[Meio] < Chave) Esquerda = Meio + 1;
        else Direita = Meio - 1;
    }
    return -1;
}

/* funcao auxiliar exigida pelo qsort para comparar dois inteiros */
int comparar(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

/* aloca e preenche um vetor de N inteiros pseudoaleatorios.
   a matricula fixa garante a mesma sequencia em todas as execucoes. */
int *gerar_vetor(int N) {
    int *V = (int *)malloc(N * sizeof(int));
    if (V == NULL) {
        printf("ERRO: nao ha memoria para o vetor!\n");
        exit(1);
    }
    srand(MATRICULA);
    for (int i = 0; i < N; i++) V[i] = rand();
    return V;
}

/* gera o vetor 4 vezes medindo o tempo de cada execucao e calcula a media */
void medir_geracao(int N, int execucoes) {
    printf("\n--- Geracao de vetor com %d elementos ---\n", N);
    double total = 0.0;
    for (int e = 1; e <= execucoes; e++) {
        double Inicio = tempo_agora();
        int *V = gerar_vetor(N);
        double Fim = tempo_agora();
        double tempo = Fim - Inicio;
        printf("  Execucao %d: %.6f seg\n", e, tempo);
        total += tempo;
        free(V);
    }
    printf("  Tempo medio: %.6f seg\n", total / execucoes);
}

/* ordena o vetor com qsort (O(n log n)), mede o tempo de cada execucao
   e imprime os primeiros e ultimos 5 elementos como evidencia da ordenacao */
void medir_ordenacao(int N, int execucoes) {
    printf("\n--- Ordenacao de vetor com %d elementos (qsort) ---\n", N);
    double total = 0.0;
    int *V_final = NULL;
    for (int e = 1; e <= execucoes; e++) {
        int *V = gerar_vetor(N);
        double Inicio = tempo_agora();
        qsort(V, N, sizeof(int), comparar);
        double Fim = tempo_agora();
        double tempo = Fim - Inicio;
        printf("  Execucao %d: %.6f seg\n", e, tempo);
        total += tempo;
        if (e == execucoes) V_final = V;
        else free(V);
    }
    printf("  Tempo medio: %.6f seg\n", total / execucoes);
    printf("  Primeiros 5: %d %d %d %d %d\n",
        V_final[0], V_final[1], V_final[2], V_final[3], V_final[4]);
    printf("  Ultimos  5: %d %d %d %d %d\n",
        V_final[N-5], V_final[N-4], V_final[N-3], V_final[N-2], V_final[N-1]);
    free(V_final);
}

/* gera e ja retorna o vetor ordenado, pronto para as pesquisas */
int *gerar_vetor_ordenado(int N) {
    int *V = gerar_vetor(N);
    qsort(V, N, sizeof(int), comparar);
    return V;
}

/* testa a pesquisa sequencial com 5 elementos em posicoes distintas do vetor:
   inicio, primeiro quartil, meio, terceiro quartil e fim.
   cada elemento e buscado 4 vezes para calcular o tempo medio. */
void medir_pesquisa_seq(int N, int execucoes) {
    printf("\n--- Pesquisa Sequencial no vetor de %d elementos ---\n", N);
    int *V = gerar_vetor_ordenado(N);

    int chaves[5];
    chaves[0] = V[0];
    chaves[1] = V[N / 4];
    chaves[2] = V[N / 2];
    chaves[3] = V[3 * N / 4];
    chaves[4] = V[N - 1];

    for (int c = 0; c < 5; c++) {
        printf("  Elemento [%d] = %d:\n", c, chaves[c]);
        double total = 0.0;
        for (int e = 1; e <= execucoes; e++) {
            double Inicio = tempo_agora();
            int idx = PesquisaSequencial(V, N, chaves[c]);
            double Fim = tempo_agora();
            double tempo = Fim - Inicio;
            printf("    Exec %d: %.6f seg  (indice=%d)\n", e, tempo, idx);
            total += tempo;
        }
        printf("    Tempo medio: %.6f seg\n", total / execucoes);
    }
    free(V);
}

/* testa a pesquisa binaria com os mesmos 5 elementos usados na sequencial,
   permitindo comparacao direta entre os dois algoritmos. */
void medir_pesquisa_bin(int N, int execucoes) {
    printf("\n--- Pesquisa Binaria no vetor de %d elementos ---\n", N);
    int *V = gerar_vetor_ordenado(N);

    int chaves[5];
    chaves[0] = V[0];
    chaves[1] = V[N / 4];
    chaves[2] = V[N / 2];
    chaves[3] = V[3 * N / 4];
    chaves[4] = V[N - 1];

    for (int c = 0; c < 5; c++) {
        printf("  Elemento [%d] = %d:\n", c, chaves[c]);
        double total = 0.0;
        for (int e = 1; e <= execucoes; e++) {
            double Inicio = tempo_agora();
            int idx = PesquisaBinaria(V, N, chaves[c]);
            double Fim = tempo_agora();
            double tempo = Fim - Inicio;
            printf("    Exec %d: %.6f seg  (indice=%d)\n", e, tempo, idx);
            total += tempo;
        }
        printf("    Tempo medio: %.6f seg\n", total / execucoes);
    }
    free(V);
}

int main() {
    int tamanhos[3] = {100000, 200000, 300000};
    int execucoes = 4;

    printf("========================================\n");
    printf("  GERACAO DE VETORES ALEATORIOS\n");
    printf("========================================\n");
    for (int i = 0; i < 3; i++)
        medir_geracao(tamanhos[i], execucoes);

    printf("\n========================================\n");
    printf("  ORDENACAO (qsort - O(n log n))\n");
    printf("========================================\n");
    for (int i = 0; i < 3; i++)
        medir_ordenacao(tamanhos[i], execucoes);

    printf("\n========================================\n");
    printf("  PESQUISA SEQUENCIAL\n");
    printf("========================================\n");
    for (int i = 0; i < 3; i++)
        medir_pesquisa_seq(tamanhos[i], execucoes);

    printf("\n========================================\n");
    printf("  PESQUISA BINARIA\n");
    printf("========================================\n");
    for (int i = 0; i < 3; i++)
        medir_pesquisa_bin(tamanhos[i], execucoes);

    return 0;
}

/*
obs: como o RAND_MAX no Windows é 32767 e os vetores tem de 100 mil a 300 mil posições, 
ha elementos repetidos, por isso, para um mesmo valor de chave, a pesquisa sequencial retorna 
o indice da primeira ocorrencia, enquanto a binária pode retornar o indice de uma ocorrência 
intermediária, isso nao invalida os testes
*/