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

    initIva(sys.iva_taxes);
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");

        if (f != NULL) {
            int value_iva;
            char letter_iva;
            while (fscanf(f, " %c %d", &letter_iva, &value_iva) == 2)
                sys.iva_taxes[letter_iva - 'A'] = value_iva;
        }
        fclose(f);
    }
    while (fgets(buf, MAXLINE, stdin)) {
        switch (buf[0]) {
			case 'q': /* libertar memória */ return 0;
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