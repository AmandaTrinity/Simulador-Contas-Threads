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

//Subtrai um valor do saldo de uma conta específica
//Retorna 1 em sucesso, 0 em falha (saldo insuficiente), -1 para conta inválida
int withdraw(int accountID, double amount) {
   int success = 0; //assume falha por padrão

   if(accountID >= 0 && accountID < NUM_ACCOUNTS) {
      pthread_mutex_lock(&locks[accountID]);
 
      if(bankAccountBalances[accountID] >= amount){
         bankAccountBalances[accountID] -= amount;
         success = 1;
      
      }
      pthread_mutex_unlock(&locks[accountID]);
   } else {
      success = -1; //Conta Inválida
   }

   return success;
}

//Consulta de Saldo com exclusao mútua
double checkBalance(int accountID) {
   double balance = -1.0;
   if(accountID >= 0 && accountID < NUM_ACCOUNTS) {
      pthread_mutex_lock(&locks[accountID]);
      balance = bankAccountBalances[accountID];
      pthread_mutex_unlock(&locks[accountID]);
   }
   return balance;
}

//Destruir o mutex(cadeado) para liberar os recursos do sistema
void cleanup() {
   int i;
   for(i = 0; i < NUM_ACCOUNTS; i++) {
      pthread_mutex_destroy(&locks[i]);
   }
}