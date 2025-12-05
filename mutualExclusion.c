/* mutualExclusion.c demonstrates the Mutual Exclusion pattern using pthreads
 *  to deposit and withdraw $$ from a bank account.
 *  When an even number of threads are used, the final balance should be $0.00,
 *   but this does not happen, due to a race condition.
 *
 * Joel Adams, Calvin College, Fall 2013.
 *
 * Usage: ./mutualExclusion [evenNumberOfThreads]
 * 
 * Exercise:
 *   1. Build and run several times, for each of 2, 4, 8, 16, ... threads.
 *   2. In bankAccount.h:
 *      - Surround each nonlocal access to bankAccountBalance with:
 *             pthread_mutex_lock( &lock );   // before the access
 *             pthread_mutex_unlock( &lock ); // after the access
 *   3. Save, rebuild, rerun, and note the difference in results.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "bankAccount.h"

#define NUM_TRANSACTIONS 1000

// Tipos de operação
#define OP_DEPOSIT   0
#define OP_WITHDRAW  1
#define OP_CHECK     2
#define OP_TRANSFER  3


/*
 * Função executada por cada thread.
 * Para cada iteração, a operação é escolhida aleatoriamente,
 * assim como a conta envolvida (ou contas, no caso de transferência).
 *
 * As operações realizadas chamam diretamente as funções do módulo
 * bankAccount.c, que já implementam o acesso seguro aos recursos compartilhados.
 */
void* threadFunction(void* arg) {
    //unsigned long threadID = (unsigned long) arg;

    for (int i = 0; i < NUM_TRANSACTIONS; i++) {
        int op = rand() % 4;
        int accountID, fromID, toID;
        double amount;

        switch (op) {

            case OP_DEPOSIT:
                accountID = rand() % NUM_ACCOUNTS;
                amount = (rand() % 500) + 1;
                deposit(accountID, amount);
                break;

            case OP_WITHDRAW:
                accountID = rand() % NUM_ACCOUNTS;
                amount = (rand() % 500) + 1;
                withdraw(accountID, amount);
                break;

            case OP_CHECK:
                accountID = rand() % NUM_ACCOUNTS;
                checkBalance(accountID);
                break;

            case OP_TRANSFER:
                fromID = rand() % NUM_ACCOUNTS;
                toID   = rand() % NUM_ACCOUNTS;

                // A transferência exige que as contas sejam distintas
                while (toID == fromID) {
                    toID = rand() % NUM_ACCOUNTS;
                }

                amount = (rand() % 500) + 1;
                transfer(fromID, toID, amount);
                break;
        }
    }

    return NULL;
}

/*
 * Interpreta o argumento passado pela linha de comando.
 * Caso o usuário não informe quantidade de threads,
 * utiliza o valor padrão 4.
 */
unsigned long processCommandLine(int argc, char** argv) {
    if (argc == 2) {
        return strtoul(argv[1], NULL, 10);
    
    } else if (argc == 1) {
        return 4;
    
    } else {
        fprintf(stderr, "Uso: %s [numThreads]\n", argv[0]);
        exit(1);
    }
}

/*
 * Função principal:
 *  - Inicializa gerador de números aleatórios
 *  - Cria as threads
 *  - Aguarda finalização de todas
 *  - Exibe saldo final das contas
 *  - Libera recursos
 */
int main(int argc, char** argv) {
    unsigned long numThreads = processCommandLine(argc, argv);
    
    // Revisão: malloc aloca dinamicamente memória no heap
    pthread_t* threads = malloc(numThreads * sizeof(pthread_t));

    // Verifica se há memória suficiente
    if (!threads) {
        perror("Erro ao alocar memória para threads");
        exit(1);
    }

    // inicializa o gerador de números aleatórios 
    srand(time(NULL));

    // Criação das threads
    for (unsigned long i = 0; i < numThreads; i++) {

        // A função pthread_create retorna 0 em caso de sucesso.
        if (pthread_create(&threads[i], NULL, threadFunction, (void*) i) != 0) { 
            
            perror("Erro ao criar thread");
            free(threads);
            exit(1);
        }
    }

    // Espera por todas as threads
    for (unsigned long i = 0; i < numThreads; i++) {
        // Revisão: Pthread_join faz com que uma thread espera (bloqueie) até que outra thread específica termine sua execução
        pthread_join(threads[i], NULL);
    }

    /*
     * Impressão do saldo final de cada conta.
     * A consulta via checkBalance utiliza mutex,
     * garantindo leitura consistente mesmo após múltiplas operações concorrentes.
     */
    
    printf("\nSaldos finais das contas:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Conta %d: %.2f\n", i, checkBalance(i));
    }

    // libera a memória alocada, e seus recursos, e encerra os mutexes
    free(threads);
    cleanup();
    
    return 0;
}
