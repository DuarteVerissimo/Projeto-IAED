#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXDESC 51
#define MAXEAN 14
#define MAXNOME 51
#define MAXPRODUTOS 10000
#define MAXIVA 26
#define MAXFATURAS
#define MAXCARROS 


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
    int quantidade;
} ItemCarro;

typedef struct {
    int numero;
    int nif;
    char *nome; // char nome[MAXNOME];
    int quantidade;
    double valor;
} Fatura;

typedef struct {
    char letra;
    int percentagem;
} Iva;


int verifica_ean(char ean[MAXEAN]);

int main() {
    int total_produtos = 0, total_faturas = 0, items_no_carro = 0;
    Produto todosprodutos[MAXPRODUTOS];
    Fatura todasfaturas[MAXFATURAS];
    ItemCarro carro[MAXCARROS];
    Iva tabiva[MAXIVA];
    
    for(int i = 0; i < MAXIVA; i++) {
        tabiva[i].percentagem = 0;
        tabiva[i].letra = 'A' + i;
    }

    while (1) {

    }

    return 0;
}

int verifica_ean(char ean[MAXEAN]) {
    int i = 0, soma = 0, num = 0, digito_de_verificacao = 0;
    
    while(ean[i] != '\0' && ean[i] >= '0' && ean[i] <= '9') {
        i++;
    }
    if (!(i == 13 || i == 8 && ean[i] == '\0'))
        return 0;
    
    for(int j=0; j < i - 1; j++) {
        num = ean[j] - '0';
        
        if(j % 2 == 0)
            soma += num;
        else
            soma += num*3;
    }

    digito_de_verificacao = (10 - (soma % 10)) % 10;

    return digito_de_verificacao != ean[i - 1] - '0';
}