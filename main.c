#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"

/** Main function
 * @param argc  number of arguments
 * @param argv  argument vector
 * @return      always returns 0
 */
int main(int argc, char *argv[]) {
    char buf[MAXLINE];
    System sys = {0};
    sys.next_invoice_number = 1;
    sys.max_invoices = 100;
    sys.invoices = malloc(sizeof(Invoice) * sys.max_invoices);

    initIva(sys.iva_taxes);
    openIvaFile(&sys, argc, argv);
    while (fgets(buf, MAXLINE, stdin)) {
        switch (buf[0]) {
			case 'q': 
                for (int i = 0; i  < sys.num_invoices; i++) 
                    free(sys.invoices[i].client_name);
                free(sys.invoices);
                free(sys.cart);    
                return 0;
			case 'p': commandP(&sys, buf); break;
			case 'l': commandL(&sys, buf); break;
			case 'a': commandA(&sys, buf); break;
			case 'r': commandR(&sys, buf); break;
			case 'f': commandF(&sys, buf); break;
			case 'c': commandC(&sys, buf); break;
			case 'd': commandD(&sys, buf); break;
			default: break;
        }
    }
    return 0;
}