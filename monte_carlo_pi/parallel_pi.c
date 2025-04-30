#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Necessário para Pthreads
#include <time.h>   // Para time() (semente)
#include <errno.h>  // Para códigos de erro (opcional, mas bom)

// Estrutura para passar como argumento na criação da thread
typedef struct{
    long long tosses;
    long long *results;
} ThreadArgs;    

// Função para calcular a diferença de tempo em segundos
double get_elapsed_time(struct timespec *start, struct timespec *end){
    double start_sec = start->tv_sec + (start->tv_nsec / 1e9);
    double end_sec = end->tv_sec + (end->tv_nsec / 1e9);
    double elapsed_sec = end_sec - start_sec;
    return elapsed_sec;
}

// Função que cada thread executará
void* monte_carlo_thread(void* arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    long long tosses_per_thread = args->tosses;
    long long local_in_circle = 0; // Contador local para esta thread
    unsigned int thread_seed; // Semente para gerador (embora rand() use estado global)
    double x, y;
    long long i;

    struct drand48_data rand_buffer;

    long seed = time(NULL) ^ (long)pthread_self();
    srand48_r(seed, &rand_buffer);

    // Gerar pontos e contar localmente
    for (i = 0; i < tosses_per_thread; ++i) {
        // Gera x e y entre 0.0 e 1.0
        drand48_r(&rand_buffer, &x);
        drand48_r(&rand_buffer, &y);

        // Verifica se está dentro do quarto de círculo
        if ((x * x + y * y) <= 1.0) {
            local_in_circle++;
        }
    }

    
    *(args->results) = local_in_circle;
    return NULL;
}

int main(int argc, char *argv[]) {


    long long total_tosses, total_in_circle;
    int num_threads;
	
    struct timespec start_total_time, end_total_time;
    struct timespec start_partial_time, end_partial_time;
    double total_elapsed_time, partial_elapsed_time ;
    
    pthread_t* thread_handles;
    long long i;
    long long tosses_for_each_thread;
    int ret;

    // --- INÍCIO DA MEDIÇÃO DE TEMPO TOTAL ---
    clock_gettime(CLOCK_MONOTONIC, &start_total_time);

    num_threads = atoi(argv[1]);
    total_tosses = strtoll(argv[2], NULL, 10); // Usar strtoll para números grandes

    ThreadArgs *args_array = malloc(num_threads * sizeof(ThreadArgs));
    long long *results = malloc(num_threads * sizeof(long long));

    // Obter número de threads e lançamentos totais (exemplo: da linha de comando)
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <numero de threads> <numero total de lançamentos>\n", argv[0]);
        return 1;
    }

    if (num_threads <= 0 || total_tosses <= 0) {
        fprintf(stderr, "Número de threads e lançamentos devem ser positivos.\n");
        return 1;
    }

     // Aloca memória para os handles das threads
    thread_handles = malloc(num_threads * sizeof(pthread_t));
    if (thread_handles == NULL) {
        perror("Falha ao alocar handles das threads");
        return 1;
    }

    // Calcula quantos lançamentos cada thread fará (divisão simples)
    // (Uma divisão mais robusta lidaria com restos se total_tosses não for divisível)
    tosses_for_each_thread = total_tosses / num_threads;
    printf("Calculando com %d threads, %lld lançamentos por thread...\n", num_threads, tosses_for_each_thread);

    // --- INÍCIO DA MEDIÇÃO DE TEMPO PARCIAL ---
    clock_gettime(CLOCK_MONOTONIC, &start_partial_time);

    // 3. Criar as threads
    for (i = 0; i < num_threads; ++i) {
        // Passa o número de lançamentos para a thread como argumento
	args_array[i].tosses = tosses_for_each_thread;
	args_array[i].results = &results[i];
        ret = pthread_create(&thread_handles[i], NULL, monte_carlo_thread, &args_array[i]);
         if (ret != 0) {
            errno = ret;
            perror("pthread_create falhou");
            free(thread_handles);
            return 1;
        }
    }

    // 4. Esperar (Join) todas as threads terminarem
    for (i = 0; i < num_threads; ++i) {
        pthread_join(thread_handles[i], NULL);
	total_in_circle += results[i];
    }

    // 6. Calcular e imprimir a estimativa final de Pi
    double pi_estimate = 4.0 * (double)total_in_circle / (double)total_tosses;

    // --- FIM DA MEDIÇÃO DE TEMPO PARCIAL --- 
    clock_gettime(CLOCK_MONOTONIC, &end_partial_time);

    // Liberar memória alocada para os handles
    free(thread_handles);

    // --- FIM DA MEDIÇÃO DE TEMPO TOTAL ---
    clock_gettime(CLOCK_MONOTONIC, &end_total_time);

    // Calcular tempo total decorrido
    total_elapsed_time = get_elapsed_time(&start_total_time, &end_total_time);
    // Calcular tempo parcial decorrido
    partial_elapsed_time = get_elapsed_time(&start_partial_time, &end_partial_time);

    printf("\nResultados Finais:\n");
    printf("Total de pontos gerados (N_total): %lld\n", total_tosses);
    printf("Pontos dentro do quarto de círculo (N_inside): %lld\n", total_in_circle);
    printf("Estimativa de Pi ≈ 4 * (N_inside / N_total): %f\n", pi_estimate);
    printf("Tempo total decorrido: %f segundos\n", total_elapsed_time);
    printf("Tempo parcial decorrido: %f segundos\n", partial_elapsed_time);

    return 0;
}
