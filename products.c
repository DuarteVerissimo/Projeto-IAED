#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"


/** Verify if an EAN code is valid
 * @param ean   EAN code to verify
 * @return      1 if valid, 0 otherwise
 */
int verifyEAN(char ean[MAXEAN]) {
    int i = 0, sum = 0, num = 0, check_digit = 0;

    while (ean[i] != '\0' && isdigit(ean[i])) 
        i++;
    
    if (!((i == 13 || i == 8) && ean[i] == '\0'))
        return 0;
    
    for (int j = 0; j < i - 1; j++) {
        num = ean[j] - '0';
        
        if (j % 2 == 0)
            sum += num;
        else
            sum += num * 3;
    }

    check_digit = (10 - (sum % 10)) % 10;
    return check_digit == ean[i - 1] - '0';
}


/** Verify if a wildcard pattern matches an EAN code
 * @param pattern   wildcard pattern
 * @param ean       EAN code to match against
 * @return          1  if valid, 0 otherwise
 */
int verifyWildcard(char *pattern, char *ean) {
    if (pattern[0] == '\0' && ean[0] == '\0')
        return 1;
    
    else if ((pattern[0] == '?' && ean[0] != '\0') || pattern[0] == ean[0])
        return verifyWildcard(pattern + 1, ean + 1);
    
    else if (pattern[0] == '*')
        return verifyWildcard(pattern + 1, ean) || (ean[0] != '\0' && verifyWildcard(pattern, ean + 1));
    
    else
        return 0;
}

/** Find a product by its EAN code
 * @param products          array of products
 * @param total_products    total number of products
 * @param ean               EAN code to search for
 * @return                  index of product if found, -1 otherwise
 */
int findProduct(Product products[MAXPRODUCTS], int total_products, char *ean) {
    int i;
    for (i = 0; i < total_products; i++) {
        if (!strcmp(products[i].ean, ean))
            return i;
    }
    return -1;
}


/** Validate product fields before adding to the system.
 * Checks EAN, VAT class, price, quantity and description.
 * @param sys           system state
 * @param ean           EAN code to validate
 * @param iva           VAT class to validate
 * @param price         price to validate
 * @param stock         quantity to validate
 * @param description   description to validate
 * @return              1 if valid, 0 otherwise
 */
int validateProduct(System *sys, char *ean, char iva, double price, int quantity, char *description) {
    int idx_iva = iva - 'A';
    
    if (!(verifyEAN(ean))) {
        puts(EINVALID_EAN);
        return 0;
    }
    if (!(sys->taxas_iva[idx_iva] >= 0)) {
        puts(EINVALID_IVA);
        return 0;
    }
    if (price <= 0) {
        puts(EINVALID_PRICE);
        return 0;
    }
    if (stock < 0) {
        puts(EINVALID_QTY);
        return 0;
    }
    if (strlen(description) > 50 || !(isupper(description[0]))) {
        puts(EINVALID_DESC);
        return 0;
    }

    return 1;
}


/** Add or update a product in the system.
 * If the product already exists, updates its stock, VAT and price.
 * Otherwise, adds a new product to the system.
 * @param sys           system state
 * @param ean           EAN code
 * @param iva           VAT class
 * @param price         unit price
 * @param quantity      quantity to add
 * @param description   product description
 * @param idx_cart      index of product in cart, -1 if not in cart
 */
void addProduct(System *sys, char *ean, char iva, double price, int quantity,
        char *description, int idx_cart) {
    int idx_product = findProduct(sys->products, sys->total_products, ean);

    if (idx_product != -1) {
        if (idx_cart != -1 && price != sys->products[idx_product].price) {
            puts(EPRODUCT_IN_USE);
            return;
        }

        sys->products[idx_product].stock += quantity;
        sys->products[idx_product].iva = iva;        
        sys->products[idx_product].price = price;
        printf("%d\n", sys->products[idx_product].stock);
        return;
    }
    else {
        if (sys->total_products >= MAXPRODUCTS) {
            puts(EINVALID_PROD);
            return;
        }

        strcpy(sys->products[sys->total_products].description, description);
        strcpy(sys->products[sys->total_products].ean, ean);
        sys->products[sys->total_products].iva = iva;
        sys->products[sys->total_products].number = sys->total_products;
        sys->products[sys->total_products].price = price;
        sys->products[sys->total_products].sold = 0;
        sys->products[sys->total_products].stock = quantity;
        
        printf("%d\n", sys->products[sys->total_products].stock);
        (sys->total_products)++;
    }
}


/** Process the 'p' command - add or update a product.
 * @param sys   system state
 * @param buf   input line
 */
void commandP(System *sys, char *buf) {
    char ean[MAXEAN];
    char iva;
    double price;
    int quantity;
    char description[MAXDESC];

    sscanf(buf, "%*s %s %c %lf %d %[^\n]", ean, &iva, &price, &quantity, description);

    if (validateProduct(sys, ean, iva, price, quantity, description)) {
        int idx_product = findProduct(sys->products, sys->total_products, ean);
        int idx_cart = findProductInCart(sys->cart, sys->cart_size, idx_product);
        addProduct(sys,  ean, iva, price, quantity, description, idx_cart);
        return;
    }
    return;
}