/* bankAccount.h simulates a simple bank account
 *
 * Joel Adams, Calvin College, Fall 2013.
 */
#include <pthread.h>
#include <stdio.h>

#define NUM_ACCOUNTS 5

//Variáveis Compartilhadas
//extern--> 'esta variável existe, mas será definida em outro lugar[.c]'

//Array de mutexes para exclusão mútua local por conta
extern pthread_mutex_t locks[NUM_ACCOUNTS]; 

//Array de saldos
extern double bankAccountBalances[NUM_ACCOUNTS]; 

//Protótipos de Funções
//adiciona um valor ao saldo de uma conta específica
int deposit(int accountID, double amount);

//Subtrai um valor do saldo de uma conta específica
//Retorna 1 em sucesso, 0 em falha (saldo insuficiente), -1 para conta inválida
int withdraw(int accountID, double amount);

//Consulta de Saldo com exclusao mútua
double checkBalance(int accountID);

//Destruir o mutex(cadeado) para liberar os recursos do sistema
void cleanup();

// (mirella) Transfere um valor de uma conta para outra com segurança (evita deadlock)
int transfer(int fromID, int toID, double amount);

// (Jv) writeLog: função para escrever no arquivo de log as operações realizadas
void writeLog(const char* operation, int fromID, int toID, double amount, double finalBalance);