/**
 * Product management: creation, validation, search and listing.
 * @file products.c
 * @author ist1117729 (Duarte Veríssimo)
 */

#ifndef PRODUCTS_H
#define PRODUCTS_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int verifyEan(char ean[MAXLINE]);
int findProduct(System *sys, char *ean);
void deleteProduct(System *sys, int idx_product);

int validateProduct(System *sys, char *ean, char iva, double price,
	int quantity, char *description);
void updateProduct(System *sys, char iva, double price, int quantity,
	char *description, int idx_cart, int idx_product);
void createProduct(System *sys, char *ean, char iva, double price,
	int quantity, char *description);
void commandP(System *sys, char buf[MAXLINE]);

void printProduct(Product *product, int soldAndInCart);
void listAllProducts(System *sys);
int verifyWildcard(char *pattern, char *ean);
void listProductsByPattern(System *sys, char *pattern);
int getProductSoldAndInCart(System *sys, int idx_product);
void commandL(System *sys, char buf[MAXLINE]);



#endif