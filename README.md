# 🏦 Simulador de Contas Bancárias (Multithreading)

Este projeto é um simulador de sistema bancário desenvolvido em **C** que utiliza **threads (POSIX pthreads)** para realizar operações concorrentes em múltiplas contas. O objetivo principal é demonstrar o uso de mecanismos de sincronização (mutexes) para evitar condições de corrida e garantir a integridade dos dados em um ambiente multithread.

## 🚀 Funcionalidades

O sistema simula operações financeiras aleatórias realizadas por múltiplas threads simultaneamente.

- **Gerenciamento de 5 Contas:** O sistema opera com um array fixo de contas bancárias.
- **Operações Disponíveis:**
  - **Depósito:** Adiciona valores ao saldo de uma conta.
  - **Saque:** Remove valores, verificando se há saldo suficiente.
  - **Consulta de Saldo:** Leitura segura do saldo atual.
  - **Transferência:** Transfere valores entre duas contas de forma segura, evitando *deadlocks*.

- **Sistema de Log:** Registro das operações realizadas em arquivo para auditoria.
- **Sincronização:** Uso de um array de **mutexes**, onde cada conta possui seu próprio "cadeado", permitindo maior paralelismo (apenas acessos à mesma conta são bloqueados).

## 🛠️ Tecnologias Utilizadas

- **Linguagem:** C (C99).
- **Biblioteca de Threads:** `pthread.h`.
- **Compilação:** GCC via Makefile.

## 📁 Estrutura do Projeto

- `bankAccount.h`: Definição das estruturas, constantes e protótipos das funções.
- `bankAccount.c`: Implementação da lógica bancária e controle de exclusão mútua.
- `mutualExclusion.c`: Ponto de entrada do programa, gerenciamento de threads e geração de operações aleatórias.
- `Makefile`: Script para automação da compilação.

## 🔧 Como Executar

### Pré-requisitos

Certifique-se de ter o compilador `gcc` e a ferramenta `make` instalados.

### Compilação

No terminal, dentro da pasta do projeto, execute:

```bash
make
```

## ▶️ Execução

Você pode definir o número de threads que deseja criar (o padrão é 4):

```bash
./mutualExclusion [quantidade_de_threads]

```
## 🧠 Conceitos Aplicados

- **Exclusão Mútua (Mutex):** Garante que apenas uma thread altere o saldo de uma conta por vez.
- **Condição de Corrida:** O projeto foi desenhado para resolver o problema de threads acessando a mesma memória simultaneamente.
- **Prevenção de Deadlock:** A função de transferência implementa lógica para evitar que duas threads fiquem esperando uma pela outra indefinidamente.
