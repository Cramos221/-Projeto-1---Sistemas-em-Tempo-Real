# -Projeto-1 Sistemas em Tempo-Real
# Estacionamento Inteligente v1.0

Um simulador de estacionamento multithread desenvolvido em C utilizando a biblioteca POSIX Threads (`pthreads`). O sistema simula a chegada, alocação e saída de veículos, gerenciando vagas comuns e preferenciais (PCD) de forma sincronizada e com uma interface visual no terminal.

## Funcionalidades e Arquitetura

* **Simulação Concorrente**: Utiliza threads independentes para representar cada carro no sistema e uma thread central dedicada ao Controlador do estacionamento.
* **Gerenciamento de Vagas**: O estacionamento possui um total de 10 vagas, sendo 8 vagas comuns e 2 vagas exclusivas para PCD. O sistema é dimensionado para simular um fluxo total de 20 carros.
* **Sincronização Segura**: A implementação controla a concorrência utilizando semáforos (`sem_t`), mutexes (`pthread_mutex_t`) e variáveis de condição (`pthread_cond_t`) para garantir que a alocação de vagas seja feita com segurança.
* **Estruturas de Dados**: Conta com uma fila customizada (`Fila`) protegida por mutex para gerenciar as requisições ordenadas de entrada e saída dos veículos.
* **Interface Visual (CLI)**: Apresenta logs com cores ANSI e um mapa de vagas dinâmico, exibindo a ocupação em tempo real e uma barra de progresso visual.
* **Relatório Estatístico**: Ao final de toda a simulação, o sistema gera um relatório detalhando o fluxo de veículos, os tempos (máximo, mínimo e médio) de permanência e a quantidade de uso por cada vaga.

## Compilação e Execução

O projeto inclui um `Makefile` já configurado para facilitar a compilação via `gcc`. 

**Para compilar o projeto:**
```bash
make all
```
## Para executar o simulador:

```bash
./estacionamento
```
## Para limpar os arquivos compilados (binário):

```bash
make clean
```

