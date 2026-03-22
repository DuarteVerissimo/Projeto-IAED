#ifndef INVOICES_H
#define INVOICES_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int findInvoice(System *sys, int number);
int verifyNif(char *nif);
char *extractName(char *buf);
int readClient(char buf[MAXLINE], int *nif, char **name);
void addInvoice(System *sys, int nif, char *name, double value, int num_items);
void deleteInvoice(System *sys, int idx_invoice);
void printInvoiceCommandF(Invoice invoice);
void commandF(System *sys, char buf[MAXLINE]);
void printInvoiceCommandC(Invoice invoice);
void listClientInvoices(System *sys, char *name);
void listAllInvoices(System *sys);
void sortInvoices(System *sys);
void sortInvoices2(System *sys, int start, int end);
int partition(System *sys, int start, int end);
void commandC(System *sys, char buf[MAXLINE]);
void commandD(System *sys, char buf[MAXLINE]);

#endif