#include "invoices.h"

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

    if (len != 9 )
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
        *quote_end = strchr(quote_start + 1, '"');
        len_name = quote_end - quote_start - 1;
        *name = malloc(len_name + 1);

        if (name == NULL) {
            puts(ENO_MEMORY);
            exit(0);
        }
        strncpy(name, quote_start + 1, len_name);
        name[len_name] = '\0';

        return name;
    } else {
        sscanf(buf, "%s", bufcpy);
        len_name = strlen(bufcpy);
        *name = malloc(len_name + 1);

        if (name == NULL) {
            puts(ENO_MEMORY);
            exit(0);
        }
        strcpy(name, bufcpy);
        return name;
    }
}


void addInvoice(System *sys, int nif, char *name, double value, int num_items) {
    sys->invoices = realloc(sizeof(Invoice) * (sys->num_invoices + 1));
    if (sys->invoices == NULL) {
        puts(ENO_MEMORY);
        exit(0);
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
    for(i = 0; i < sys->num_invoices - 1; i++)
        sys->invoices[i] = sys->invoices[i + 1];
    sys->num_invoices--;
}


void printInvoice(Invoice invoice) {
    printf("%d %.2lf %s\n",
        invoice.number,
        invoice.value,
        invoice.client_name);
}


int readClient(char buf[MAXLINE], int *nif, char **name) {
    char first_arg[MAXLINE];
    int info_read = sscanf(buf + 2, "%s", first_arg);

    if (info_read <= 0) {
        *nif = 999999999;
        *name = malloc(strlen("Cliente final") + 1);

        if (*name == NULL) {
            puts(ENO_MEMORY);
            exit(0);
        }   
        strcpy(*name, "Cliente final");
    } else {
        if (strcmp(first_arg, "error") == 0)
            return -1;
        else if (verifyNif(first_arg)) {
            *nif = atoi(first_arg);
            char *rest = buf + 2 + strlen(first_arg);
            *name = extractName(rest);
        } else {
            *nif = 999999999;
            *name = extractName(buf + 2);
        }
    }
    return 1;
}