#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXDESC 51 //TRATAR DISTO DEPOIS
#define MAXEAN 14
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
    char *nome_cliente; 
    double valor;
    int num_items;
} Fatura;




/*
typedef struct {
    Produto todosprodutos[MAXPRODUTOS];
    int total_produtos;
    Fatura *faturas;
    int total_faturas;
    int fatura_atual;
    int taxas_iva[MAXIVA];
} Sistema;
 */

void inicializa_iva(int taxas_iva[MAXIVA]);
int verifica_ean(char ean[MAXEAN]);
void comando_p(Produto todosprodutos[MAXPRODUTOS], int *ptotal_produtos, int taxas_iva[MAXIVA]);
int verifica_wildcard(char *codigo, char *ean);
void comando_l(Produto todosprodutos[MAXPRODUTOS], int total_produtos, ItemNoCesto *cesto, int num_items);
void comando_a(Produto todosprodutos[MAXPRODUTOS], int total_produtos, ItemNoCesto **cesto, int *num_items, int taxas_iva[MAXIVA]);
void comando_r(Produto todosprodutos[MAXPRODUTOS], int total_produtos, Fatura *faturas, int num_faturas, int taxas_iva[MAXIVA], ItemNoCesto *cesto, int num_items_cesto);
void comando_f(Produto todosprodutos[MAXPRODUTOS], ItemNoCesto **cesto, int *num_items, Fatura **faturas, int *num_faturas, int *proximo_numero_fatura, int taxas_iva[MAXIVA]);
void comando_c(Fatura *faturas, int num_faturas);
void comando_d(Produto todosprodutos[MAXPRODUTOS], int *total_produtos, Fatura **faturas, int *num_faturas, ItemNoCesto *cesto, int num_items_cesto);