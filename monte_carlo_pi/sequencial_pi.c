#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

// Função auxiliar para calcular a diferença de tempo em segundos (double)
double get_elapsed_time(struct timespec *start, struct timespec *end) {
    double start_sec = start->tv_sec + (start->tv_nsec / 1e9);
    double end_sec = end->tv_sec + (end->tv_nsec / 1e9);
    return end_sec - start_sec;
}

int main(int argc, char *argv[]) {

    long long toss, number_of_tosses; 
    long long number_in_circle = 0;
    double x, y, elapsed_time;
    struct timespec start_time, end_time;
	
    // Verifica se o número de lançamentos foi passado como argumento via CLI
    if (argc != 2){
	    fprintf(stderr, "Uso: %s <numero de lançamentos>", argv[0]);
	    return 1;
    }

    number_of_tosses = strtoll(argv[1], NULL, 10); // Converte o argumento para long long

    // Verifica se o número de lançamentos não é negativo
    if (number_of_tosses < 0 ){
    	fprintf(stderr, "Erro: O Número de lançamentos deve ser positivvo");
    }

    // Semear o gerador de números aleatórios uma vez
    srand(time(NULL));
    printf("Gerando %lld pontos aleatórios...\n", number_of_tosses);

    // --- INÍCIO DA MEDIÇÃO DE TEMPO ---
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Loop principal
    for (toss = 0; toss < number_of_tosses; toss++) {
        // Gerar coordenadas x e y aleatórias entre 0.0 e 1.0
        x = (double)rand() / (RAND_MAX + 1.0);
        y = (double)rand() / (RAND_MAX + 1.0);

        // Verificar se o ponto (x, y) está dentro do quarto de círculo unitário
        if ((x * x + y * y) <= 1.0) {
            number_in_circle++; // Incrementa o contador se estiver dentro
        }
    }
    // --- FIM DA MEDIÇÃO DE TEMPO ---
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calcular o tempo decorrido
    elapsed_time = get_elapsed_time(&start_time, &end_time);

    // Calcular a estimativa de Pi
    double pi_estimate = 4.0 * (double)number_in_circle / (double)number_of_tosses;

    // Imprimir os resultados
    printf("\nResultados:\n");
    printf("Total de pontos gerados (N_total): %lld\n", number_of_tosses);
    printf("Pontos dentro do quarto de círculo (N_inside): %lld\n", number_in_circle);
    printf("Estimativa de Pi ≈ 4 * (N_inside / N_total): %f\n", pi_estimate);
    printf("Tempo de CPU usado: %f segundos\n", elapsed_time);

    return 0; 
}
