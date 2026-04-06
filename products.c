/**
 * Product management: creation, validation, search and listing.
 * @file products.c
 * @author ist1117729 (Duarte Veríssimo)
 */

#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"


/** Verify if an EAN code is valid.
 *  @param ean   EAN code to verify
 * @return      1 if valid, 0 otherwise
 */
int verifyEan(char ean[MAXLINE]) {
    int i = 0, sum = 0, num = 0, check_digit = 0;

    while (ean[i] != '\0' && isdigit(ean[i]))
        i++;
    
    if (!((i == 13 || i == 8) && ean[i] == '\0'))
        return 0;
    
    for (int j = 0; j < i - 1; j++) {
        num = ean[j] - '0';
        sum += num * (j % 2 == 0 ? 1 : 3);
    }
    check_digit = (10 - (sum % 10)) % 10;
    return check_digit == ean[i - 1] - '0';
}

/**
 * Finds a product by EAN using binary search on the sorted index array.
 * @param sys   system state
 * @param ean   EAN code to search for
 * @return      index in products array if found, -1 otherwise
 */
int findProduct(System *sys, char *ean) {
    int low = 0, high = sys->total_products - 1, mid, comp;
    while (low <= high) {
        mid = low + (high - low) / 2;
        char *ean_mid = sys->products[sys->product_indexes_by_ean[mid]].ean;
        comp = strcmp(ean, ean_mid);    
        if (comp == 0) 
            return sys->product_indexes_by_ean[mid];
        if (comp < 0)
            high = mid - 1;
        if (comp > 0)
            low = mid + 1;
    }
    return -1;
}

/** 
 * Validate product fields before adding to the system.
 * Checks EAN, IVA, price, quantity and description.
 * @param sys           system state
 * @param ean           EAN code to validate
 * @param iva           IVA class letter
 * @param price         price to validate
 * @param stock         quantity to validate
 * @param description   description to validate
 * @return              1 if valid, 0 otherwise
 */
int validateProduct(System *sys, char *ean, char iva, double price, 
        int quantity, char *description) {
    int idx_iva = indexIva(iva);
    
    if (!(verifyEan(ean))) {
        puts(EINVALID_EAN);
        return 0;
    }
    if (idx_iva == -1 || sys->iva_taxes[idx_iva] < 0) {
        puts(EINVALID_IVA);
        return 0;
    }
    if (price <= 0) {
        puts(EINVALID_PRICE);
        return 0;
    }
    if (quantity < 0) {
        puts(EINVALID_QTY);
        return 0;
    }
    if (strlen(description) > 50 || !(isupper(description[0]) ||
        (unsigned char)description[0] >= 128)) {
        puts(EINVALID_DESC);
        return 0;
    }

    return 1;
}

/**
 * Updates an existing product's fields and increments its stock.
 * Rejects the update if the product is in the cart and the price changed.
 * @param sys           system state
 * @param iva           new IVA class letter
 * @param price         new unit price
 * @param quantity      quantity to add to current stock
 * @param description   new product description
 * @param idx_cart      index in cart (-1 if not present)
 * @param idx_product   index in products array
 */
void updateProduct(System *sys, char iva, double price, int quantity,
        char *description, int idx_cart, int idx_product) {
    if (idx_cart != -1 && price != sys->products[idx_product].price) {
        puts(EPRODUCT_IN_USE);
        return;
    }
    sys->products[idx_product].stock += quantity;
    sys->products[idx_product].iva = iva;        
    sys->products[idx_product].price = price;
    strcpy(sys->products[idx_product].description, description);
    printf("%d\n", sys->products[idx_product].stock);
}

/**
 * Creates a new product and inserts it maintaining EAN sorted order.
 * @param sys           system state
 * @param ean           EAN code
 * @param iva           IVA class letter
 * @param price         unit price
 * @param quantity      initial stock
 * @param description   product description
 */
void createProduct(System *sys,  char *ean, char iva, double price,
        int quantity, char *description) {
    int i = sys->total_products - 1;

    if (sys->total_products >= MAXPRODUCTS) {
        puts(EINVALID_PROD);
        return;
    }
    strcpy(sys->products[sys->total_products].description, description);
    strcpy(sys->products[sys->total_products].ean, ean);
    sys->products[sys->total_products].iva = iva;
    sys->products[sys->total_products].price = price;
    sys->products[sys->total_products].sold = 0;
    sys->products[sys->total_products].stock = quantity;

    while (i >= 0 && 
            strcmp(ean, sys->products[sys->product_indexes_by_ean[i]].ean) < 0) {
        sys->product_indexes_by_ean[i + 1] = sys->product_indexes_by_ean[i];
        i--;
    }
    sys->product_indexes_by_ean[i + 1] = sys->total_products;
    printf("%d\n", sys->products[sys->total_products].stock);
    (sys->total_products)++;
}

/** 
 * Process the 'p' command - add or update a product.
 * @param sys   system state
 * @param buf   input line
 */
