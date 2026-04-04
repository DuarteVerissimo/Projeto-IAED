/**
 * IVA tax rate management and billing summary.
 * @file iva.c
 * @author ist1117729 (Duarte Veríssimo)
 */

#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"

/**
 * Initializes all IVA tax rates to -1 (undefined).
 * @param iva_taxes Array of IVA taxs rates
 */
void initIva(int iva_taxes[MAXIVA]) {
    int i;
    for (i = 'A' - 'A'; i < MAXIVA; i++)
        iva_taxes[i] = - 1;
}

/**
 * Sets the default IVA tax rates.
 * @param iva_taxes Array of IVA taxs rates
 */
void initDefaultIva(int iva_taxes[MAXIVA]) {
    iva_taxes['A' - 'A'] = 0;
    iva_taxes['B' - 'A'] = 6;
    iva_taxes['C' - 'A'] = 13;
    iva_taxes['D' - 'A'] = 23;
}

/** Calculate price with IVA and symmetric rounding to cents.
 * @param price     unit price
 * @param quantity  quantity
 * @param iva_value iva value percentage
 * @return          total price with iva rounded to cents
 */
double calculatePrice(double price, int quantity, int iva_value) {
    double cents = price * quantity * (100 + iva_value);
    return (int)(cents + 0.5) / 100.0;
}


void commandR(System *sys, char buf[MAXLINE]) {
    char ean_product[MAXLINE];
    if (sscanf(buf + 2, "%s", ean_product) == 1) {
        if (!verifyEan(ean_product)) {
            puts(EINVALID_EAN);
            return;
        }
        int idx_product = findProduct(sys, ean_product);
        if (idx_product == -1) {
            printf("%s: %s\n", ean_product, ENO_PRODUCT);
            return;
        }
        int soldAndInCart;
        soldAndInCart = getProductSoldAndInCart(sys, idx_product);
        printf("%d %d %s\n", sys->products[idx_product].stock, soldAndInCart, sys->products[idx_product].description);
    } else {
        int i;
        printf("%d %d %.2lf\n", sys->total_items_sold, sys->next_invoice_number - 1, sys->total_revenue);
        for (i = 0; i < MAXIVA; i++) {
            if (sys->iva_taxes[i] >= 0) {
                printf("%c %d%%\n", 'A' + i, sys->iva_taxes[i]);
            }
        }
    }
}

/**
 * Loads IVA rates from a file if provided, otherwise uses default rates.
 * @param sys   system state
 * @param argc  number of command-line arguments
 * @param argv  command-line argument vector
 */
void openIvaFile(System *sys, int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");

        if (f != NULL) {
            int value_iva;
            char letter_iva;
            while (fscanf(f, " %c %d", &letter_iva, &value_iva) == 2)
                sys->iva_taxes[letter_iva - 'A'] = value_iva;
            fclose(f);
        }
    } else
        initDefaultIva(sys->iva_taxes);
}