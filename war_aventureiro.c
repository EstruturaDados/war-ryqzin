#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NOME 30
#define COR 10

typedef struct Territorio {
    char nome[NOME];
    char cor[COR];
    int tropas;
} Territorio;

/* lê uma linha e remove '\n' */
void lerString(char *buf, size_t size, const char *prompt) {
    printf("%s", prompt);
    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* lê inteiro seguro usando fgets + strtol */
int lerInt(const char *prompt) {
    char buf[64];
    long val;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof buf, stdin)) return 0;
        char *end;
        val = strtol(buf, &end, 10);
        if (end != buf) return (int)val;
        printf("Entrada invalida. Tente novamente.\n");
    }
}

/* aloca dinamicamente o vetor de territorios */
Territorio *alocarTerritorios(int n) {
    Territorio *mapa = calloc((size_t)n, sizeof(Territorio));
    if (!mapa) {
        printf("Erro ao alocar memoria para %d territorios\n", n);
        exit(1);
    }
    return mapa;
}

/* cadastra territorios usando ponteiros */
void cadastrarTerritorios(Territorio *mapa, int n) {
    for (int i = 0; i < n; i++) {
        Territorio *t = mapa + i;
        char prompt[80];
        snprintf(prompt, sizeof prompt, "Informe o nome do territorio %d: ", i + 1);
        lerString(t->nome, NOME, prompt);
        snprintf(prompt, sizeof prompt, "Informe a cor (dono/exercito) do territorio %d: ", i + 1);
        lerString(t->cor, COR, prompt);
        t->tropas = lerInt("Informe a quantidade de tropas (numero inteiro): ");
        if (t->tropas < 0) t->tropas = 0;
        printf("\n");
    }
}

/* exibe todos os territorios */
void listarTerritorios(Territorio *mapa, int n) {
    printf("\n--- Mapa de Territorios ---\n");
    for (int i = 0; i < n; i++) {
        Territorio *t = mapa + i;
        printf("%d) Nome: %s | Cor: %s | Tropas: %d\n", i + 1, t->nome, t->cor, t->tropas);
    }
    printf("---------------------------\n\n");
}

/* Simula o ataque: usa rand() para rolar dados (1..6)
   Regras:
   - Se atacante vencer (dado maior), defensor troca de cor para a do atacante
     e recebe metade das tropas do atacante (arredondado para baixo). As tropas
     transferidas são subtraídas do atacante.
   - Se atacante perder ou empatar, atacante perde 1 tropa.
*/
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante == NULL || defensor == NULL) return;

    int dadoA = rand() % 6 + 1;
    int dadoD = rand() % 6 + 1;

    printf("\nAtaque: %s (tropas %d, cor %s) -> %s (tropas %d, cor %s)\n",
           atacante->nome, atacante->tropas, atacante->cor,
           defensor->nome, defensor->tropas, defensor->cor);
    printf("Rolagem: atacante %d x defensor %d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        /* atacante vence */
        int transferencia = atacante->tropas / 2; /* metade das tropas */
        if (transferencia < 0) transferencia = 0;
        atacante->tropas -= transferencia;
        defensor->tropas += transferencia; /* transfere para o defensor (acrescenta) */
        /* defensor muda de cor para a do atacante */
        strncpy(defensor->cor, atacante->cor, COR - 1);
        defensor->cor[COR - 1] = '\0';

        printf("Atacante venceu! %d tropas transferidas para %s.\n", transferencia, defensor->nome);
    } else {
        /* atacante perde (inclui empate como derrota) */
        if (atacante->tropas > 0) atacante->tropas -= 1;
        printf("Atacante perdeu. %s perde 1 tropa.\n", atacante->nome);
    }

    printf("\nEstado apos o ataque:\n");
    printf("%s -> Cor: %s | Tropas: %d\n", atacante->nome, atacante->cor, atacante->tropas);
    printf("%s -> Cor: %s | Tropas: %d\n\n", defensor->nome, defensor->cor, defensor->tropas);
}

/* libera memoria alocada */
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

int main(void) {
    srand((unsigned)time(NULL));

    int n = lerInt("Quantos territorios deseja cadastrar? ");
    if (n <= 0) {
        printf("Numero de territorios invalido.\n");
        return 0;
    }

    Territorio *mapa = alocarTerritorios(n);
    cadastrarTerritorios(mapa, n);

    int opc;
    do {
        printf("=== MENU ===\n");
        printf("1. Listar territorios\n");
        printf("2. Ataque entre territorios\n");
        printf("3. Sair\n");
        opc = lerInt("Escolha uma opcao: ");

        if (opc == 1) {
            listarTerritorios(mapa, n);
        } else if (opc == 2) {
            listarTerritorios(mapa, n);
            int ia = lerInt("Escolha o numero do territorio atacante: ") - 1;
            int id = lerInt("Escolha o numero do territorio defensor: ") - 1;

            if (ia < 0 || ia >= n || id < 0 || id >= n) {
                printf("Indices invalidos. Operacao cancelada.\n\n");
                continue;
            }
            if (ia == id) {
                printf("Um territorio nao pode atacar a si mesmo.\n\n");
                continue;
            }

            Territorio *atacante = mapa + ia;
            Territorio *defensor = mapa + id;

            if (strcmp(atacante->cor, defensor->cor) == 0) {
                printf("Atencao: nao e permitido atacar um territorio da mesma cor.\n\n");
                continue;
            }
            if (atacante->tropas <= 0) {
                printf("Atencao: territorio atacante nao tem tropas suficientes.\n\n");
                continue;
            }

            atacar(atacante, defensor);
        } else if (opc == 3) {
            printf("Saindo...\n");
        } else {
            printf("Opcao invalida.\n");
        }
    } while (opc != 3);

    liberarMemoria(mapa);
    return 0;
}