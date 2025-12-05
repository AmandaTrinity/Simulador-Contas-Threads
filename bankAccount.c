/* bankAccount.h simulates a simple bank account
 *
 * Joel Adams, Calvin College, Fall 2013.
 */
#include <pthread.h>
#include <stdio.h>
#include <time.h>

void writeLog(const char* operation, int fromID, int toID, double amount, double finalBalance);

#define NUM_ACCOUNTS 5 //numero de conta

// Shared Variables --> array de mutexes, é um para cada conta
pthread_mutex_t locks[NUM_ACCOUNTS] = {PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER}; //variavel de exclusao mutua inicializada

//! mutex para proteger a escrita no arquivo de bankAccount_log.txt (Jv)
//? OBS.: se multiplas threads tentarem escrever no arquivo ao mesmo tempo, pode corromper ou perder os dados. por isso, eu decidi colocar um mutex exclusivo para proteger a escrita no arquivo de log.
pthread_mutex_t logLock = PTHREAD_MUTEX_INITIALIZER;

//Array de saldos, um para cada conta                                       
double bankAccountBalances[NUM_ACCOUNTS] = {0.0};//guarda o saldo da conta --> é o nosso recurso compartilhado

//Adiciona um valor ao saldo de uma conta específica
int deposit(int accountID, double amount) {

   // validar parâmetros
   if (amount <= 0){
      writeLog("FALHA_DEPOSITO_VALOR_INVALIDO", accountID, -1, amount, -1); // integração com log para caso de valor inválido no depósito (Jv)

      return -1; // valor inválido (Jv)
   }

   if (accountID < 0 || accountID >= NUM_ACCOUNTS){
      writeLog("FALHA_DEPOSITO_CONTA_INVALIDA", accountID, -1, amount, -1); // integração com log para caso de conta inválida no depósito (Jv)

      return -1; // conta inválida (Jv)

   }
   else {
      pthread_mutex_lock(&locks[accountID]);
      bankAccountBalances[accountID] += amount;

      writeLog("DEPOSITO", accountID, -1, amount, bankAccountBalances[accountID]); // integração com log para sucesso no depósito (Jv)

      pthread_mutex_unlock(&locks[accountID]);

      return 1; //sucesso
   }
}

//Subtrai um valor do saldo de uma conta específica
//Retorna 1 em sucesso, 0 em falha (saldo insuficiente), -1 para conta inválida
int withdraw(int accountID, double amount) {
   int success = 0; //assume falha por padrão

   if (amount <= 0){
      writeLog("FALHA_SAQUE_VALOR_INVALIDO", accountID, -1, amount, -1); // integração com log para caso de valor inválido no saque (Jv)

      return -1; // valor inválido
   }

   else if(accountID >= 0 && accountID < NUM_ACCOUNTS) {
      pthread_mutex_lock(&locks[accountID]);

      if(bankAccountBalances[accountID] >= amount){
         bankAccountBalances[accountID] -= amount;

         writeLog("SAQUE", accountID, -1, amount, bankAccountBalances[accountID]); // integração com log para sucesso no saque(Jv)

         success = 1;
      
      } else {
         writeLog("FALHA_SAQUE_SALDO_INSUFICIENTE", accountID, -1, amount, bankAccountBalances[accountID]); // integração com log para caso de saldo insuficiente na conta (Jv)

         success = 0; // saldo insuficiente (Jv)
      }

      pthread_mutex_unlock(&locks[accountID]);
   
   } else {
      writeLog("FALHA_SAQUE_CONTA_INVALIDA", accountID, -1, amount, -1); // integração com log para caso de conta inválida (Jv)

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

      writeLog("CONSULTA", accountID, -1, 0.0, balance); // integração com log caso amount seja 0 (Jv)

      pthread_mutex_unlock(&locks[accountID]);
   } else {
      writeLog("FALHA_CONSULTA_CONTA_INVALIDA", accountID, -1, 0.0, -1); // integração com log para caso de conta inválida na consulta (Jv)
   }

   return balance;
}

//Destruir o mutex (cadeado) para liberar os recursos do sistema, não chama o writeLog pois é chamado ao final da execução para destruir os mutexes
void cleanup() {
   int i;

   for(i = 0; i < NUM_ACCOUNTS; i++) {
      pthread_mutex_destroy(&locks[i]);
   }
}

