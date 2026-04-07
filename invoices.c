/**
 * Invoice management: creation, deletion, listing and client handling.
 * @file invoices.c
 * @author ist1117729 (Duarte Veríssimo)
 */

#include "invoices.h"
#include "cart.h"
#include "iva.h"
#include "products.h"

/**
 * Verifies if a NIF is valid: 9 digits, not starting with zero.
 * @param nif NIF string to verify
 * @return 1 if valid, 0 otherwise
 */
int verifyNif(char *nif) {
    int len = strlen(nif), i;

    if (nif[0] == '0') return 0;

    if (len != NIF_LENGTH)
        return 0;
    
    for (i = 0; i < len; i++) {
        if (!isdigit(nif[i]))
            return 0;
    }

    return 1;
}

/**
 * Verifies if a character is a valid starting character for a name.
 * @param c character to verify
 * @return 1 if valid, 0 otherwise
 */
int isValidNameStart(unsigned char c) {
    if (isalpha(c) || c >= 128) {
        return 1;
    }
    return 0;
}

/**
 * Extracts a name enclosed in double quotes from a string.
 * @param quote_start pointer to the opening quote character
 * @return dynamically allocated name string or NULL if invalid
 */
char *extractQuotedName(char *quote_start) {
    char *quote_end = strchr(quote_start + 1, '"');

    if (quote_end == NULL ||
        !isValidNameStart((unsigned char)quote_start[1])) {
        puts(EINVALID_NAME);
        return NULL;
    }
 
    int len_name = quote_end - quote_start - 1;

    /* Allocate memory for the name */
    char *name = malloc(len_name + 1);
    checkMemory(name);

    strncpy(name, quote_start + 1, len_name);
    name[len_name] = '\0';
    
    return name;
}

/**
 * Extracts a single word name from a buffer.
 * @param buf input buffer containing the name
 * @return dynamically allocated name string or NULL if invalid
 */
char *extractSingleWordName(char *buf) {
    char bufcpy[MAXLINE];
    sscanf(buf, "%s", bufcpy);

    if (!isValidNameStart((unsigned char)bufcpy[0])) {
        puts(EINVALID_NAME);
        return NULL;
    }
    
    int len_name = strlen(bufcpy);

    /* Allocate memory for the name */
    char *name = malloc(len_name + 1);
    checkMemory(name);

    strcpy(name, bufcpy);
    return name;
}

/**
 * Decides how to extract a client name based on the presence of quotes.
 * @param buf input buffer to parse
 * @return pointer to the extracted name or NULL if extraction fails
 */
char *extractName(char *buf) {
    char *quote_start = strchr(buf, '"');

    if (quote_start != NULL) {
        return extractQuotedName(quote_start);
    } else {
        return extractSingleWordName(buf);
    }
}

/**
 * Sets the default client: NIF 999999999 and name "Cliente final".
 * @param nif pointer to store the default NIF
 * @param name pointer to store the allocated default name
 */
void setStandardClient(int *nif, char **name) {
    *nif = DEFAULT_NIF;
    *name = malloc(strlen(DEFAULT_CLIENT_NAME) + 1);
    checkMemory(*name);
    strcpy(*name, DEFAULT_CLIENT_NAME);
}

/**
 * Reads client information from the buffer, identifying NIF and name.
 * Handles default values when arguments are missing.
 * @param buf input line buffer
 * @param nif pointer to store the client nif
 * @param name pointer to store the allocated client name string
 * @return 1 if successful, 0 if invalid, -1 on specific error ("error")
 */
int readClient(char *buf, int *nif, char **name) {
    char first_arg[MAXLINE], sec_arg[MAXLINE];
    int arg_read = sscanf(buf + 2, "%s %s", first_arg, sec_arg);

    if (arg_read <= 0) {
        setStandardClient(nif, name);
        return 1;
    } else if (first_arg[0] == '"' || arg_read == 1) {
        *nif = DEFAULT_NIF;
        *name = extractName(buf + 2);
    } else{
        if (!verifyNif(first_arg)) {
            printf("%s: %s\n", first_arg, EINVALID_NIF);
            return 0;
        }
        *nif = atoi(first_arg);
        *name = extractName(buf + 2 + strlen(first_arg));
    }

    if (*name == NULL) return 0;

    if (strcmp(*name, "error") == 0)
        return -1;

    return 1;
}




/**
 * Finds an invoice by its number using linear search.
 * @param sys system state
 * @param number invoice number to search for
 * @return index in invoices array if found, -1 otherwise
 */
int findInvoice(System *sys, int number) {
	int i;
	for (i = 0; i < sys->num_invoices; i++) {
		if (sys->invoices[i].number == number)
			return i;
	}
	return -1;
}

/**
 * Adds a new invoice to the system.
 * Dynamically expands the invoices array if the capacity is reached.
 * @param sys system state
 * @param nif client nif
 * @param name client name string
 * @param value total value of the invoice
 * @param num_items number of items in the invoice
 */
