/* bankAccount.h simulates a simple bank account
 *
 * Joel Adams, Calvin College, Fall 2013.
 */



#include <pthread.h>
#include <stdio.h>

#define NUM_ACCOUNTS 5 //numero de conta

// Shared Variables --> array de mutexes, é um para cada conta
pthread_mutex_t locks[NUM_ACCOUNTS] = {PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER
                                       ,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER}; //variavel de exclusao mutua inicializada

//Array de saldos, um para cada conta                                       
double bankAccountBalances[NUM_ACCOUNTS] = {0.0};//guarda o saldo da conta --> é o nosso recurso compartilhado

//Adiciona um valor ao saldo de uma conta específica
void deposit(int accountID, double amount) {
   if(accountID >= 0 && accountID < NUM_ACCOUNTS) {
      pthread_mutex_lock(&locks[accountID]);
      bankAccountBalances[accountID] += amount;
      pthread_mutex_unlock(&locks[accountID]);
   }
}

// subtract amount from bankAccountBalance
void withdraw(double amount) {
  // pthread_mutex_lock( &lock );
   bankAccountBalance -= amount;
  // pthread_mutex_unlock( &lock );
}

void cleanup() {
   pthread_mutex_destroy(&lock);
}

