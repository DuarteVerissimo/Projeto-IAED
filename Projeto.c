#include <stdio.h>

#define MAXDESC 50
#define MAXEAN
#define MAXNOME 50
#define MAXNIF 10

/*Estrutura de produtos*/
typedef {
    char descricao[MAXDESC];
    char ean[MAXEAN];
    double preco;
    iva;
    int stock;
    int vendidos;
}