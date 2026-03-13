#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXDESC 51 //TRATAR DISTO DEPOIS
#define MAXEAN 14
#define MAXNOME 51 //TRATAR DISTO DEPOIS
#define MAXPRODUTOS 10000
#define MAXIVA 26
#define MAXLINHA 65535

/*Estrutura de produtos*/
typedef struct {
    char descricao[MAXDESC];
    char ean[MAXEAN];
    double preco;
    char iva;
    int stock;
    int vendidos;
    int numero; //ver se vou usar isto
} Produto;

typedef struct {
    char ean[MAXEAN];
    int indice_produto;
    int quantidade;
} ItemNoCesto;

typedef struct {
    int numero;
    int nif;
    char nome_cliente[MAXNOME]; //TRATAR DISTO DEPOIS
    double valor;
    ItemNoCesto *Cesto;
    int num_items;
} Fatura;

typedef struct {
    Produto todosprodutos[MAXPRODUTOS];
    int total_produtos;
    Fatura *faturas;
    int total_faturas;
    int fatura_ativa;
    int taxas_iva[MAXIVA];
} Sistema;

void inicializa_iva(int taxas_iva[MAXIVA]);
int verifica_ean(char ean[MAXEAN]);
void comando_p(Sistema *Sistema);
int verifica_wildcard(char *codigo, char *ean);
void comando_l(Sistema *Sistema);