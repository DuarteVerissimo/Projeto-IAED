#ifndef PRODUCTS_H
#define PRODUCTS_H

#include "types.h" //ver depois
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
#define MAXPRODUCTS 10000
#define MAXDESC 51
#define MAXEAN 14

typedef struct {
    char description[MAXDESC];
    char ean[MAXEAN];
    double price;
    char iva;
    int stock;
    int sold;
    int number;
} Product;
*/  

int verifyEan(char ean[MAXEAN]);
int verifyWildcard(char *pattern, char *ean);
int findProduct(System *sys, char *ean);
int validateProduct(System *sys, char *ean, char iva, double price, int quantity, char *description);
void addProduct(System *sys, char *ean, char iva, double price, int stock, char *description, int idx_cart);
void commandP(System *sys, char buf[MAXLINE]);
void printProduct(Product *product, int soldAndInCart);
void listAllProducts(System *sys);
void listProductsByPattern(System *sys, char *pattern);
int getProductSoldAndInCart(System *sys, int idx_product);
void commandL(System *sys, char buf[MAXLINE]);


#endif