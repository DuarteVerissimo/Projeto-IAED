#ifndef INVOICES_H
#define INVOICES_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findInvoice(System *sys, int number);
int verifyNif(char *nif);
char *extractName(char *buf);
void commandF(System *sys, char *buf);
void commandC(System *sys, char *buf);
void commandD(System *sys, char *buf);

#endif