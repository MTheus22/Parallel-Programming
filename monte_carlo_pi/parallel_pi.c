#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Necessário para Pthreads
#include <time.h>   // Para time() (semente)
#include <errno.h>  // Para códigos de erro (opcional, mas bom)

// Variáveis globais compartilhadas entre as threads
long long total_tosses;
long long total_in_circle = 0; // Contador global de pontos dentro do círculo
int num_threads;
pthread_mutex_t mutex; // Mutex para proteger o acesso a total_in_circle

// Função para calcular a diferença de tempo em segundos
double get_elapsed_time(struct timespec *start, struct timespec *end){
    double start_sec = start->tv_sec + (start->tv_nsec / 1e9);
    double end_sec = end->tv_sec + (end->tv_nsec / 1e9);
    double elapsed_sec = end_sec - start_sec;
    return elapsed_sec;
}

// Função que cada thread executará
void* monte_carlo_thread(void* arg) {
    long long tosses_per_thread = *(long long*)arg; // Recebe o nº de lançamentos para esta thread
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

    // --- Seção Crítica ---
    // Bloqueia o mutex antes de atualizar o contador global compartilhado
    pthread_mutex_lock(&mutex);

    total_in_circle += local_in_circle; // Adiciona a contagem local ao total global

    // Libera o mutex após a atualização
    pthread_mutex_unlock(&mutex);
    // --- Fim da Seção Crítica ---

    return NULL;
}

int main(int argc, char *argv[]) {

    struct timespec start_total_time, end_total_time;
    struct timespec start_partial_time, end_partial_time;
    double total_elapsed_time, partial_elapsed_time ;
    
    pthread_t* thread_handles;
    long long i;
    long long tosses_for_each_thread;
    int ret;

    // Obter número de threads e lançamentos totais (exemplo: da linha de comando)
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <numero de threads> <numero total de lançamentos>\n", argv[0]);
        return 1;
    }

    // --- INÍCIO DA MEDIÇÃO DE TEMPO TOTAL ---
    clock_gettime(CLOCK_MONOTONIC, &start_total_time);

    num_threads = atoi(argv[1]);
    total_tosses = strtoll(argv[2], NULL, 10); // Usar strtoll para números grandes

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

    // 2. Inicializar o mutex
    ret = pthread_mutex_init(&mutex, NULL);
     if (ret != 0) {
        errno = ret;
        perror("pthread_mutex_init falhou");
        free(thread_handles);
        return 1;
    }


    // --- INÍCIO DA MEDIÇÃO DE TEMPO PARCIAL ---
    clock_gettime(CLOCK_MONOTONIC, &start_partial_time);

    // 3. Criar as threads
    for (i = 0; i < num_threads; ++i) {
        // Passa o número de lançamentos para a thread como argumento
        ret = pthread_create(&thread_handles[i], NULL, monte_carlo_thread, &tosses_for_each_thread);
         if (ret != 0) {
            errno = ret;
            perror("pthread_create falhou");
            // Lidar com erro (talvez cancelar threads já criadas)
            free(thread_handles);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
    }

    // 4. Esperar (Join) todas as threads terminarem
    for (i = 0; i < num_threads; ++i) {
        pthread_join(thread_handles[i], NULL);
    }

    // 6. Calcular e imprimir a estimativa final de Pi
    double pi_estimate = 4.0 * (double)total_in_circle / (double)total_tosses;

    // --- FIM DA MEDIÇÃO DE TEMPO PARCIAL --- 
    clock_gettime(CLOCK_MONOTONIC, &end_partial_time);

    // 5. Destruir o mutex
    pthread_mutex_destroy(&mutex);

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
