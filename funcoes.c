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

int comando_p() {
    Produto produto;
    scanf("%s %c %lf %d %[^\n]", produto.ean, &produto.iva, &produto.preco, &produto.stock, produto.descricao);

}