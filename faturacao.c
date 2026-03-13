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

    if (!((i == 13 || i == 8) && ean[i] == '\0'))
        return 0;
    
    for(int j = 0; j < i - 1; j++) {
        num = ean[j] - '0';
        
        if(j % 2 == 0)
            soma += num;
        else
            soma += num * 3;
    }

    digito_de_verificacao = (10 - (soma % 10)) % 10;
    return digito_de_verificacao == ean[i - 1] - '0';
}



void comando_p(Sistema *sistema) {
    Produto produto;
    char verificacao_descricao[MAXLINHA];
    scanf("%s %c %lf %d %[^\n]", produto.ean, &produto.iva, &produto.preco, &produto.stock, verificacao_descricao);
    int indice_letra_liva = produto.iva - 'A';
    
    if (!(verifica_ean(produto.ean))) {
        printf("invalid ean\n");
        return;
    }
    if (!(sistema->taxas_iva[indice_letra_liva] >= 0)) {
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
    if (strlen(verificacao_descricao) > 50 || !(isupper(verificacao_descricao[0]))) {
        printf("invalid description\n");
        return;
    }
    else
        strcpy(produto.descricao, verificacao_descricao);

    for(int i = 0; i < sistema->total_produtos; i++) {
        if (strcmp(sistema->todosprodutos[i].ean, produto.ean) == 0) {
            sistema->todosprodutos[i].stock += produto.stock;
            sistema->todosprodutos[i].iva = produto.iva;
            
            /*
            if (sistema->todosprodutos[i].preco != produto.preco) {
                printf("product in use\n");
                return;
            } 
            */
            
            sistema->todosprodutos[i].preco = produto.preco;
            printf("%d\n", sistema->todosprodutos[i].stock);
            return;
        }
    }

    if (sistema->total_produtos >= MAXPRODUTOS){
        printf("invalid product\n");
        return;
    }

    produto.vendidos = 0;
    sistema->todosprodutos[sistema->total_produtos] = produto;
    sistema->todosprodutos[sistema->total_produtos].numero = sistema->total_produtos;
    (sistema->total_produtos)++;
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



void comando_l(Sistema *sistema) {
    char arg[MAXLINHA];
    char *palavra;
    int encontrou = 0, encontrou_prod_com_stock = 0;

    fgets(arg, MAXLINHA, stdin);
    palavra = strtok(arg, " \n");
    
    if (palavra == NULL || strcmp(palavra, "*") == 0) {
        for (int i = 0; i < sistema->total_produtos; i++) {
            if(sistema->todosprodutos[i].stock > 0) {
                printf("%s %c %.2lf %d %d %s\n",
                    sistema->todosprodutos[i].ean,
                    sistema->todosprodutos[i].iva,
                    sistema->todosprodutos[i].preco,
                    sistema->todosprodutos[i].vendidos,
                    sistema->todosprodutos[i].stock,
                    sistema->todosprodutos[i].descricao);
                encontrou_prod_com_stock = 1;
            }
        }
        if (!(encontrou_prod_com_stock))
            printf("*: no such product\n");
    } else {
        while (palavra != NULL) {
            encontrou = 0;
            for (int i = 0; i < sistema->total_produtos; i++) {
                if (sistema->todosprodutos[i].stock > 0 && verifica_wildcard(palavra, sistema->todosprodutos[i].ean)) {
                    printf("%s %c %.2lf %d %d %s\n",
                        sistema->todosprodutos[i].ean,
                        sistema->todosprodutos[i].iva,
                        sistema->todosprodutos[i].preco,
                        sistema->todosprodutos[i].vendidos,
                        sistema->todosprodutos[i].stock,
                        sistema->todosprodutos[i].descricao);
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


void comando_a(Produto todosprodutos[MAXPRODUTOS], int total_produtos, int taxas_iva[MAXIVA]) {
    char arg[MAXLINHA];
    int quantidade = 1, encontrou_prod_com_ean = 0, indice_do_produto = 0;
    char ean_prod_cesto[MAXEAN];

    fgets(arg, MAXLINHA, stdin);
    if (sscanf(arg, "%d %s", &quantidade, ean_prod_cesto) != 2) {
        sscanf(arg, "%s", ean_prod_cesto);
    }

    if (!(verifica_ean(ean_prod_cesto))) {
        printf("invalid ean\n");
        return;
    }

    for (int i = 0; i < total_produtos; i++) {
        if (!(strcmp(sistema->todosprodutos[i].ean, ean_prod_cesto))) {
            encontrou_prod_com_ean = 1;
            indice_do_produto = i;
            break;
        }
    }
    
    if (!(encontrou_prod_com_ean)) {                //ver se dá mal por ordem dos erros
        printf("%s: no such product\n", ean_prod_cesto);
        return;
    }
    if (quantidade < 0 && /* falta aqui alguma coisa */ + quantidade < 0) {
        printf("invalid quantity\n");
        return;
    }
    if (quantidade > 0 && todosprodutos[indice_do_produto].stock < quantidade) {
        printf("no stock\n");
        return;
    } else {
        /* falta aqui alguma coisa */ += quantidade;
        todosprodutos[indice_do_produto].stock -= quantidade;
    }

    int valor_iva = taxas_iva[todosprodutos[indice_do_produto].iva - 'A'];
    double preco_com_iva, total_no_cesto;

    preco_com_iva = todosprodutos[indice_do_produto].preco * (1 + valor_iva/ 100);
    total_no_cesto = preco_com_iva * /* falta aqui alguma coisa */;

    double total_final = (long long)(total_no_cesto * 100 + 0.5) / 100.0;

    printf("%c %.2lf %d %.2lf %s",
        todosprodutos[indice_do_produto].iva,
        todosprodutos[indice_do_produto].preco,
        /* falta aqui alguma coisa */,
        total_final,
        todosprodutos[indice_do_produto].descricao);
}