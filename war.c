#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  NOME 30
#define  COR 10

struct territorio {
    char pais[NOME];
    char cor_tropas[COR];
    int quantidade_tropas;
};

int main(void) {
    struct territorio t[5];

    for (int i = 0; i < 5; i++) {
        printf("informe o nome do pais %d: ", i + 1);
        if (!fgets(t[i].pais, NOME, stdin)) return 1;
        size_t ln = strlen(t[i].pais);
        if (ln > 0 && t[i].pais[ln - 1] == '\n') t[i].pais[ln - 1] = '\0';

        /* ler cor das tropas */
        printf("informe a cor das tropas do pais %d: ", i + 1);
        if (!fgets(t[i].cor_tropas, COR, stdin)) t[i].cor_tropas[0] = '\0';
        ln = strlen(t[i].cor_tropas);
        if (ln > 0 && t[i].cor_tropas[ln - 1] == '\n') t[i].cor_tropas[ln - 1] = '\0';

        /* ler quantidade de tropas (usa fgets + strtol para evitar problemas com scanf) */
        char buf[32];
        printf("informe a quantidade de tropas do pais %d: ", i + 1);
        if (!fgets(buf, sizeof buf, stdin)) {
            t[i].quantidade_tropas = 0;
        } else {
            t[i].quantidade_tropas = (int)strtol(buf, NULL, 10);
        }
    }

    printf("\nTerritorios cadastrados:\n");
    for (int i = 0; i < 5; i++) {
        printf("Pais: %s\n", t[i].pais);
        printf("Cor das tropas: %s\n", t[i].cor_tropas);
        printf("Quantidade de tropas: %d\n", t[i].quantidade_tropas);
        printf("-------------------------\n");
    }
    return 0;
}