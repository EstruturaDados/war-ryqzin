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

/* atribui uma missao sorteada para 'destino' (destino deve ter espaço alocado) */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    if (totalMissoes <= 0) return;
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

/* exibe missao (por valor) */
void exibirMissao(const char* missao) {
    printf("Sua missao: %s\n\n", missao);
}

/* verifica se missao foi cumprida
   missao: string descritiva
   mapa: ponteiro para vetor de territorios
   tamanho: quantidade de territorios
   corJogador: cor/dono do jogador cuja missao sera verificada
   retorna 1 se cumprida, 0 caso contrario
   Implementa verificacoes simples para as missoes pre-definidas.
*/
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador) {
    if (!missao || !mapa || !corJogador) return 0;

    /* Missao 1: Conquistar 3 territorios seguidos */
    if (strcmp(missao, "Conquistar 3 territorios seguidos") == 0) {
        for (int i = 0; i <= tamanho - 3; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0 &&
                strcmp(mapa[i+1].cor, corJogador) == 0 &&
                strcmp(mapa[i+2].cor, corJogador) == 0) {
                return 1;
            }
        }
        return 0;
    }

    /* Missao 2: Controlar 5 territorios */
    if (strcmp(missao, "Controlar 5 territorios") == 0) {
        int cont = 0;
        for (int i = 0; i < tamanho; i++) if (strcmp(mapa[i].cor, corJogador) == 0) cont++;
        return (cont >= 5) ? 1 : 0;
    }

    /* Missao 3: Eliminar todas as tropas de uma cor adversaria
       -> Verifica se existe alguma cor diferente cuja soma de tropas == 0 */
    if (strcmp(missao, "Eliminar todas as tropas de uma cor adversaria") == 0) {
        /* descobrir cores presentes diferentes de corJogador */
        for (int i = 0; i < tamanho; i++) {
            const char *cor = mapa[i].cor;
            if (strcmp(cor, corJogador) == 0) continue;
            /* checar se essa cor tem tropas */
            int soma = 0;
            for (int j = 0; j < tamanho; j++) {
                if (strcmp(mapa[j].cor, cor) == 0) soma += mapa[j].tropas;
            }
            if (soma == 0) return 1; /* uma cor adversaria foi eliminada */
        }
        return 0;
    }

    /* Missao 4: Controlar todos os territorios */
    if (strcmp(missao, "Controlar todos os territorios") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) != 0) return 0;
        }
        return 1;
    }

    /* Missao 5: Controlar 2 territorios seguidos (fallback simples) */
    if (strcmp(missao, "Controlar 2 territorios seguidos") == 0) {
        for (int i = 0; i <= tamanho - 2; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0 &&
                strcmp(mapa[i+1].cor, corJogador) == 0) {
                return 1;
            }
        }
        return 0;
    }

    /* por padrão, nao cumprida */
    return 0;
}

/* libera memoria alocada */
void liberarMemoria(Territorio *mapa, char* missao1, char* missao2) {
    free(mapa);
    free(missao1);
    free(missao2);
}

int main(void) {
    srand((unsigned)time(NULL));

    /* vetor de missoes pre-definidas */
    char *missoesDisponiveis[] = {
        "Conquistar 3 territorios seguidos",
        "Controlar 5 territorios",
        "Eliminar todas as tropas de uma cor adversaria",
        "Controlar todos os territorios",
        "Controlar 2 territorios seguidos"
    };
    int totalMissoes = sizeof(missoesDisponiveis) / sizeof(missoesDisponiveis[0]);

    int n = lerInt("Quantos territorios deseja cadastrar? ");
    if (n <= 0) {
        printf("Numero de territorios invalido.\n");
        return 0;
    }

    Territorio *mapa = alocarTerritorios(n);
    cadastrarTerritorios(mapa, n);

    /* solicitar cores dos dois jogadores (usar para vincular missoes) */
    char corJogador1[COR];
    char corJogador2[COR];
    lerString(corJogador1, COR, "Informe a cor do Jogador 1: ");
    lerString(corJogador2, COR, "Informe a cor do Jogador 2: ");
    printf("\n");

    /* alocar dinamicamente missao de cada jogador e atribuir aleatoriamente */
    char *missaoJog1 = malloc(256);
    char *missaoJog2 = malloc(256);
    if (!missaoJog1 || !missaoJog2) {
        printf("Erro ao alocar memoria para missoes.\n");
        free(mapa);
        free(missaoJog1);
        free(missaoJog2);
        return 1;
    }
    atribuirMissao(missaoJog1, missoesDisponiveis, totalMissoes);
    atribuirMissao(missaoJog2, missoesDisponiveis, totalMissoes);

    /* exibir missao apenas uma vez no inicio para cada jogador */
    printf("Jogador 1 (cor %s):\n", corJogador1);
    exibirMissao(missaoJog1);
    printf("Jogador 2 (cor %s):\n", corJogador2);
    exibirMissao(missaoJog2);

    int opc;
    int jogoAtivo = 1;
    do {
        printf("=== MENU ===\n");
        printf("1. Listar territorios\n");
        printf("2. Ataque entre territorios\n");
        printf("3. Sair\n");
        printf("4. Verificar missões (manual)\n");
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

            /* apos ataque, verificar se algum jogador cumpriu sua missao */
            if (verificarMissao(missaoJog1, mapa, n, corJogador1)) {
                printf("=== MISSÃO CUMPRIDA ===\n");
                printf("Jogador 1 (cor %s) cumpriu a missao: %s\n", corJogador1, missaoJog1);
                jogoAtivo = 0;
                break;
            }
            if (verificarMissao(missaoJog2, mapa, n, corJogador2)) {
                printf("=== MISSÃO CUMPRIDA ===\n");
                printf("Jogador 2 (cor %s) cumpriu a missao: %s\n", corJogador2, missaoJog2);
                jogoAtivo = 0;
                break;
            }
        } else if (opc == 4) {
            /* opção extra: verificar missões manualmente */
            int j1 = verificarMissao(missaoJog1, mapa, n, corJogador1);
            int j2 = verificarMissao(missaoJog2, mapa, n, corJogador2);

            printf("\nStatus das missoes:\n");
            printf("Jogador 1 (cor %s) - Missao: %s -> %s\n",
                   corJogador1, missaoJog1, j1 ? "CUMPRIDA" : "NAO CUMPRIDA");
            printf("Jogador 2 (cor %s) - Missao: %s -> %s\n\n",
                   corJogador2, missaoJog2, j2 ? "CUMPRIDA" : "NAO CUMPRIDA");

            if (j1 && !j2) {
                printf("Jogador 1 venceu por missao cumprida.\n");
                jogoAtivo = 0;
                break;
            } else if (j2 && !j1) {
                printf("Jogador 2 venceu por missao cumprida.\n");
                jogoAtivo = 0;
                break;
            } else if (j1 && j2) {
                printf("Ambos os jogadores cumpriram suas missoes.\n");
                jogoAtivo = 0;
                break;
            }
        } else if (opc == 3) {
            printf("Saindo...\n");
            jogoAtivo = 0;
        } else {
            printf("Opcao invalida.\n");
        }
    } while (jogoAtivo);

    liberarMemoria(mapa, missaoJog1, missaoJog2);
    return 0;
}