// (mirella) Transfere valor entre contas. Retorna 1 se sucesso, 0 se falha e -1 para parâmetros inválidos
// Transfere valor entre contas.
int transfer(int fromID, int toID, double amount) {
   
   //1. Validação de Parâmetros
   if (amount <= 0) {
      writeLog("FALHA_TRANSFERENCIA_VALOR_INVALIDO", fromID, toID, amount, -1); // integração com log para caso de valor inválido na transferência (Jv)
      
      return -1; // Valor inválido
   }

   if (fromID < 0 || fromID >= NUM_ACCOUNTS || toID < 0 || toID >= NUM_ACCOUNTS) {
      writeLog("FALHA_TRANSFERENCIA_CONTA_INVALIDA", fromID, toID, amount, -1); // integração com log para caso de conta inválida na transferência (Jv)

      return -1; // Conta inválida
   }

   if (fromID == toID) {
      writeLog("FALHA_TRANSFERENCIA_MESMA_CONTA", fromID, toID, amount, -1); // integração com log para caso de mesma conta na transferência (Jv)

      return -1; // Mesma conta
   }

   if (bankAccountBalances[fromID] < amount) {
      writeLog("FALHA_TRANSFERENCIA_SALDO_INSUFICIENTE", fromID, toID, amount, bankAccountBalances[fromID]); // integração com log para caso de saldo insuficiente na transferência (Jv)

      return 0; // Saldo insuficiente
   }

   // 2. Prevenção de Deadlock: Ordenar os bloqueios
   //* Revisão: O Deadlock acontece quando duas ou mais threads tentam bloquear os mesmos recursos em ordens opostas. Ex:
   // Thread 1(A --> B): bloqueia a conta A e tenta bloquear a conta B
   // troca de contexto
   // Thread 2(B --> A): bloqueia a conta B e tenta bloquear a conta A
   
   // Travar sempre a conta com menor ID primeiro. Isso ajuda na quebra da 'espera circular'
   int firstLock = (fromID < toID) ? fromID : toID;
   int secondLock = (fromID < toID) ? toID : fromID;

   // 3. Bloquear os recursos (Mutexes)
   pthread_mutex_lock(&locks[firstLock]);
   pthread_mutex_lock(&locks[secondLock]);

   int success = 0;

   // 4. Realizar a transferência se houver saldo após uma reavaliação (boa prática)
   if (bankAccountBalances[fromID] >= amount) {
      bankAccountBalances[fromID] -= amount;
      bankAccountBalances[toID] += amount;
      success = 1;

      writeLog("TRANSFERENCIA", fromID, toID, amount, bankAccountBalances[fromID]); // integração com log para caso de sucesso na transferencia (Jv)

   } else {
      writeLog("FALHA_TRANSFERENCIA_SALDO_INSUFICIENTE", fromID, toID, amount, bankAccountBalances[fromID]); // integração com log para caso de saldo insuficiente na tranferencia (Jv)
   }

   // 5. Desbloquear (na ordem inversa, boa prática)
   pthread_mutex_unlock(&locks[secondLock]);
   pthread_mutex_unlock(&locks[firstLock]);

   return success;
}

/* (Jv) função writeLog

   ! responsável por escrever no arquivo de log todas as operações realizadas sobre contas bancárias.

   ? operation: nome da operação (ex.: DEPOSITO, SAQUE, TRANSFERENCIA, CONSULTA)
   ? fromID: ID da conta de origem
   ? toID: ID da conta de destino (ou -1 caso não exista)
   ? amount: valor envolvido na operação
   ? finalBalance: saldo final da conta de origem após a operação

   ! a função abre o arquivo "bankAccount_log.txt" em mode append e escreve:
   !- horário da operação
   !- tipo da operação
   !- Contas envolvidas na operação
   !- valor movimentado
   !- saldo final (o saldo final é apenas da conta de origem)

   *Obs1.: esta função é chamada após cada operação bancária, e pode ser protegida por mutex caso haja escrita simultânea por múltiplas threads.

   *Obs2.: a variável operation é definida como ponteiro, pois strings é representada como array de caracteres em C, e como eu só quero ler a operação e não modificar o texo, uso o 'const char*' evitando cópias desnecessárias.
*/
void writeLog(const char* operation, int fromID, int toID, double amount, double finalBalance){

   pthread_mutex_lock(&logLock); // protege a escrita no arquivo de log

   FILE* f = fopen("bankAccount_log.txt", "a");

   if (f == NULL) {
      pthread_mutex_unlock(&logLock);
      return; // falha ao abrir o arquivo de log 
   }

   time_t now = time(NULL);
   struct tm* t = localtime(&now);

   fprintf(f, "[%02d:%02d:%02d] OP = %s | Origem = %d | Destino = %d | Valor = %.2f | SaldoFinal = %.2f \n", t->tm_hour, t->tm_min, t->tm_sec, operation, fromID, toID, amount, finalBalance);

   fclose(f);

   pthread_mutex_unlock(&logLock); // libera o mutex após a escrita
}
