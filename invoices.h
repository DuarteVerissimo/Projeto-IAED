/**
 * Header for invoice management.
 * @file invoices.h
 * @author ist1117729 (Duarte Veríssimo)
 */

#ifndef INVOICES_H
#define INVOICES_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


int verifyNif(char *nif);
int isValidNameStart(unsigned char c);
char *extractQuotedName(char *quote_start);
char *extractSingleWordName(char *buf);
char *extractName(char *buf);
void setStandardClient(int *nif, char **name);
int readClient(char *buf, int *nif, char **name);

int findInvoice(System *sys, int number);
void addInvoice(System *sys, int nif, char *name, double value, int num_items);
void deleteInvoice(System *sys, int idx_invoice);

void printInvoiceCommandF(Invoice invoice);
void commandF(System *sys, char *buf);

void sortInvoices(System *sys, int start, int end);
int partition(System *sys, int start, int end);
void printInvoiceCommandC(Invoice invoice);
void listClientInvoices(System *sys, char *name);
void listAllInvoices(System *sys);
void commandC(System *sys, char *buf);

void printAndDeleteInvoice(System *sys, int idx_invoice);
void commandDProduct(System *sys, char ean[MAXLINE], int quantity);
void commandD(System *sys, char *buf);

void destroyInvoices(System *sys);

#endif