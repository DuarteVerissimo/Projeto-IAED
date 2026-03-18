#ifndef CART_H
#define CART_H

#include "types.h"

int findProductInCart(CartItem *cart, int cart_size, int product_idx);
void addToCart(System *sys, int idx_product, int quantity);
void removeFromCart(System *sys, int idx_cart);
void printCartItem(System *sys, int idx_cart);
void listCart(System *sys);
void emptyCart(System *sys);
int validateCartItem(System *sys, char *product_ean, int quantity);
void commandA(System *sys, char buf[MAXLINE]);


#endif