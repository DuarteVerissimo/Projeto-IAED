#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXDESC 51
#define MAXEAN 14
#define MAXNOME 51
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
    int numero;
} Produto;

typedef struct {
    char ean[MAXEAN];
    int indice_produto;
    int quantidade;
} ItemCarro;

typedef struct {
    int numero;
    int nif;
    char *nome;
    int quantidade;
    double valor;
} Fatura;

typedef struct {
    char letra;
    int percentagem;
} Iva;

void inicializa_iva(int taxas_iva[MAXIVA]);
int verifica_ean(char ean[MAXEAN]);
void comando_p(Produto todosprodutos[MAXPRODUTOS], int *ptotal_produtos, int taxas_iva[MAXIVA]);
int verifica_wildcard(char *codigo, char *ean);
void comando_l(Produto todosprodutos[MAXPRODUTOS], int total_produtos);