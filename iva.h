/**
 * Header for IVA tax rate management and billing summary.
 * @file iva.h
 * @author ist1117729 (Duarte Veríssimo)
 */

#ifndef IVA_H
#define IVA_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"

int indexIva(char c);
void initIva(int *iva_taxes);
void initDefaultIva(int *iva_taxes);
void openIvaFile(System *sys, int argc, char **argv);

double calculatePrice(double price, int quantity, int iva_value);
void commandR(System *sys, char *buf);

#endif