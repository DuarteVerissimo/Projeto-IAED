#include "invoices.h"
#include "cart.h"
#include "iva.h"
#include "products.h"

int findInvoice(System *sys, int number) {
	int i;
	for (i = 0; i < sys->num_invoices; i++) {
		if (sys->invoices[i].number == number)
			return i;
	}
	return -1;
}

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


char *extractName(char *buf) {
    char *quote_start = strchr(buf, '"');
    char *quote_end, *name;
    int len_name;
    char bufcpy[MAXLINE];

    if (quote_start != NULL) {
        quote_end = strchr(quote_start + 1, '"');
        if (quote_end == NULL || !isalpha(quote_start[1])) {
            puts (EINVALID_NAME);
            return NULL;
        }
        len_name = quote_end - quote_start - 1;
        name = malloc(len_name + 1);
        checkMemory(name);
        strncpy(name, quote_start + 1, len_name);
        name[len_name] = '\0';
        return name;
    } else {
        sscanf(buf, "%s", bufcpy);
        if (!isalpha(bufcpy[0])) {
            puts(EINVALID_NAME);
            return NULL;
        }        
        len_name = strlen(bufcpy);
        name = malloc(len_name + 1);
        checkMemory(name);
        strcpy(name, bufcpy);
        return name;
    }
}

void setStandardClient(int *nif, char **name) {
    *nif = DEFAULT_NIF;
    *name = malloc(strlen(DEFAULT_CLIENT_NAME) + 1);
    checkMemory(*name);
    strcpy(*name, DEFAULT_CLIENT_NAME);
}

int readClient(char buf[MAXLINE], int *nif, char **name) {
    char first_arg[MAXLINE], sec_arg[MAXLINE];
    int arg_read = sscanf(buf + 2, "%s %s", first_arg, sec_arg);

    if (arg_read <= 0) {
        setStandardClient(*nif, **name);
        return 1;
    } else {
        if (first_arg[0] == '"' || arg_read == 1) {
            *nif = DEFAULT_NIF;
            *name = extractName(buf + 2);
        } else{
            if (!verifyNif(first_arg)) {
                printf("%s: %s\n", first_arg, EINVALID_NIF);
                return 0;
            }
            *nif = atoi(first_arg);
            if (arg_read == 1) {
                *name = malloc(strlen(DEFAULT_CLIENT_NAME) + 1);
                checkMemory(*name);
                strcpy(*name, DEFAULT_CLIENT_NAME);
            } else
                *name = extractName(buf + 2 + strlen(first_arg));
        }
    }
    if (*name == NULL) return 0;
    if (strcmp(*name, "error") == 0)
        return -1;
    return 1;
}


void addInvoice(System *sys, int nif, char *name, double value, int num_items) {
    if (sys->num_invoices >= sys->max_invoices) {
        sys->max_invoices *= 2;
        sys->invoices = realloc(sys->invoices, sizeof(Invoice) * sys->max_invoices);
    }
    sys->invoices[sys->num_invoices].nif = nif;
    sys->invoices[sys->num_invoices].num_items = num_items;
    sys->invoices[sys->num_invoices].number = sys->next_invoice_number;
    sys->next_invoice_number++;
    sys->invoices[sys->num_invoices].value = value;
    sys->invoices[sys->num_invoices].client_name = malloc(strlen(name) + 1);
    strcpy(sys->invoices[sys->num_invoices].client_name, name);
    sys->num_invoices++;
}


void deleteInvoice(System *sys, int idx_invoice) {
    int i;
    free(sys->invoices[idx_invoice].client_name);
    for(i = idx_invoice; i < sys->num_invoices - 1; i++)
        sys->invoices[i] = sys->invoices[i + 1];
    sys->num_invoices--;
}


void printInvoiceCommandF(Invoice invoice) {
    printf("%d %.2lf %d\n",
        invoice.num_items,
        invoice.value,
        invoice.number);
}

void commandF(System *sys, char buf[MAXLINE]) {
    int nif, idx_product, i, num_items = 0;
    double total_price = 0.0;
    char *name;
    int clientInfo = readClient(buf, &nif, &name);
    if (clientInfo == -1) {
        free(name);
        for (i = 0; i < sys->cart_size; i++)
            sys->products[sys->cart[i].product_index].stock += sys->cart[i].quantity;
        emptyCart(sys);
    } else if (clientInfo) {
        for (i = 0; i < sys->cart_size; i++) {
            if (sys->cart[i].quantity > 0)
                num_items += sys->cart[i].quantity;
            idx_product = sys->cart[i].product_index;
            total_price += calculatePrice(sys->products[idx_product].price, 
                sys->cart[i].quantity, sys->iva_taxes[sys->products[idx_product].iva - 'A']);
            sys->products[idx_product].sold += sys->cart[i].quantity;
        }
        //sys->total_revenue += total_price;
        //sys->total_items_sold += num_items;
        addInvoice(sys, nif, name, total_price, num_items);
        printInvoiceCommandF(sys->invoices[sys->num_invoices - 1]);
        free(name);
        emptyCart(sys);
    }
}

void printInvoiceCommandC(Invoice invoice) {
    printf("%d %.2lf %s\n",
        invoice.number,
        invoice.value,
        invoice.client_name);
}

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

void listAllInvoices(System *sys) {
    int i;
    sortInvoices(sys, 0, sys->num_invoices - 1);
	for (i = 0; i < sys->num_invoices; i++)
		printInvoiceCommandC(sys->invoices[i]);
}


void sortInvoices(System *sys, int start, int end) {
    int i;
    
    if (start >= end)
        return;
    i = partition(sys, start, end);
    sortInvoices(sys, start, i - 1);
    sortInvoices(sys, i + 1, end);
}

int partition(System *sys, int start, int end) {
    int i = start - 1, j, comp;
    char *pivot = sys->invoices[end].client_name;
    int pivot_number = sys->invoices[end].number;

    for (j = start; j < end; j++) {
        comp = strcmp(sys->invoices[j].client_name, pivot);
        if (comp < 0 || (comp == 0 && sys->invoices[j].number < pivot_number)) {
            i++;
            Invoice aux = sys->invoices[i];
            sys->invoices[i] = sys->invoices[j];
            sys->invoices[j] = aux;
        }
    }
    Invoice aux = sys->invoices[i + 1];
    sys->invoices[i + 1] = sys->invoices[end];
    sys->invoices[end] = aux;
    return i + 1;
}

void commandC(System *sys, char buf[MAXLINE]) {
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

void commandDProduct(System *sys, char ean[MAXLINE], int quantity) {
    int idx_product = findProduct(sys, ean);
    int idx_cart = findProductInCart(sys, idx_product);

    if (idx_product == -1) {
        printf("%s: %s\n", ean, ENO_PRODUCT);
        return;
    }

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

void printAndDeleteInvoice(System *sys, int idx_invoice) {
    printf("%.2lf %d %s\n",
        sys->invoices[idx_invoice].value,
        sys->invoices[idx_invoice].nif,
        sys->invoices[idx_invoice].client_name);
    deleteInvoice(sys, idx_invoice);
}

void commandD(System *sys, char buf[MAXLINE]) {
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

void destroyInvoices(System *sys) {
    int i;
    for (i = 0; i < sys->num_invoices; i++)
        free(sys->invoices[i].client_name);
    free(sys->invoices);
}