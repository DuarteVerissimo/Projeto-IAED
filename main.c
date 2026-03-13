#include "faturacao.h"

int main(int argc, char *argv[]) {
    Sistema sistema;
    sistema.total_produtos = 0;
    sistema.total_faturas = 0;
    sistema.faturas = NULL;
    sistema.fatura_ativa = -1;
    char comando;

    inicializa_iva(sistema.taxas_iva);
    if (argc > 1) {
        FILE *ficheiro_iva = fopen(argv[1], "r");
        
        if (ficheiro_iva != NULL) {
            int valor_taxa_iva;
            char letra_iva;
            while (fscanf(ficheiro_iva, " %c %d", &letra_iva, &valor_taxa_iva) == 2) {
                sistema.taxas_iva[letra_iva - 'A'] = valor_taxa_iva;
            }
        }
        fclose(ficheiro_iva);
    }

    while (1) {
        scanf(" %c", &comando);
        switch (comando) {
        case 'q':
            return 0;
        
        case 'p':
            comando_p(&sistema);
            break;

        case 'l':
            comando_l(&sistema);
            break;

        case 'a':
        
        }
    }
    return 0;
}