void addInvoice(System *sys, int nif, char *name, 
                double value, int num_items) {
    /* Expand the array if it reaches the maximum capacity */
    if (sys->num_invoices >= sys->max_invoices) {
        sys->max_invoices *= 2;
        sys->invoices = realloc(sys->invoices,
                                sizeof(Invoice) * sys->max_invoices);
        checkMemory(sys->invoices);
    }

    sys->invoices[sys->num_invoices].nif = nif;
    sys->invoices[sys->num_invoices].num_items = num_items;
    sys->invoices[sys->num_invoices].value = value;
    sys->invoices[sys->num_invoices].number = sys->next_invoice_number++;

    /* Allocate memory for the client name string */
    sys->invoices[sys->num_invoices].client_name = malloc(strlen(name) + 1);
    checkMemory(sys->invoices[sys->num_invoices].client_name);
    strcpy(sys->invoices[sys->num_invoices].client_name, name);

    sys->num_invoices++;
}


/**
 * Deletes an invoice from the system and updates global totals.
 * Frees the allocated memory for the client name before shifting elements.
 * @param sys system state
 * @param idx_invoice index of the invoice to be deleted
 */
void deleteInvoice(System *sys, int idx_invoice) {
    int i;

    /* Free the dynamically allocated name */
    free(sys->invoices[idx_invoice].client_name);

    /* Update system statistics */
    sys->total_revenue -= sys->invoices[idx_invoice].value;
    if (sys->total_revenue < 0.001)
        sys->total_revenue = 0.0;

    sys->total_items_sold -= sys->invoices[idx_invoice].num_items;

    /* Shift remaining invoices */
    for(i = idx_invoice; i < sys->num_invoices - 1; i++)
        sys->invoices[i] = sys->invoices[i + 1];

    sys->num_invoices--;
}





/**
 * Prints the invoice details in the format:
 * <num_items> <total_value> <invoice_number>
 * @param invoice invoice to be printed
 */
void printInvoiceCommandF(Invoice invoice) {
    printf("%d %.2lf %d\n",
           invoice.num_items,
           invoice.value,
           invoice.number);
}

/**
 * Processes the 'f' command to finalize or cancel an invoice.
 * Updates product sales, system revenue, and creates an invoice if valid.
 * Clears the shopping cart and frees allocated memory after execution.
 * @param sys system state
 * @param buf input line buffer
 */
void commandF(System *sys, char *buf) {
    int nif, idx_product, idx_iva, i, num_items = 0;
    double total_price = 0.0;
    char *name;
    int clientInfo = readClient(buf, &nif, &name);

    /* If there was an error cancel invoice and restore stock */
    if (clientInfo == -1) {
        free(name);
        for (i = 0; i < sys->cart_size; i++)
            sys->products[sys->cart[i].product_index].stock += sys->cart[i].quantity;
        
        destroyCart(sys);
    } else if (clientInfo) {
        /* Loop through cart to update totals and sales */
        for (i = 0; i < sys->cart_size; i++) {
            if (sys->cart[i].quantity > 0)
                num_items += sys->cart[i].quantity;

            idx_product = sys->cart[i].product_index;
            idx_iva = indexIva(sys->products[idx_product].iva);

            total_price += calculatePrice(sys->products[idx_product].price, 
                                          sys->cart[i].quantity, 
                                          sys->iva_taxes[idx_iva]);

            sys->products[idx_product].sold += sys->cart[i].quantity;
        }
        sys->total_revenue += total_price;
        sys->total_items_sold += num_items;

        addInvoice(sys, nif, name, total_price, num_items);
        printInvoiceCommandF(sys->invoices[sys->num_invoices - 1]);
        
        free(name);
        destroyCart(sys);
    }
}




/**
 * Partitions the invoices array for Sort.
 * Orders by client name and uses invoice number as a tie-breaker.
 * @param sys system state
 * @param start partition start index
 * @param end partition end index (pivot)
 * @return pivot final position
 */
int partition(System *sys, int start, int end) {
    int i = start - 1, j, comp;
    char *pivot = sys->invoices[end].client_name;
    int pivot_number = sys->invoices[end].number;
    Invoice aux;

    for (j = start; j < end; j++) {
        comp = strcmp(sys->invoices[j].client_name, pivot);
        
        /* Sort by name, then by number if names are equal */
        if (comp < 0 || 
                (comp == 0 && sys->invoices[j].number < pivot_number)) {
            i++;
            aux = sys->invoices[i];
            sys->invoices[i] = sys->invoices[j];
            sys->invoices[j] = aux;
        }
    }

    /* Move pivot to its final sorted place */
    aux = sys->invoices[i + 1];
    sys->invoices[i + 1] = sys->invoices[end];
    sys->invoices[end] = aux;

    return i + 1;
}