void commandP(System *sys, char buf[MAXLINE]) {
    char ean[MAXLINE], iva, description[MAXLINE];
    double price;
    int quantity;

    sscanf(buf + 2, "%s %c %lf %d %[^\n]", ean, &iva, &price, &quantity,
        description);

    if (validateProduct(sys, ean, iva, price, quantity, description)) {
        int idx_product = findProduct(sys, ean);
        if (idx_product != -1) {
            int idx_cart = findProductInCart(sys, idx_product);
            updateProduct(sys, iva, price, quantity, description,
                idx_cart, idx_product);
        } else {
            createProduct(sys, ean, iva, price, quantity, description);
        }
    }
}

/** 
 * Prints product information in the format:
 * <ean> <iva> <price> <sold_and_in_cart> <stock> <description>
 * @param product       pointer to product
 * @param soldAndInCart quantity sold plus quantity in cart
 */
void printProduct(Product *product, int soldAndInCart) {
    printf("%s %c %.2lf %d %d %s\n",
        product->ean,
        product->iva,
        product->price,
        soldAndInCart,
        product->stock,
        product->description);
}

/** Get the total quantity sold plus quantity in cart for a product.
 * @param sys           system state
 * @param idx_product   index of product in products array
 * @return              total quantity sold and in cart
 */
int getProductSoldAndInCart(System *sys, int idx_product) {
    int idx_cart = findProductInCart(sys, idx_product);
    if (idx_cart != -1)
        return  sys->products[idx_product].sold + sys->cart[idx_cart].quantity;
    else 
        return sys->products[idx_product].sold;
}

/**
 * Lists all products with stock > 0, in order of creation.
 * @param sys   system state
 */
void listAllProducts(System *sys) {
    int i, soldAndInCart, found = 0;
    for (i = 0; i < sys->total_products; i++) {
        if (sys->products[i].stock > 0) {
            soldAndInCart = getProductSoldAndInCart(sys, i);
            printProduct(&sys->products[i], soldAndInCart);
            found = 1;
        }
    }
    if (!found)
        puts("*: no such product");
}

/** Verify if a wildcard pattern matches an EAN code.
 * @param pattern   wildcard pattern
 * @param ean       EAN code to match against
 * @return          1  if valid, 0 otherwise
 */
int verifyWildcard(char *pattern, char *ean) {
    if (pattern[0] == '\0' && ean[0] == '\0')
        return 1;
    if ((pattern[0] == '?' && ean[0] != '\0') || pattern[0] == ean[0])
        return verifyWildcard(pattern + 1, ean + 1);
    if (pattern[0] == '*')
        return verifyWildcard(pattern + 1, ean) ||
            (ean[0] != '\0' && verifyWildcard(pattern, ean + 1));
    return 0;
}

/**
 * Lists all products with stock > 0 whose EAN matches the given pattern.
 * @param sys       system state
 * @param pattern   wildcard pattern to match against EAN codes
 */
void listProductsByPattern(System *sys, char *pattern) {
    int i, soldAndInCart, found = 0;
    for (i = 0; i < sys->total_products; i++) {
        if (sys->products[i].stock > 0 
            && verifyWildcard(pattern, sys->products[i].ean)) {
            soldAndInCart = getProductSoldAndInCart(sys, i);
            printProduct(&sys->products[i], soldAndInCart);
            found = 1;
        }
    }
    if (!found)
        printf("%s: %s\n", pattern, ENO_PRODUCT);
}

/**
 * Processes the 'l' command - lists products matching given EAN wildcards.
 * Lists all available products if no argument or '*' is given.
 * @param sys   system state
 * @param buf   input line
 */
void commandL(System *sys, char buf[MAXLINE]) {
    char *pattern = strtok(buf + 2, " \n");

    if (pattern == NULL)
        listAllProducts(sys);
    else {
        while (pattern != NULL) {
            listProductsByPattern(sys, pattern);
            pattern = strtok(NULL, " \n");
        }
    }
}

/**
 * Removes a product from the system, updating all index structures.
 * Also updates cart indices to reflect the removed product's position.
 * @param sys           system state
 * @param idx_product   index of the product to remove
 */
void deleteProduct(System *sys, int idx_product) {
    int i, idx_by_ean = 0;
    for (i = 0; i < sys->total_products; i++) {
        if (sys->product_indexes_by_ean[i] == idx_product) {
            idx_by_ean = i;
            break;
        }
    }
    for (i = 0; i < sys->total_products; i++) {
        if (sys->product_indexes_by_ean[i] > idx_product)
            sys->product_indexes_by_ean[i] -= 1;
    }
    for (i = idx_by_ean; i < sys->total_products - 1; i++)
        sys->product_indexes_by_ean[i] = sys->product_indexes_by_ean[i + 1];
    
    for (i = idx_product; i < sys->total_products - 1; i++)
        sys->products[i] = sys->products[i + 1];

    for (i = 0; i < sys->cart_size; i++) {
        if (sys->cart[i].product_index > idx_product)
            sys->cart[i].product_index--;
    }
    sys->total_products--;
}