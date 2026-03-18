#include "invoices.h"

int findInvoice(System *sys, int number) {
	int i;
	for (i = 0; i < sys->num_invoices; i++) {
		if (sys->invoices[i].number == number)
			return i;
	}
	return -1;
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

printInvoice(Invoice invoice) {
    printf("%d %.2lf %d\n",
        invoice.number,
        invoice.value,
        invoice.client_name);
}