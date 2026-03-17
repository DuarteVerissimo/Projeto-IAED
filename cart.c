#include "cart.h"


/** Find a product in the cart by its product index
 * @param cart          array of cart items
 * @param cart_size     number of items in cart
 * @param product_idx   product index to search for
 * @return              index in cart if found, -1 otherwise
 */
int findProductInCart(CartItem *cart, int cart_size, int product_idx) {
    int i;
    for (i = 0; i < cart_size; i++) {
        if (cart[i].product_index == product_idx)
            return i;
    }
    return -1;
}