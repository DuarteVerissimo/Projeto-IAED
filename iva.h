#ifndef IVA_H
#define IVA_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void initIva(int iva_taxes[MAXIVA]);
double calculatePrice(double price, int quantity, int iva_value);

#endif