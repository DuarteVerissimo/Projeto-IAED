/**
 * Header for shopping cart management.
 * @file cart.h
 * @author ist1117729 (Duarte Veríssimo)
 */


#ifndef CART_H
#define CART_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void checkMemory(void *ptr);
int findProductInCart(System *sys, int product_idx);
void addToCart(System *sys, int idx_product, int quantity);
void removeFromCart(System *sys, int idx_cart);
void printCartItem(System *sys, int idx_cart);
void listCart(System *sys);
int validateCartItem(System *sys, char *product_ean, int quantity);
void commandA(System *sys, char buf[MAXLINE]);
void destroyCart(System *sys);

#endif