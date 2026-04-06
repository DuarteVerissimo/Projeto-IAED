/**
 * Shopping cart management: adding, removing, validating and listing items.
 * @file cart.c
 * @author ist1117729 (Duarte Veríssimo)
 */

#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"

/**
 * Checks if memory allocation was successful.
 * Exits the program with an error message if it failed.
 * @param ptr pointer to the allocated memory
 */
void checkMemory(void *ptr) {
    if (ptr == NULL) {
        puts(ENO_MEMORY);
        exit(0);
    }
}

/** 
 * Finds a product in the cart by its product index using linear search.
 * @param sys system state
 * @param product_idx product index to search for
 * @return index in cart if found, -1 otherwise
 */
int findProductInCart(System *sys, int product_idx) {
    int i;
    
    for (i = 0; i < sys->cart_size; i++) {
        if (sys->cart[i].product_index == product_idx)
            return i;
    }
    
    return -1;
}

/**
 * Adds a quantity of a product to the shopping cart.
 * If the product is already in the cart, it updates the quantity.
 * Otherwise, it dynamically allocates memory for a new cart item.
 * @param sys system state
 * @param idx_product index of the product to be added
 * @param quantity amount of the product to add
 * @return the index of the item in the cart array
 */
int addToCart(System *sys, int idx_product, int quantity) {
    int idx_cart = findProductInCart(sys, idx_product);

    if (idx_cart != -1) {
        sys->cart[idx_cart].quantity += quantity;
        return idx_cart;
    }

    sys->cart = realloc(sys->cart, sizeof(CartItem) * (sys->cart_size + 1));
    checkMemory(sys->cart);

    sys->cart[sys->cart_size].product_index = idx_product;
    sys->cart[sys->cart_size].quantity = quantity;
    strcpy(sys->cart[sys->cart_size].ean, sys->products[idx_product].ean);

    sys->cart_size++;
    return sys->cart_size - 1;
}

/**
 * Removes an item from the shopping cart by shifting subsequent elements.
 * @param sys system state
 * @param idx_cart index of the item to remove from the cart
 */
void removeFromCart(System *sys, int idx_cart) {
    int j;

    for (j = idx_cart; j < sys->cart_size - 1; j++)
        sys->cart[j] = sys->cart[j + 1];
        
    sys->cart_size--;
}

/**
 * Prints a shopping cart item in the format:
 * <iva> <unit_price> <quantity> <total_price_with_iva> <description>
 * @param sys system state
 * @param idx_cart index of the item in the cart array
 */
void printCartItem(System *sys, int idx_cart) {
    int idx_product = sys->cart[idx_cart].product_index;
    int idx_iva = indexIva(sys->products[idx_product].iva);
    Product *p = &sys->products[idx_product]; 
    
    double total_price = calculatePrice(p->price, 
                                        sys->cart[idx_cart].quantity, 
                                        sys->iva_taxes[idx_iva]);
    
    printf("%c %.2lf %d %.2lf %s\n",
           p->iva, 
           p->price, 
           sys->cart[idx_cart].quantity, 
           total_price, 
           p->description);
}

/**
 * Lists all items currently in the shopping cart.
 * Prints in ascending order by EAN by iterating through the sorted index array.
 * @param sys system state
 */
void listCart(System *sys) {
    int i, idx_cart;

    for (i = 0; i < sys->total_products; i++) {
        idx_cart = findProductInCart(sys, sys->product_indexes_by_ean[i]);

        if (idx_cart != -1 && sys->cart[idx_cart].quantity > 0)
            printCartItem(sys, idx_cart);
    }
}

/** 
 * Validates the addition or removal of an item from the cart.
 * @param sys system state
 * @param product_ean product EAN code
 * @param quantity amount of the product to add or remove
 * @return 1 if valid, 0 otherwise
 */
int validateCartItem(System *sys, char *product_ean, int quantity) {
    int idx_product, idx_cart;

    if (!verifyEan(product_ean)) {
        puts(EINVALID_EAN);
        return 0;
    }
    
    idx_product = findProduct(sys, product_ean);
    if (idx_product == -1) {
        printf("%s: %s\n", product_ean, ENO_PRODUCT);
        return 0;
    }
    
    idx_cart = findProductInCart(sys, idx_product);
    if (quantity < 0 && (idx_cart == -1 || 
        sys->cart[idx_cart].quantity + quantity < 0)) {
        puts(EINVALID_QTY);
        return 0;
    }
    
    if (quantity > 0 && sys->products[idx_product].stock - quantity < 0) {
        puts(ENO_STOCK);
        return 0;
    }
    
    return 1;
}

/** 
 * Parses the arguments for the 'a' command.
 * @param buf input line
 * @param product_ean pointer to store the extracted EAN code
 * @param quantity pointer to store the extracted quantity
 * @return 1 if arguments were read successfully, -1 if empty
 */
int readCartArguments(char *buf, char *product_ean, int *quantity) {
    char arg1[MAXLINE], arg2[MAXLINE];
	int num_read = sscanf(buf + 2, "%s %s", arg1, arg2);

	if (num_read == 1) {
		strcpy(product_ean, arg1);
        return 1;
	} else if (num_read == 2) {
        *quantity = atoi(arg1);
        strcpy(product_ean, arg2);
        return 1;
    }

    return -1;
}

/** 
 * Processes the 'a' command to add items to the cart or list them.
 * @param sys system state
 * @param buf input line
 */
void commandA(System *sys, char *buf) {
	int quantity = 1, idx_product, idx_cart;
    char product_ean[MAXLINE];

    if (readCartArguments(buf, product_ean, &quantity) == -1) {
        listCart(sys);
        return;
    }

	if (!validateCartItem(sys, product_ean, quantity))
	    return;

	idx_product = findProduct(sys, product_ean);

    /* Update main product stock */
    sys->products[idx_product].stock -= quantity;

	idx_cart = addToCart(sys, idx_product, quantity);
	printCartItem(sys, idx_cart);

    /* If quantity dropped to 0, remove the item entirely */
    if (sys->cart[idx_cart].quantity == 0) {
        removeFromCart(sys, idx_cart);
    }
}

/**
 * Frees the dynamically allocated memory for the shopping cart.
 * Resets the cart pointer to NULL and the cart size to 0.
 * @param sys system state
 */
void destroyCart(System *sys) {
    free(sys->cart);
    sys->cart = NULL;
    sys->cart_size = 0;
}