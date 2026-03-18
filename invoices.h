#ifndef INVOICES_H
#define INVOICES_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findInvoice(System *sys, int number);
void addInvoice(System *sys, int nif, char *name, double value, int num_items);
void deleteInvoice(System *sys, int idx_invoice);
void printInvoice(Invoice invoice);
int verifyNif(char *nif);
char *extractName(char *buf);
void commandF(System *sys, char *buf);
void commandC(System *sys, char *buf);
void commandD(System *sys, char *buf);

#endif