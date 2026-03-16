#include "faturacao.h"

int main(int argc, char *argv[]) {
    Produto todosprodutos[MAXPRODUTOS];
    int total_produtos = 0;

    ItemNoCesto *cesto = NULL;
    int num_items_cesto = 0;

    Fatura *faturas = NULL;
    int num_faturas = 0, numero_proxima_fatura = 1;

    char comando;
    
    int taxas_iva[MAXIVA];
    inicializa_iva(taxas_iva);
    if (argc > 1) {
        FILE *ficheiro_iva = fopen(argv[1], "r");
        
        
        if (ficheiro_iva != NULL) {
            int valor_taxa_iva;
            char letra_iva;
            while (fscanf(ficheiro_iva, " %c %d", &letra_iva, &valor_taxa_iva) == 2) {
                taxas_iva[letra_iva - 'A'] = valor_taxa_iva;
            }
        }
        fclose(ficheiro_iva);
    }

    while (1) {
        scanf(" %c", &comando);
        switch (comando) {
        case 'q':
            for (int i = 0; i < num_faturas; i++) {
                free(faturas[i].nome_cliente);
            }
            free(faturas);
            free(cesto);
            return 0;
        
        case 'p':
            comando_p(todosprodutos, &total_produtos, taxas_iva);
            break;

        case 'l':
            comando_l(todosprodutos, total_produtos, cesto, num_items_cesto);
            break;

        case 'a':
            comando_a(todosprodutos, total_produtos, &cesto, &num_items_cesto, taxas_iva);
            break;

        case 'r':
            comando_r(todosprodutos, total_produtos, faturas, num_faturas, taxas_iva);
            break;

        case 'f':
            comando_f(todosprodutos, &cesto, &num_items_cesto, &faturas, &num_faturas, &numero_proxima_fatura, taxas_iva);
            break;

        case 'c':
            comando_c(faturas, num_faturas);
            break;
        
        case 'd':
            comando_d(todosprodutos, &total_produtos, &faturas, &num_faturas, cesto, num_items_cesto);
            break;
        
        default:
            break;
        }
    }
    return 0;
}