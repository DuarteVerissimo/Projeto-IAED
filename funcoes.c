#include "estruturas.h"

int verifica_ean(char ean[MAXEAN]) {
    int i = 0, soma = 0, num = 0, digito_de_verificacao = 0;
    
    while(ean[i] != '\0' && ean[i] >= '0' && ean[i] <= '9') {
        i++;
    }
    if (!((i == 13 || i == 8) && ean[i] == '\0'))
        return 0;
    
    for(int j=0; j < i - 1; j++) {
        num = ean[j] - '0';
        
        if(j % 2 == 0)
            soma += num;
        else
            soma += num*3;
    }

    digito_de_verificacao = (10 - (soma % 10)) % 10;

    return digito_de_verificacao == ean[i - 1] - '0';
}

void comando_p(Produto todosprodutos[MAXPRODUTOS], int *ptotal_produtos) {
    Produto produto;
    char verificacao_descricao[MAXLINHA];
    scanf("%s %c %lf %d %[^\n]", produto.ean, &produto.iva, &produto.preco, &produto.stock, verificacao_descricao);

    if (!(verifica_ean(produto.ean))) {
        printf("invalid ean\n");
        return;
    }
    if (!(produto.iva >= 'A' && produto.iva <= 'Z')) {
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
            todosprodutos[i].preco = produto.preco;
            printf("%d\n", todosprodutos[i].stock);
            return;
        }
    }

    if (*ptotal_produtos >= MAXPRODUTOS){
        printf("invalid product\n");
        return;
    }

    todosprodutos[*ptotal_produtos] = produto;
    todosprodutos[*ptotal_produtos].numero = *ptotal_produtos;
    (*ptotal_produtos)++;
    printf("%d\n", produto.stock);
    return;
}