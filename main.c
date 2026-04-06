/**
 * Main entry point for the billing system.
 * @file main.c
 * @author ist1117729 (Duarte Veríssimo)
 */

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "products.h"
#include "cart.h"
#include "invoices.h"
#include "iva.h"

/**
 * Main function that initializes the system and processes input commands.
 * @param argc number of command-line arguments
 * @param argv command-line argument vector
 * @return 0 on successful execution
 */
int main(int argc, char **argv) {
    char buf[MAXLINE];
    System sys = {0};

    /* Initialize system state */
    sys.next_invoice_number = 1;
    sys.max_invoices = 100;
    sys.invoices = malloc(sizeof(Invoice) * sys.max_invoices);
    checkMemory(sys.invoices);

    initIva(sys.iva_taxes);
    openIvaFile(&sys, argc, argv);

    /* Main command processing loop */
    while (fgets(buf, MAXLINE, stdin)) {
        switch (buf[0]) {
            case 'q': 
                destroyInvoices(&sys);
                destroyCart(&sys);  
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