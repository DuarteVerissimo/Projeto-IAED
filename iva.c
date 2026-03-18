#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"

void initIva(int iva_taxes[MAXIVA]) {
    iva_taxes['A' - 'A'] = 0;
    iva_taxes['B' - 'A'] = 6;
    iva_taxes['C' - 'A'] = 13;
    iva_taxes['D' - 'A'] = 23;

    for (int i = 'E' - 'A'; i < MAXIVA; i++)
        iva_taxes[i] = - 1;
}

/** Calculate price with IVA and symmetric rounding
 * @param price     unit price
 * @param quantity  quantity
 * @param iva_value iva value percentage
 * @return          total price with iva rounded to cents
 */
double calculatePrice(double price, int quantity, int iva_value) {
    double total_price = price * quantity * (1 + iva_value / 100.0);
    total_price = (int)(total_price * 100 + 0.5) / 100.0;
    return total_price;
}