/**
 * Sorts invoices.
 * @param sys system state
 * @param start starting index
 * @param end ending index
 */
void sortInvoices(System *sys, int start, int end) {
    if (start >= end)
        return;

    int i = partition(sys, start, end);
    sortInvoices(sys, start, i - 1);
    sortInvoices(sys, i + 1, end);
}

/**
 * Prints the invoice details in the format:
 * <invoice_number> <total_value> <client_name>
 * @param invoice invoice to be printed
 */
void printInvoiceCommandC(Invoice invoice) {
    printf("%d %.2lf %s\n",
            invoice.number,
            invoice.value,
            invoice.client_name);
}

/**
 * Lists all invoices for a specific client.
 * If no invoices are found, prints an error message.
 * @param sys system state
 * @param name client name to search for
 */
void listClientInvoices(System *sys, char *name) {
    int i, found = 0;

    for (i = 0; i < sys->num_invoices; i++) {
        if (strcmp(sys->invoices[i].client_name, name) == 0) {
            found = 1;
            printInvoiceCommandC(sys->invoices[i]);
        }
    }

    if (!found)
        printf("%s: %s\n", name, ENO_CLIENT);
}

/**
 * Sorts and lists every invoice stored in the system.
 * @param sys system state
 */
void listAllInvoices(System *sys) {
    int i;

    sortInvoices(sys, 0, sys->num_invoices - 1);

	for (i = 0; i < sys->num_invoices; i++)
		printInvoiceCommandC(sys->invoices[i]);
}

/**
 * Processes the 'c' command to list invoices.
 * If no arguments are given, lists all; otherwise, lists by client name.
 * @param sys system state
 * @param buf input line buffer
 */
void commandC(System *sys, char *buf) {
    char first_arg[MAXLINE];
    int arg_read = sscanf(buf + 2, "%s", first_arg);
    
    if (arg_read <= 0) {
        listAllInvoices(sys);
    } else {
        char *name = extractName(buf + 2);
        if (name == NULL) return;

        listClientInvoices(sys, name);
        free(name);
    }
}




/**
 * Prints the invoice details in the format and then deletes it:
 * <total_value> <nif> <client_name>
 * @param sys system state
 * @param idx_invoice index of the invoice to be printed
 */
void printAndDeleteInvoice(System *sys, int idx_invoice) {
    printf("%.2lf %d %s\n",
        sys->invoices[idx_invoice].value,
        sys->invoices[idx_invoice].nif,
        sys->invoices[idx_invoice].client_name);
    deleteInvoice(sys, idx_invoice);
}

/**
 * Processes the 'd' command for a single product.
 * Verifies if the product is in the cart before updating stock.
 * If the stock reaches zero, the product is removed from the system.
 * @param sys system state
 * @param ean product EAN code
 * @param quantity amount to remove from stock
 */
void commandDProduct(System *sys, char ean[MAXLINE], int quantity) {
    int idx_product = findProduct(sys, ean);

    if (idx_product == -1) {
        printf("%s: %s\n", ean, ENO_PRODUCT);
        return;
    }

    int idx_cart = findProductInCart(sys, idx_product);
    if (idx_cart != -1 && sys->cart[idx_cart].quantity > 0) {
        puts(EPRODUCT_IN_USE);
        return;
    }

    int final_stock = sys->products[idx_product].stock - quantity;
    if (final_stock < 0) {
        puts(EINVALID_QTY);
        return;
    }

    sys->products[idx_product].stock = final_stock;
    printf("%d %s\n", final_stock, sys->products[idx_product].description);

    if (final_stock == 0)
        deleteProduct(sys, idx_product);
}

/**
 * Processes the 'd' command to delete products or invoices.
 * If two arguments are given, it updates product stock.
 * Otherwise, it removes an invoice by its number.
 * @param sys system state
 * @param buf input line buffer
 */
void commandD(System *sys, char *buf) {
    char first_arg[MAXLINE], sec_arg[MAXLINE];
    int arg_read = sscanf(buf + 2, "%s %s", first_arg, sec_arg);

    if (arg_read == 2) {
        if(!verifyEan(first_arg)) {
            puts(EINVALID_EAN);
            return;
        }

        int quantity = atoi(sec_arg);
        if (quantity <= 0) {
            puts(EINVALID_QTY);
            return;
        }
        commandDProduct(sys, first_arg, quantity);
    } else {
        int number = atoi(first_arg);
        int idx_invoice = findInvoice(sys, number);
        
        if (idx_invoice == -1) {
            printf("%d: %s\n", number, ENO_INVOICE);
            return;
        }
        printAndDeleteInvoice(sys, idx_invoice);
    }
}




/**
 * Frees all dynamically allocated invoice data.
 * @param sys System state
 */
void destroyInvoices(System *sys) {
    int i;
    for (i = 0; i < sys->num_invoices; i++)
        free(sys->invoices[i].client_name);
    free(sys->invoices);
}