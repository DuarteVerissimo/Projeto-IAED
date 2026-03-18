#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"


/** Find a product in the cart by its product index
 * @param cart          array of cart items
 * @param cart_size     number of items in cart
 * @param product_idx   product index to search for
 * @return              index in cart if found, -1 otherwise
 */
int findProductInCart(System *sys, int product_idx) {
    int i;
    for (i = 0; i < sys->cart_size; i++) {
        if (sys->cart[i].product_index == product_idx)
            return i;
    }
    return -1;
}


void addToCart(System *sys, int idx_product, int quantity) {
    int idx_cart = findProductInCart(sys, idx_product);

    if (idx_cart != -1)
        sys->cart[idx_cart].quantity += quantity;
    else {
        sys->cart = realloc(sys->cart, sizeof(CartItem) * (sys->cart_size + 1));

        if (sys->cart == NULL) {
            puts(ENO_MEMORY);
            exit(0);
        }

        sys->cart[sys->cart_size].product_index = idx_product;
        sys->cart[sys->cart_size].quantity = quantity;
        strcpy(sys->cart[sys->cart_size].ean, sys->products[idx_product].ean);
        sys->cart_size++;
    }
}


void removeFromCart(System *sys, int idx_cart) {
    int j;
    for (j = idx_cart; j < sys->cart_size - 1; j++)
        sys->cart[j] = sys->cart[j + 1];
    sys->cart_size--;
}


void printCartItem(System *sys, int idx_cart) {
    int idx_product = sys->cart[idx_cart].product_index;
    double total_price = calculatePrice(sys->products[idx_product].price, sys->cart[idx_cart].quantity, sys->iva_taxes[sys->products[idx_product].iva - 'A']);
    
    printf("%c %.2lf %d %.2lf %s\n",
        sys->products[idx_product].iva,
        sys->products[idx_product].price,
        sys->cart[idx_cart].quantity,
        total_price,
        sys->products[idx_product].description);
}


void listCart(System *sys) {
    int i, j;
    for (i = 0; i < sys->cart_size; i++) {
        int done = 1;
        for(j = 0; j < sys->cart_size - 1 - i; j++) {
            if (strcmp(sys->cart[j].ean, sys->cart[j + 1].ean) > 0) {
                CartItem aux = sys->cart[j];
                sys->cart[j] = sys->cart[j + 1];
                sys->cart[j + 1] = aux;
                done = 0;
            }
        }
        if (done)
            break;
    }

    for (i = 0; i < sys->cart_size; i++) {
        if (sys->cart[i].quantity > 0)
            printCartItem(sys, i);
    }
}


void emptyCart(System *sys) {
    free(sys->cart);
    sys->cart = NULL;
    sys->cart_size = 0;
}


int validateCartItem(System *sys, char *product_ean, int quantity){
    if (!verifyEan(product_ean)) {
        puts(EINVALID_EAN);
        return 0;
    }

    int idx_product = findProduct(sys, product_ean);
    if (idx_product == -1) {
        printf("%s: %s\n", product_ean, ENO_PRODUCT);
        return 0;
    }

    int idx_cart = findProductInCart(sys, idx_product);
    if (quantity < 0 && (idx_cart == -1 || sys->cart[idx_cart].quantity + quantity < 0)) {
        puts(EINVALID_QTY);
        return 0;
    }

    if (quantity > 0 && sys->products[idx_product].stock <= 0) {
        puts(ENO_STOCK);
        return 0;
    }
    return 1;
}


void commandA(System *sys, char buf[MAXLINE]) {
	int quantity = 1;
	char product_ean[MAXEAN];
	int num_read = sscanf(buf + 2, "%d %s", &quantity, product_ean);

	if (num_read == 1) {
		sscanf(buf + 2, "%s", product_ean);
		quantity = 1;
	}
	if (num_read >= 1) {
		if (!validateCartItem(sys, product_ean, quantity))
			return;
		int idx_product = findProduct(sys, product_ean);
		int idx_cart = findProductInCart(sys, idx_product);
		sys->products[idx_product].stock -= quantity;		
        if (idx_cart != -1) {
			sys->cart[idx_cart].quantity += quantity;
			if (sys->cart[idx_cart].quantity == 0) {
				printCartItem(sys, idx_cart);
				removeFromCart(sys, idx_cart);
				return;
			}
		} else {
			addToCart(sys, idx_product, quantity);
			idx_cart = sys->cart_size - 1;
		}
		printCartItem(sys, idx_cart);
	} else {
		listCart(sys);
	}
}