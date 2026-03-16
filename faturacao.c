#include "faturacao.h"

void inicializa_iva(int taxas_iva[MAXIVA]) {
    taxas_iva['A' - 'A'] = 0;
    taxas_iva['B' - 'A'] = 6;
    taxas_iva['C' - 'A'] = 13;
    taxas_iva['D' - 'A'] = 23;

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
    if (strlen(verificacao_descricao) > 50 || !(isupper(verificacao_descricao[0]))) {
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

void comando_l(Produto todosprodutos[MAXPRODUTOS], int total_produtos, ItemNoCesto *cesto, int num_items) {
    char arg[MAXLINHA];
    char *palavra;
    int encontrou = 0, encontrou_prod_com_stock = 0, num_vendidos_nocesto = 0;

    fgets(arg, MAXLINHA, stdin);
    palavra = strtok(arg, " \n");
    
    if (palavra == NULL || strcmp(palavra, "*") == 0) {
        for (int i = 0; i < total_produtos; i++) {
            num_vendidos_nocesto = 0;
            if(todosprodutos[i].stock > 0) {
                for (int j = 0; j < num_items; j++) {
                    if (cesto[j].indice_produto == i) {
                        num_vendidos_nocesto = todosprodutos[i].vendidos + cesto[j].quantidade;
                    }
                }

                printf("%s %c %.2lf %d %d %s\n",
                    todosprodutos[i].ean,
                    todosprodutos[i].iva,
                    todosprodutos[i].preco,
                    num_vendidos_nocesto,
                    todosprodutos[i].stock,
                    todosprodutos[i].descricao);
                encontrou_prod_com_stock = 1;
            }
        }
        if (!(encontrou_prod_com_stock))
            printf("*: no such product\n");
    } else {
        while (palavra != NULL) {
            encontrou = 0;
            num_vendidos_nocesto = 0;
            for (int i = 0; i < total_produtos; i++) {
                if (todosprodutos[i].stock > 0 && verifica_wildcard(palavra, todosprodutos[i].ean)) {
                    for (int j = 0; j < num_items; j++) {
                        if (cesto[j].indice_produto == i) {
                            num_vendidos_nocesto = todosprodutos[i].vendidos + cesto[j].quantidade;
                        }
                    }
                    
                    printf("%s %c %.2lf %d %d %s\n",
                        todosprodutos[i].ean,
                        todosprodutos[i].iva,
                        todosprodutos[i].preco,
                        num_vendidos_nocesto,
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

void comando_a(Produto todosprodutos[MAXPRODUTOS], int total_produtos, ItemNoCesto **cesto, int *num_items, int taxas_iva[MAXIVA]) {
    char arg[MAXLINHA], ean_produto[MAXEAN];
    int quantidade = 1, indice_cesto = -1, indice_produto = -1;

    fgets(arg, MAXLINHA, stdin);
    int arg_lidos = sscanf(arg, "%d %s", &quantidade, ean_produto);
    
    if (arg_lidos == 1) {
        sscanf(arg, "%s", ean_produto);
        quantidade = 1;
    } else if (arg_lidos <= 0) {
        for (int i = 0; i < *num_items; i++) {
            int done = 1;
            for (int j = 0; j < *num_items - 1 - i; j++) {
                if (strcmp((*cesto)[j].ean, (*cesto)[j+1].ean) > 0) {
                    ItemNoCesto temp = (*cesto)[j];
                    (*cesto)[j] = (*cesto)[j+1];
                    (*cesto)[j+1] = temp;
                    done = 0;
                }
            }
            if (done) break;
        }
        
        
        for (int i = 0; i < *num_items; i++) {
            if ((*cesto)[i].quantidade > 0) {
                double preco_total;
                preco_total = todosprodutos[(*cesto)[i].indice_produto].preco * (*cesto)[i].quantidade * (1 + taxas_iva[todosprodutos[(*cesto)[i].indice_produto].iva - 'A'] / 100.0);
                preco_total = (int)(preco_total * 100 + 0.5) / 100.0;
                
                printf("%c %.2lf %d %.2lf %s\n", 
                    todosprodutos[(*cesto)[i].indice_produto].iva, 
                    todosprodutos[(*cesto)[i].indice_produto].preco, 
                    (*cesto)[i].quantidade, 
                    preco_total,
                    todosprodutos[(*cesto)[i].indice_produto].descricao);
            }
        }
    }

    if (arg_lidos > 0) {
        if (!verifica_ean(ean_produto)) {
            printf("invalid ean\n");
            return;
        }

        for (int i = 0; i < total_produtos; i++) {
            if (!(strcmp(todosprodutos[i].ean, ean_produto))) {
                indice_produto = i;
            }
        }

        if (indice_produto == -1) {
            printf("%s: no such product\n", ean_produto);
            return;
        }

        for (int j = 0; j < *num_items; j++) {
            if (!strcmp((*cesto)[j].ean, ean_produto)) {
                indice_cesto = j;
                if (quantidade < 0 && (*cesto)[j].quantidade + quantidade < 0) {
                    printf("invalid quantity\n");
                    return;
                }
            }
        }
        
        if (quantidade > 0 && todosprodutos[indice_produto].stock <= 0) {
            printf("no stock\n");
            return;
        }

        if (indice_cesto != -1) {
            (*cesto)[indice_cesto].quantidade += quantidade;
        } else {
            *cesto = realloc(*cesto, sizeof(ItemNoCesto) * (*num_items + 1));
            if (*cesto == NULL) {
                printf("No memory.\n");
                // voltar aqui
                exit(0);
            }
            strcpy((*cesto)[*num_items].ean, ean_produto);
            (*cesto)[*num_items].indice_produto = indice_produto;
            (*cesto)[*num_items].quantidade = quantidade;
            (*num_items)++;
        }
        
        todosprodutos[indice_produto].stock -= quantidade;
        //todosprodutos[indice_produto].vendidos += quantidade;

        double preco_total;
        int indice = (indice_cesto != - 1) ? indice_cesto : (*num_items - 1);
        preco_total = todosprodutos[indice_produto].preco * (*cesto)[indice].quantidade * (1 + taxas_iva[todosprodutos[indice_produto].iva - 'A'] / 100.0);
        preco_total = (int)(preco_total * 100 + 0.5) / 100.0;

        printf("%c %.2lf %d %.2lf %s\n", 
            todosprodutos[indice_produto].iva, 
            todosprodutos[indice_produto].preco, 
            (*cesto)[indice].quantidade, 
            preco_total,
            todosprodutos[indice_produto].descricao);
    }
}

void comando_r(Produto todosprodutos[MAXPRODUTOS], int total_produtos, Fatura *faturas, int num_faturas, int taxas_iva[MAXIVA]) {
    char arg[MAXLINHA], ean_produto[MAXEAN];
    int encontrou = 0;

    fgets(arg, MAXLINHA, stdin);

    if (sscanf(arg, "%s", ean_produto) == 1) {
        if (!verifica_ean(ean_produto)) {
            printf("invalid ean\n");
            return;
        }


        for (int i = 0; i < total_produtos; i++) {
            if (!strcmp(todosprodutos[i].ean, ean_produto)) {
                encontrou = 1;

                printf ("%d %d %s\n", todosprodutos[i].stock, todosprodutos[i].vendidos, todosprodutos[i].descricao);
                return;
            }
        }

        if (!encontrou) {
            printf("%s: no such product\n", ean_produto);
            return;
        }
    } else {
        double valor_total = 0.0;
        int total_items = 0;

        for(int i = 0; i < num_faturas; i++) {
            valor_total += faturas[i].valor;
            total_items += faturas[i].num_items;
        }

        printf("%d %d %.2lf\n", total_items, num_faturas, valor_total);

        for (int i = 0; i < MAXIVA; i++) {
            if (taxas_iva[i] >= 0) {
                printf("%c %d%%\n", 'A' + i, taxas_iva[i]);
            }
        }
    }
}


int verifica_nif(char *arg) {
    int tamanho = strlen(arg);
    
    if (tamanho != 9) return 0;

    for (int i = 0; i < tamanho; i++) {
        if (!isdigit(arg[i])) return 0;
    }

    return 1;
}

char *extrai_nome(char *arg) {
    char *aspas = strchr(arg, '"');
    
    if (aspas != NULL) {
        char *final_aspas = strchr(aspas + 1, '"');
        int tamanho_nome = final_aspas - aspas - 1;
        char *nome = malloc(tamanho_nome + 1);

        if (nome == NULL) {
            printf("No memory.\n");
            exit(0);
        }        
        
        strncpy(nome, aspas + 1, tamanho_nome);
        nome[tamanho_nome] = '\0';

        return nome;
    } else {
        char argcopy[MAXLINHA];
        sscanf(arg, "%s", argcopy);
        int tamanho_nome = strlen(argcopy);
        char *nome = malloc(tamanho_nome + 1);

        if (nome == NULL) {
            printf("No memory.\n");
            exit(0);
        }   

        strcpy(nome, argcopy);

        return nome;
    }
}

void comando_f(Produto todosprodutos[MAXPRODUTOS], ItemNoCesto **cesto, int *num_items, Fatura **faturas, int *num_faturas, int *numero_proxima_fatura, int taxas_iva[MAXIVA]) {
    char arg[MAXLINHA];
    char primeiro_arg[MAXLINHA];
    int nif = 999999999;
    char *nome = NULL;
    int lidos, num_items_dif_cesto = 0;

    fgets (arg, MAXLINHA, stdin);
    lidos = sscanf(arg, "%s", primeiro_arg);

    if (lidos <= 0) {
        nome = malloc(strlen("Cliente final") + 1);
        strcpy(nome, "Cliente final");

    } else if (strcmp(primeiro_arg, "error") == 0) {
        for (int i = 0; i < *num_items; i++) {
            todosprodutos[(*cesto)[i].indice_produto].stock += (*cesto)[i].quantidade;
        }
        free(*cesto);
        *cesto = NULL;
        *num_items = 0;
        return;

    } else if (verifica_nif(primeiro_arg)){
        nif = atoi(primeiro_arg);
        char *resto = arg + strlen(primeiro_arg);
        nome = extrai_nome(resto);
        
    } else {
        nome = extrai_nome(arg);
    }

    double preco_total = 0.0;
    for (int i = 0; i < *num_items; i++) {
        preco_total += todosprodutos[(*cesto)[i].indice_produto].preco * (*cesto)[i].quantidade * (1 + taxas_iva[todosprodutos[(*cesto)[i].indice_produto].iva - 'A']/100.0);
    }
    preco_total = (int)(preco_total * 100 + 0.5) / 100.0;

    *faturas = realloc(*faturas, sizeof(Fatura) * (*num_faturas + 1));

    if (*faturas == NULL) {
        printf("No memory.\n");
        exit(0);
    }   

    (*faturas)[*num_faturas].nif = nif;
    (*faturas)[*num_faturas].nome_cliente = malloc(strlen(nome) + 1);
    strcpy((*faturas)[*num_faturas].nome_cliente, nome);
    (*faturas)[*num_faturas].numero = *numero_proxima_fatura;
    (*faturas)[*num_faturas].valor = preco_total;
    
    for (int i = 0; i < *num_items; i++) {
        if ((*cesto)[i].quantidade > 0) {
            todosprodutos[(*cesto)[i].indice_produto].vendidos += (*cesto)[i].quantidade;
            num_items_dif_cesto++;
        }
    }
    (*faturas)[*num_faturas].num_items = num_items_dif_cesto;
    
    printf("%d %.2lf %d\n", (*faturas)[*num_faturas].num_items, (*faturas)[*num_faturas].valor, (*faturas)[*num_faturas].numero);
    
    free(nome);
    (*num_faturas)++;
    (*numero_proxima_fatura)++;

    free(*cesto);
    *cesto = NULL;
    *num_items = 0;
}