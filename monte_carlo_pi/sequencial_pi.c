#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

// Calcula o tempo decorrido entre dois pontos usando clock_gettime()
double get_elapsed_time(struct timespec *start, struct timespec *end) {
    double start_sec = start->tv_sec + (start->tv_nsec / 1e9);
    double end_sec = end->tv_sec + (end->tv_nsec / 1e9);
    return end_sec - start_sec;
}

int main(int argc, char *argv[]) {
    // Verifica se o número de lançamentos foi passado corretamente
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero de lançamentos>\n", argv[0]);
        return 1;
    }

    // Converte argumento para long long com verificação de validade
    char *endptr;
    long long total_tosses = strtoll(argv[1], &endptr, 10);
    if (*endptr != '\0' || total_tosses <= 0) {
        fprintf(stderr, "Erro: forneça um número inteiro positivo.\n");
        return 1;
    }

    long long in_circle = 0;              // Contador de pontos dentro do círculo
    double x, y, elapsed_time;
    struct timespec start_time, end_time;

    srand(time(NULL));                    // Inicializa o gerador de números aleatórios
    printf("Gerando %lld pontos aleatórios...\n", total_tosses);

    clock_gettime(CLOCK_MONOTONIC, &start_time); // Início da medição de tempo

    // Loop principal de Monte Carlo: gera pontos e conta quantos caem no círculo
    for (long long i = 0; i < total_tosses; i++) {
        x = (double)rand() / (RAND_MAX + 1.0);   // Coordenada x em [0.0, 1.0)
        y = (double)rand() / (RAND_MAX + 1.0);   // Coordenada y em [0.0, 1.0)
        if ((x * x + y * y) <= 1.0) {
            in_circle++;                         // Conta se estiver dentro do quarto de círculo
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);   // Fim da medição de tempo
    elapsed_time = get_elapsed_time(&start_time, &end_time);

    // Estimação de Pi baseada na proporção de pontos no círculo
    double pi_estimate = 4.0 * (double)in_circle / (double)total_tosses;

    // Exibe os resultados finais
    printf("\nResultados:\n");
    printf("Total de pontos gerados (N_total): %lld\n", total_tosses);
    printf("Pontos dentro do quarto de círculo (N_inside): %lld\n", in_circle);
    printf("Estimativa de Pi ≈ 4 * (N_inside / N_total): %f\n", pi_estimate);
    printf("Tempo de CPU usado: %f segundos\n", elapsed_time);

    return 0;
}

