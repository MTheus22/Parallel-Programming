#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

// Estrutura usada para passar argumentos à thread
typedef struct {
    long long tosses;
    long long *result;
} ThreadArgs;

// Função para calcular tempo decorrido em segundos
double get_elapsed_time(struct timespec *start, struct timespec *end) {
    double start_sec = start->tv_sec + (start->tv_nsec / 1e9);
    double end_sec = end->tv_sec + (end->tv_nsec / 1e9);
    return end_sec - start_sec;
}

// Função executada por cada thread
void* monte_carlo_thread(void* arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    long long tosses = args->tosses;
    long long local_in_circle = 0;
    double x, y;

    // Inicializar gerador aleatório local
    struct drand48_data rand_buffer;
    long seed = time(NULL) ^ (long)pthread_self();
    srand48_r(seed, &rand_buffer);

    for (long long i = 0; i < tosses; ++i) {
        drand48_r(&rand_buffer, &x);
        drand48_r(&rand_buffer, &y);
        if ((x * x + y * y) <= 1.0) {
            local_in_circle++;
        }
    }

    *(args->result) = local_in_circle;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <numero de threads> <numero total de lançamentos>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    long long total_tosses = strtoll(argv[2], NULL, 10);

    if (num_threads <= 0 || total_tosses <= 0) {
        fprintf(stderr, "Número de threads e lançamentos devem ser positivos.\n");
        return 1;
    }

    // Medição de tempo total e parcial
    struct timespec start_total_time, end_total_time;
    struct timespec start_partial_time, end_partial_time;
    clock_gettime(CLOCK_MONOTONIC, &start_total_time);

    // Alocação de recursos
    pthread_t *thread_handles = malloc(num_threads * sizeof(pthread_t));
    ThreadArgs *args_array = malloc(num_threads * sizeof(ThreadArgs));
    long long *results = malloc(num_threads * sizeof(long long));

    if (!thread_handles || !args_array || !results) {
        perror("Erro de alocação");
        free(thread_handles);
        free(args_array);
        free(results);
        return 1;
    }

    // Distribuição justa dos lançamentos entre threads
    long long base_tosses = total_tosses / num_threads;
    long long remainder = total_tosses % num_threads;

    printf("Calculando com %d threads...\n", num_threads);
    clock_gettime(CLOCK_MONOTONIC, &start_partial_time);

    for (int i = 0; i < num_threads; ++i) {
        args_array[i].tosses = base_tosses + (i < remainder ? 1 : 0);
        args_array[i].result = &results[i];
        int ret = pthread_create(&thread_handles[i], NULL, monte_carlo_thread, &args_array[i]);
        if (ret != 0) {
            errno = ret;
            perror("pthread_create falhou");
            free(thread_handles);
            free(args_array);
            free(results);
            return 1;
        }
    }

    long long total_in_circle = 0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(thread_handles[i], NULL);
        total_in_circle += results[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end_partial_time);
    clock_gettime(CLOCK_MONOTONIC, &end_total_time);

    // Estimativa final de Pi
    double pi_estimate = 4.0 * (double)total_in_circle / (double)total_tosses;

    // Tempo decorrido
    double total_elapsed = get_elapsed_time(&start_total_time, &end_total_time);
    double partial_elapsed = get_elapsed_time(&start_partial_time, &end_partial_time);

    // Resultados
    printf("\nResultados Finais:\n");
    printf("Total de pontos gerados (N_total): %lld\n", total_tosses);
    printf("Pontos dentro do quarto de círculo (N_inside): %lld\n", total_in_circle);
    printf("Estimativa de Pi ≈ 4 * (N_inside / N_total): %f\n", pi_estimate);
    printf("Tempo total decorrido: %f segundos\n", total_elapsed);
    printf("Tempo parcial decorrido: %f segundos\n", partial_elapsed);

    // Liberação de recursos
    free(thread_handles);
    free(args_array);
    free(results);

    return 0;
}

