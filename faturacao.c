#include "faturacao.h"

void inicializa_iva(int taxas_iva[MAXIVA]) {
    taxas_iva[0] = 0;
    taxas_iva[1] = 6;
    taxas_iva[2] = 13;
    taxas_iva[3] = 23;

    for(int i = 4; i < MAXIVA; i++) {
        taxas_iva[i] = -1;
    }
}

int verifica_ean(char ean[MAXEAN]) {
    int i = 0, soma = 0, num = 0, digito_de_verificacao = 0;
    
    while(ean[i] != '\0' && ean[i] >= '0' && ean[i] <= '9') {
        i++;
    }

    if (i == 8 && ean[i] == '\0') {
        for (int j = 0; j < i - 1; j++) {
            num = ean[j] - '0';
            
            if (j % 2 == 0) {
                soma += num * 3;
            } else {
                soma += num;
            }
        }
    } else if (i == 13 && ean[i] == '\0') {
        for (int j = 0; j < i - 1; j++) {
            num = ean[j] - '0';

            if (j % 2 == 0) {
                soma += num;
            } else {
                soma += num * 3;
            }
        }
    } else {
        return 0;
    }

    digito_de_verificacao = (10 - (soma % 10)) % 10;

    return digito_de_verificacao == ean[i - 1] - '0';
}

void comando_p(Produto todosprodutos[MAXPRODUTOS], int *ptotal_produtos, int taxas_iva[MAXIVA]) {
    Produto produto;
    char verificacao_descricao[MAXLINHA];
    scanf("%s %c %lf %d %[^\n]", produto.ean, &produto.iva, &produto.preco, &produto.stock, verificacao_descricao);
    int indice_letra_liva = produto.iva - 'A';
    
    if (!(verifica_ean(produto.ean))) {
        printf("invalid ean\n");
        return;
    }
    if (!(taxas_iva[indice_letra_liva] >= 0)) {
        printf("invalid iva\n");
        return;
    }
    if (produto.preco <= 0) {
        printf("invalid price\n");
        return;
    }
    if (produto.stock < 0) {
        printf("invalid quantity\n");
        return;
    }
    if (strlen(verificacao_descricao) > 50) {
        printf("invalid description\n");
        return;
    }
    else
        strcpy(produto.descricao, verificacao_descricao);

    for(int i = 0; i < *ptotal_produtos; i++) {
        if (strcmp(todosprodutos[i].ean, produto.ean) == 0) {
            todosprodutos[i].stock += produto.stock;
            todosprodutos[i].iva = produto.iva;
            
            /*
            if (todosprodutos[i].preco != produto.preco) {
                printf("product in use\n");
                return;
            } 
            */
            
            todosprodutos[i].preco = produto.preco;
            printf("%d\n", todosprodutos[i].stock);
            return;
        }
    }

    if (*ptotal_produtos >= MAXPRODUTOS){
        printf("invalid product\n");
        return;
    }

    produto.vendidos = 0;
    todosprodutos[*ptotal_produtos] = produto;
    todosprodutos[*ptotal_produtos].numero = *ptotal_produtos;
    (*ptotal_produtos)++;
    printf("%d\n", produto.stock);
    return;
}

int verifica_wildcard(char *codigo, char *ean) {
    if (codigo[0] == '\0' && ean[0] == '\0'){
        return 1;
    }

    else if ((codigo[0] == '?' && ean[0] != '\0') || codigo[0] == ean[0]) {
        return verifica_wildcard(codigo + 1, ean + 1);
    }

    else if (codigo[0] == '*'){
        return verifica_wildcard(codigo+ 1, ean) || (ean[0] != '\0' && verifica_wildcard(codigo, ean + 1));
    }

    else {
        return 0;
    }
}

void comando_l(Produto todosprodutos[MAXPRODUTOS], int total_produtos) {
    char arg[MAXLINHA];
    char *palavra;
    int encontrou = 0;

    fgets(arg, MAXLINHA, stdin);
    palavra = strtok(arg, " \n");
    
    if (palavra == NULL || strcmp(palavra, "*") == 0) {
        for (int i = 0; i < total_produtos; i++) {
            if(todosprodutos[i].stock > 0) {
                printf("%s %c %.2lf %d %d %s\n",
                    todosprodutos[i].ean,
                    todosprodutos[i].iva,
                    todosprodutos[i].preco,
                    todosprodutos[i].vendidos,
                    todosprodutos[i].stock,
                    todosprodutos[i].descricao);
            }
        }
    } else {
        while (palavra != NULL) {
            encontrou = 0;
            for (int i = 0; i < total_produtos; i++) {
                if (todosprodutos[i].stock > 0 && verifica_wildcard(palavra, todosprodutos[i].ean)) {
                    printf("%s %c %.2lf %d %d %s\n",
                        todosprodutos[i].ean,
                        todosprodutos[i].iva,
                        todosprodutos[i].preco,
                        todosprodutos[i].vendidos,
                        todosprodutos[i].stock,
                        todosprodutos[i].descricao);
                    encontrou = 1;
                }
            }
            if (encontrou == 0) {
                printf("%s: no such product\n", palavra);
            }

            palavra = strtok(NULL, " \n");
        }
    }
}